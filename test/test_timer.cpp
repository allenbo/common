#include "common/all.hpp"
#include <stdio.h>

using namespace common;

int main() {
    Timer t1;
    int sum = 0;
    for(int i = 0; i < 1000000; i ++ ) {
        sum += i;
    }
    int64_t elapsed = t1.elapsed();
    fprintf(stderr, "Sum from 0 to 999999 takes %lld us\n", elapsed);

    {
        ScopeTimer _("Test");
        int sum = 0;
        for(int i = 0; i < 1000000; i ++ ) {
            sum += i;
        }
    }
}
