#include "common/all.hpp"
#include <stdio.h>
#include <sys/time.h>
using namespace common;

void microsleep(int us) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = us * 1000; // 1us
    nanosleep(&ts, NULL);
}

static int global_value = 0;
Mutex m;

void vv_func() {
    ScopeLock _(&m);
    global_value += 1;
    microsleep(10);
}

void test_vv() {
    global_value = 0;
    ThreadPool pool;
    int n_task = 100;
    for(int i = 0; i < n_task; i ++) {
        pool.add(vv_func);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(global_value == n_task);
}

int rv_func() {
    return 1;
}

void test_rv() {
    global_value = 0;
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(rv_func, &rsts[i]);
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
      sum += rsts[i];
    }
    assert(sum == n_task);
}

void va1_func(int a) {
    ScopeLock _(&m);
    global_value += a;
    microsleep(10);
}

void test_va1() {
    global_value = 0;
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;

    for(int i = 0; i < n_task; i ++) {
        pool.add(va1_func, i);
        sum += i;
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(sum == global_value);
}

int ra1_func(int a) {
    return a;
}

void test_ra1() {
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(ra1_func, i, &rsts[i]);
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == i);
    }
}

void va2_func(int a, int b) {
    ScopeLock _(&m);
    global_value += a + b;
    microsleep(10);
}

void test_va2() {
    global_value = 0;
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;

    for(int i = 0; i < n_task; i ++) {
        pool.add(va2_func, i, i * 2);
        sum += i + i * 2;
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(sum == global_value);
}

int ra2_func(int a, int b) {
    return a + b;
}

void test_ra2() {
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(ra2_func, i, i * 2, &rsts[i]);
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == i + i * 2);
    }
}


void va3_func(int a, int b, int c) {
    ScopeLock _(&m);
    global_value += a + b + c;
}

void test_va3() {
    global_value = 0;
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;

    for(int i = 0; i < n_task; i ++) {
        pool.add(va3_func, i, i * 2, i * 3);
        sum += i + i * 2 + i * 3;
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(sum == global_value);
}

int ra3_func(int a, int b, int c) {
    return a + b + c;
}

void test_ra3() {
    ThreadPool pool;

    int sum = 0;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(ra3_func, i, i * 2, i * 3, &rsts[i]);
    }

    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == i + i * 2 + i * 3);
    }
}

void test_func() {
    test_vv();
    test_rv();
    test_va1();
    test_ra1();
    test_va2();
    test_ra2();
    test_va3();
    test_ra3();
}

class Tester {
    public:
        Tester(){
            reset();
        }
        void reset(int sum = 0) { _sum = sum; }
        int sum() const { return _sum; }

        void vcv() {
            ScopeLock _(&_m);
            _sum += 1;
            microsleep(10);
        }

        int rcv() {
            return _sum;
        }

        void vca1(int a) {
            ScopeLock _(&_m);
            _sum += a;
            microsleep(10);
        }

        int rca1(int a) {
            return a;
        }

        void vca2(int a, int b) {
            ScopeLock _(&_m);
            _sum += a + b;
            microsleep(10);
        }

        int rca2(int a, int b) {
            return a + b;
        }
    private:
        Mutex _m;
        int _sum;
};

void test_vcv() {
    Tester test;
    ThreadPool pool;
    int n_task = 100;
    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::vcv, &test);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(test.sum() == n_task);
}

void test_rcv() {
    Tester test;
    test.reset(10);

    ThreadPool pool;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::rcv, &test, &rsts[i]);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == test.sum());
    }
}

void test_vca1() {
    Tester test;
    ThreadPool pool;
    int n_task = 100;
    int sum = 0;
    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::vca1, &test, i);
        sum += i;
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(test.sum() == sum);
}

void test_rca1() {
    Tester test;
    test.reset(10);

    ThreadPool pool;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::rca1, &test, i, &rsts[i]);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == i);
    }
}

void test_vca2() {
    Tester test;
    ThreadPool pool;
    int n_task = 100;
    int sum = 0;
    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::vca2, &test, i, i * 2);
        sum += i + i * 2;
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    assert(test.sum() == sum);
}

void test_rca2() {
    Tester test;
    test.reset(10);

    ThreadPool pool;
    const int n_task = 100;
    int rsts[n_task];

    for(int i = 0; i < n_task; i ++) {
        pool.add(&Tester::rca2, &test, i, i * 2,  &rsts[i]);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
    for(int i = 0; i < n_task; i ++) {
        assert(rsts[i] == i + i * 2);
    }
}

void test_method() {
    test_vcv();
    test_rcv();
    test_vca1();
    test_rca1();
    test_vca2();
    test_rca2();
}

int main() {
    test_func();
    test_method();
}
