#ifndef __COMMON_THREADING_HPP__
#define __COMMON_THREADING_HPP__

#include <pthread.h>
#include <stdio.h>
#include <time.h>

namespace COMMON {

class Runable {
    public:
        virtual void run() = 0;
};

class Thread: public Runable {
    public:
        Thread(const Runable& runable) {
            _context = &runable;
        }
        Thread() {
            _context = NULL;
        }

        virtual void run() {
            printf("In thread\n");
        }

        void start() {
            if (_context == NULL) {
                _context = this;
            }
            pthread_create(&_thread_id, NULL, &Thread::_run_thread, (void*)_context);
            struct timespec ts;
            ts.tv_sec = 0;
            ts.tv_nsec = 1000; // 1us
            nanosleep(&ts, NULL);
            /*
            pthread_join(_thread_id, NULL);
            pthread_detach(_thread_id);
            */

        }

        void join() {
            pthread_join(_thread_id, NULL);
        }

        virtual ~Thread() {
            join();
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
};

}

#endif
