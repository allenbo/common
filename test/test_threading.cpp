#include "common/all.hpp"
#include <stdio.h>

using namespace COMMON;

class MyThread: public Thread {
    public:
        MyThread(int* pa, Mutex* m) : _pa(pa), _m(m){}
        void run() {
            ScopeLock _(_m);
            *_pa += 1;
            sleep(1);
        }
    private:
        Mutex* _m;
        int* _pa;
};

static int global_int_value = 0;

void vv_func() {
    global_int_value++;
}
void test_async_vv() {
    AsyncMethod async;
    async.start_async(vv_func);
    async.wait();
    assert(global_int_value == 1);
}

int rv_func() {
    return 1;
}

void test_async_rv() {
    AsyncMethod async;
    int a = 0;
    async.start_async(rv_func, &a);
    async.wait();
    assert(a == 1);
}

void va1_func(int a) {
    global_int_value = a;
}

void test_async_va1() {
    int a = 10;
    AsyncMethod async;
    async.start_async(va1_func, a);
    async.wait();
    assert(global_int_value == a);
}

int ra1_func(int a) {
    return a * global_int_value;
}

void test_async_ra1() {
    int a = 5;
    int r = 0;
    AsyncMethod async;
    async.start_async(ra1_func, a, &r);
    async.wait();
    assert(global_int_value * a == r);
}

void va2_func(int a, int b) {
    global_int_value = a + b;
}

void test_async_va2() {
    int a = 1, b = 2;
    AsyncMethod async;
    async.start_async(va2_func, a, b);
    async.wait();
    assert(global_int_value == a + b);
}

int ra2_func(int a , int b) {
    return a + b;
}

void test_async_ra2() {
    int a = 1, b = 2;
    int r = 0;
    AsyncMethod async;
    async.start_async(ra2_func, a, b, &r);
    async.wait();
    assert(r == a + b);
}

void va3_func(int a, int b, int c) {
    global_int_value = a + b + c;
}

void test_async_va3() {
  int a = 1, b = 2, c = 3;
    AsyncMethod async;
    async.start_async(va3_func, a, b, c);
    async.wait();
    assert(global_int_value == a + b + c);
}

int ra3_func(int a, int b, int c){
    return a + b + c;
}

void test_async_ra3(){
    int a = 1, b = 2, c = 3;
    int r = 0;

    AsyncMethod async;
    async.start_async(ra3_func, a, b, c, &r);
    async.wait();
    assert(r == a + b + c);
}

void test_async() {
  test_async_vv();
  test_async_rv();
  test_async_va1();
  test_async_ra1();
  test_async_va2();
  test_async_ra2();
  test_async_va3();
  test_async_ra3();
}
int main() {
    int a = 0;
    Mutex m;
    MyThread thread[10] = {
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m),
        MyThread(&a, &m)
    };
    for (int i = 0; i < 10; i ++ ) {
        thread[i].start();
    }

    for (int i = 0; i < 10; i ++) {
        thread[i].join();
    }
    assert(a == 10);
    test_async();
}
