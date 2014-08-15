#include "common/all.hpp"
#include <stdio.h>
using namespace COMMON;

void print_task(void* args) {
    long id = (long)args;
    for(int i = 0; i < 100; i ++ ) {
        fprintf(stderr, "%ld:%d\n", id, i);
    }
    fprintf(stderr, "Finish %ldth task\n", id);
}

int main() {
    ThreadPool pool;
    int n_task = 100;
    for(int i = 0; i < n_task; i ++ ) {
        pool.add(print_task, (void*)i);
        fprintf(stderr, "Add %dth task to pool\n", i);
    }
    pool.destroy(TP_POSTPONED_SHUTDOWN);
}
