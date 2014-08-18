#ifndef __COMMON_THREADING_HPP__
#define __COMMON_THREADING_HPP__

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include "common/misc.hpp"

namespace COMMON {

class Lock {
    CLASS_NOCOPY(Lock)
    public:
        Lock(){}
        virtual void lock() = 0;
        virtual void unlock() = 0;
        virtual ~Lock() {}
};

class Mutex : public Lock {
    CLASS_NOCOPY(Mutex)
    public:
        Mutex() {
            pthread_mutex_init(&_lock, NULL);
        }
        ~Mutex() {
            pthread_mutex_destroy(&_lock);
        }
        void lock() {
            pthread_mutex_lock(&_lock);
        }
        void unlock() {
            pthread_mutex_unlock(&_lock);
        }
    private:
        pthread_mutex_t _lock;
        friend class Condition;
};

class ScopeLock {
    public:
        ScopeLock() {
            _lock = new Mutex();
            _lock->lock();
        }
        ~ScopeLock() {
            _lock->unlock();
            delete _lock;
        }
    private:
        Lock* _lock;
};

class Condition {
    CLASS_NOCOPY(Condition)
    public:
        Condition(Mutex* lock) {
            _lock = lock;
            pthread_cond_init(&_cond, NULL);
        }
        ~Condition(){
            pthread_cond_destroy(&_cond);
        }

        void wait() {
            pthread_cond_wait(&_cond, &_lock->_lock); 
        }

        void notify() {
            pthread_cond_signal(&_cond);
        }

        void notify_all() {
            pthread_cond_broadcast(&_cond);
        }
    private:
        pthread_cond_t _cond;
        Mutex* _lock;
};

class Runable {
    public:
        virtual void run() = 0;
};

class Thread: public Runable {
    public:
        Thread(const Runable& runable) {
            _context = &runable;
            _lock = new Mutex();
        }
        Thread() {
            _context = NULL;
            _lock = new Mutex();
        }

        virtual void run() {
            printf("In thread\n");
        }

        void start() {
            if (_active == true) {
                return;
            }
            _active = true;
            if (_context == NULL) {
                _context = this;
            }
            pthread_create(&_thread_id, NULL, &Thread::_run_thread, (void*)_context);
        }

        void join() {
            pthread_join(_thread_id, NULL);
            _active = false;
        }

        bool is_active() {
            return _active;
        }

        virtual ~Thread() {
            join();
            delete _lock;
        }

    private:
        static void* _run_thread(void* context) {
            Runable * self = (Runable*)context;
            self->run();
            pthread_exit(NULL);
            return NULL;
        }

        pthread_t _thread_id;
        const Runable * _context;

        bool _active;
        Lock* _lock;
};

}

#endif
