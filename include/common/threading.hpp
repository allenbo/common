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
        ScopeLock(Mutex* lock) {
            _lock = lock;
            _lock->lock();
        }
        ~ScopeLock() {
            _lock->unlock();
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
        virtual ~Runable(){}
};



class Thread: public Runable {
    public:
        Thread(const Runable& runable) {
            _active = false;
            _context = &runable;
            _lock = new Mutex();
        }
        Thread() {
            _active = false;
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

class AsyncMethod {
    public:
        AsyncMethod(): _runable(NULL), _thread(NULL) {
        }
        void start_async(void (*fn) (void) ) {
            typedef void (*FUNC) (void);
            class VVRunable : public Runable {
                public:
                    VVRunable(FUNC fn) : _fn(fn) {
                    }
                    void run() {
                        _fn();
                    }
                private:
                    FUNC _fn;
            };
            _runable = new VVRunable(fn);
            _thread = new Thread(*_runable);
            _thread->start();
        }

        template<class R>
        void start_async(R (*fn) (void), R* r) {
            typedef R (*FUNC) (void);

            class RVRunable : public Runable {
                public:
                    RVRunable(FUNC fn, R* r) : _fn(fn), _r(r){
                    }
                    void run() {
                        *_r = _fn();
                    }
                private:
                    FUNC _fn;
                    R* _r;
            };

            _runable = new  RVRunable(fn, r);
            _thread = new Thread(*_runable);
            _thread->start();
        }

        template<class A1>
        void start_async(void (*fn) (A1) , A1 a1) {
            typedef void (*FUNC) (A1);

            class VA1Runable : public Runable {
                public:
                    VA1Runable(FUNC fn, A1 a1) : _fn(fn), _a1(a1){
                    }
                    void run() {
                        _fn(_a1);
                    }
                private:
                    FUNC _fn;
                    A1 _a1;
            };

            _runable = new  VA1Runable(fn, a1);
            _thread = new Thread(*_runable);
            _thread->start();
        }
        template<class A1, class R>
        void start_async(R (*fn) (A1), A1 a1, R*r) {
            typedef R (*FUNC) (A1);

            class RA1Runable : public Runable {
                public:
                    RA1Runable(FUNC fn, A1 a1, R* r) : _fn(fn), _a1(a1), _r(r) {
                    }

                    void run() {
                        *_r = _fn(_a1);
                    }

                private:
                    FUNC _fn;
                    A1 _a1;
                    R* _r;
            };

            _runable = new RA1Runable(fn, a1, r);
            _thread = new Thread(*_runable);
            _thread->start();
        }

        template<class A1, class A2>
        void start_async(void (*fn) (A1, A2) , A1 a1, A2 a2) {
            typedef void (*FUNC) (A1, A2);

            class VA2Runable : public Runable {
                public:
                    VA2Runable(FUNC fn, A1 a1, A2 a2) : _fn(fn), _a1(a1), _a2(a2) {
                    }
                    void run() {
                        _fn(_a1, _a2);
                    }
                private:
                    FUNC _fn;
                    A1 _a1;
                    A2 _a2;
            };

            _runable = new  VA2Runable(fn, a1, a2);
            _thread = new Thread(*_runable);
            _thread->start();
        }
        template<class A1, class A2, class R>
        void start_async(R (*fn) (A1, A2), A1 a1, A2 a2, R*r) {
            typedef R (*FUNC) (A1, A2);

            class RA2Runable : public Runable {
                public:
                    RA2Runable(FUNC fn, A1 a1, A2 a2, R* r) : _fn(fn), _a1(a1), _a2(a2), _r(r) {
                    }

                    void run() {
                        *_r = _fn(_a1, _a2);
                    }

                private:
                    FUNC _fn;
                    A1 _a1;
                    A2 _a2;
                    R* _r;
            };

            _runable = new RA2Runable(fn, a1, a2, r);
            _thread = new Thread(*_runable);
            _thread->start();
        }

        template<class A1, class A2, class A3>
        void start_async(void (*fn) (A1, A2, A3) , A1 a1, A2 a2, A3 a3) {
            typedef void (*FUNC) (A1, A2, A3);

            class VA3Runable : public Runable {
                public:
                    VA3Runable(FUNC fn, A1 a1, A2 a2, A3 a3) : _fn(fn), _a1(a1), _a2(a2), _a3(a3){
                    }
                    void run() {
                        _fn(_a1, _a2, _a3);
                    }
                private:
                    FUNC _fn;
                    A1 _a1;
                    A2 _a2;
                    A3 _a3;
            };

            _runable = new  VA3Runable(fn, a1, a2, a3);
            _thread = new Thread(*_runable);
            _thread->start();
        }
        template<class A1, class A2, class A3, class R>
        void start_async(R (*fn) (A1, A2, A3), A1 a1, A2 a2, A3 a3, R*r) {
            typedef R (*FUNC) (A1, A2, A3);

            class RA3Runable : public Runable {
                public:
                    RA3Runable(FUNC fn, A1 a1, A2 a2, A3 a3, R* r) : _fn(fn), _a1(a1), _a2(a2), _a3(a3), _r(r) {
                    }

                    void run() {
                        *_r = _fn(_a1, _a2, _a3);
                    }

                private:
                    FUNC _fn;
                    A1 _a1;
                    A2 _a2;
                    A3 _a3;
                    R* _r;
            };

            _runable = new RA3Runable(fn, a1, a2, a3, r);
            _thread = new Thread(*_runable);
            _thread->start();
        }

        void wait() {
          _thread->join();
        }

        ~AsyncMethod() {
            _thread->join();
            if (_runable) delete _runable;
            if (_thread) delete _thread;
        }

    private:
        Runable *_runable;
        Thread *_thread;
};

}

#endif
