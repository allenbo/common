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

void test_async_func() {
    test_async_vv();
    test_async_rv();
    test_async_va1();
    test_async_ra1();
    test_async_va2();
    test_async_ra2();
    test_async_va3();
    test_async_ra3();
}


class Tester {
    public:
        Tester() {
            reset();
        }

        void reset() {
            _val = 0;
        }

        void vc() {
            _val = global_int_value;
        }

        int rc() {
            return global_int_value;
        }

        void vca1(int a){
            _val = a;
        }

        int rca1(int a) {
            return  a;
        }

        void vca2(int a, int b) {
            _val += a + b;
        }

        int rca2(int a, int b) {
            return _val + a + b;
        }

        int value() const { return _val;}
    private:
        int _val;
};

void test_async_vc() {
    Tester test;
    AsyncMethod async;
    async.start_async(&Tester::vc, &test);
    async.wait();
    assert(test.value() == global_int_value);
}

void test_async_rc() {
    Tester test;
    global_int_value = 10;
    int r = 0;
    AsyncMethod async;
    async.start_async(&Tester::rc, &test, &r);
    async.wait();
    assert(r == global_int_value);
}

void test_async_vca1() {
    Tester test;
    int a = 10;
    AsyncMethod async;
    async.start_async(&Tester::vca1, &test, a);
    async.wait();
    assert(test.value() == a);
}

void test_async_rca1() {
    Tester test;
    int a = 10;
    int r = 0;
    AsyncMethod async;
    async.start_async(&Tester::rca1, &test, a, &r);
    async.wait();
    assert(r == a);
}

void test_async_vca2() {
    Tester test;
    int a = 10;
    int b = 20;
    AsyncMethod async;
    async.start_async(&Tester::vca2, &test, a, b);
    async.wait();
    assert(test.value() == a + b);
}

void test_async_rca2() {
    Tester test;
    int a = 10;
    int b = 20;
    int r = 0;
    AsyncMethod async;
    async.start_async(&Tester::rca2, &test, a, b, &r);
    async.wait();
    assert(r == a + b);
}

void test_async_method() {
    test_async_vc();
    test_async_rc();
    test_async_vca1();
    test_async_rca1();
    test_async_vca2();
    test_async_rca2();
}

void test_async() {
    test_async_func();
    test_async_method();
    fprintf(stderr, "Finish async test\n");
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
    fprintf(stderr, "Finish thread test\n");
    assert(a == 10);
    test_async();
}
