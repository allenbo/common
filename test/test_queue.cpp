#include "common/all.hpp"
#include <stdlib.h>

#include <iostream>

using namespace common;

void* put(void* args) {
    Queue<int>* q = (Queue<int>*)args;
    int capacity = q->capacity();
    for(int i = 0; i < 10000; i ++ ) {
        for(int j = 0; j< capacity; j ++ ) {
            q->push(j);
            fprintf(stderr, "Put %d\n", j);
        }
    }

    for(int j = 0; j< capacity; j ++ ) {
        q->push(-1);
        fprintf(stderr, "Put -1\n");
    }
    fprintf(stderr, "put thread exit\n");
    pthread_exit(NULL);
    return NULL;
}

void* get(void* args) {
    pthread_t id = pthread_self();
    Queue<int>* q = (Queue<int>*)args;
    for(;;) {
        int item = q->pop();
        fprintf(stderr, "thread %ld get %d\n", (long)id, item);
        if (item == -1) break;
    }
    fprintf(stderr, "get thread %ld exit\n", (long)id);
    pthread_exit(NULL);
    return NULL;
}


int main() {
    int n_cpu = get_ncpu();
    std::cout << "There are " << n_cpu << std::endl;
    int q_size = n_cpu * 2;
    Queue<int> queue(q_size);
    
    pthread_t put_thread;
    pthread_create(&put_thread, NULL, put, (void*)&queue);

    pthread_t get_threads[n_cpu - 2];
    for(int i = 0; i < n_cpu - 1; i ++ ) {
        pthread_create(&get_threads[i], NULL, get, (void*)&queue);
    }
    pthread_join(put_thread, NULL);
    for(int i = 0; i < n_cpu - 1; i ++ ) {
        pthread_join(get_threads[i], NULL);
    }
}
