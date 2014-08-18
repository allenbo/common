#include "common/all.hpp"

#include <stdio.h>

using namespace COMMON;

class MyThread: public Thread {
    public:
        void run() {
            printf("In mythread\n");
        }
};

int main() {
    MyThread thread[10];
    for (int i = 0; i < 10; i ++ ) {
        thread[i].start();
    }

    for (int i = 0; i < 10; i ++) {
        thread[i].join();
    }
}
