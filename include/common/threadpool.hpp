#ifndef __COMMON_THREADPOOL_HPP__
#define __COMMON_THREADPOOL_HPP__

#include "common/misc.hpp"
#include "common/queue.hpp"
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <stdio.h>

namespace COMMON {

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
