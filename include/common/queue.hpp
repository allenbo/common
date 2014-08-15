#ifndef __COMMON_QUEUE_HPP__
#define __COMMON_QUEUE_HPP__

#include "common/misc.hpp"
#include <pthread.h>
#include <deque>
#include <stdio.h>

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


}

#endif
