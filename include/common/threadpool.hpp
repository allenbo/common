#ifndef __COMMON_THREADPOOL_HPP__
#define __COMMON_THREADPOOL_HPP__

#include "common/misc.hpp"
#include <time.h>
#include <pthread.h>
#include <deque>
#include <assert.h>
#include <stdio.h>

#define CLASS_NOCOPY(class) \
    private: \
        class& operator=(class& other); \
        class(const class& other);

namespace COMMON {

template<class T>
class Queue {
    public:
        Queue(size_t size = 0)
            :_size(size)
        {
            pthread_mutex_init(&_lock, NULL);
            pthread_cond_init(&_empty_cond, NULL);
            pthread_cond_init(&_full_cond, NULL);
        }
        ~Queue() {
            _queue.clear();
            pthread_mutex_destroy(&_lock);
            pthread_cond_destroy(&_empty_cond);
            pthread_cond_destroy(&_full_cond);
        }

        void push(const T& item) {
            pthread_mutex_lock(&_lock);
            size_t queue_size = _queue.size();
            if (_size != 0 && queue_size == _size) {
                //fprintf(stderr, "Queue full, wait for consuming\n");
                pthread_cond_wait(&_full_cond, &_lock);
                _queue.push_back(item);
            } else {
                _queue.push_back(item);
                pthread_cond_signal(&_empty_cond);
            }
            pthread_mutex_unlock(&_lock);
        }

        bool try_pop(T* item) {
            pthread_mutex_lock(&_lock);
            size_t queue_size = _queue.size();
            bool rst = false;
            if (queue_size != 0) {
                *item = _queue.front();
                _queue.pop_front();
                rst = true;
                if (queue_size == _size - 1) {
                    pthread_cond_signal(&_full_cond);
                }
            }
            pthread_mutex_unlock(&_lock);
            return rst;
        }
        
        T pop() {
            pthread_mutex_lock(&_lock);
            while(_queue.empty()) {
                pthread_cond_wait(&_empty_cond, &_lock);
            }

            T item = _queue.front();
            _queue.pop_front();
            if (_queue.size() == _size - 1) {
                pthread_cond_signal(&_full_cond);
            }
            pthread_mutex_unlock(&_lock);
            return item;
        }

        size_t size() {
            pthread_mutex_lock(&_lock);
            size_t size = _queue.size();
            pthread_mutex_unlock(&_lock);
            return size;
        }

        size_t capacity() const { return _size; }
        void clear() { _queue.clear(); }

    private:
        std::deque<T> _queue;
        pthread_mutex_t _lock;
        pthread_cond_t _empty_cond;
        pthread_cond_t _full_cond;
        size_t _size;
};

typedef void (*func) (void*);

struct ThreadFunc {
    func function;
    void* args;
};

#define TP_IMMEDIATE_SHUTDOWN 0
#define TP_POSTPONED_SHUTDOWN 1

class ThreadPool {
    CLASS_NOCOPY(ThreadPool)
    public:
        ThreadPool(size_t pool_size = get_ncpu(), size_t queue_size = 2 * get_ncpu())
            :_pool_size(pool_size), _task_queue(queue_size)
        {
            pthread_mutex_init(&_lock, NULL);
            _stop = false;
            _shutdown = -1;
            assert(_pool_size > 0);
            _pool = (pthread_t*) malloc(sizeof(pthread_t) * _pool_size);
            assert(_pool != NULL);
            _active_count = 0;

            pthread_mutex_lock(&_lock);
            int i;
            for(i = 0; i < _pool_size; i ++ ) {
                int ret = pthread_create(&_pool[i], NULL, _run_thread, (void*)this);
                if (ret != 0) {
                    pthread_mutex_unlock(&_lock);
                    destroy();
                    break;
                } else {
                    _active_count ++;
                }
            }
            if (i == _pool_size) pthread_mutex_unlock(&_lock);
        }

        void destroy(int shutdown = TP_IMMEDIATE_SHUTDOWN) {
            if (shutdown != TP_IMMEDIATE_SHUTDOWN && shutdown != TP_POSTPONED_SHUTDOWN) {
                _shutdown = TP_IMMEDIATE_SHUTDOWN;
            } else {
                _shutdown = shutdown;
            }
            for(int i = 0; i < _active_count; i ++ ) {
                pthread_join(_pool[i], NULL);
            }
            _task_queue.clear();
            free(_pool);
            _pool = NULL;
            _stop = true;
        }

        bool add(func function, void* args) {
            if (_stop == true) return false;
            ThreadFunc tf;
            tf.function = function;
            tf.args = args;
            _task_queue.push(tf);
            return true;
        }

        ~ThreadPool() {
            if (!_stop) {
                destroy();
            }
        }
    private:
        Queue<ThreadFunc> _task_queue;
        pthread_mutex_t _lock;
        pthread_t* _pool;
        int _shutdown;
        size_t _pool_size;
        bool _stop;
        size_t _active_count;

        static void* _run_thread(void* context) {
            ThreadPool* self = (ThreadPool*)context;

            for(;;) {
                if (self->_shutdown == TP_IMMEDIATE_SHUTDOWN) {
                    break;
                } else if (self->_shutdown == TP_POSTPONED_SHUTDOWN && self->_task_queue.size() == 0) {
                    //fprintf(stderr, "Postponed shutdown");
                    break;
                }

                ThreadFunc job;
                if (self->_task_queue.try_pop(&job)) {
                    job.function(job.args);
                } else {
                    struct timespec ts;
                    ts.tv_sec = 0;
                    ts.tv_nsec = 1000; // 1us
                    nanosleep(&ts, NULL);
                }
            }
            pthread_exit(NULL);
            return NULL;
        }
};

}

#endif
