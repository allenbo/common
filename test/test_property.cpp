#include "common/all.hpp"
#include <stdlib.h>
#include <fstream>
#include <iostream>
using namespace COMMON;

void usage() {
    std::cerr << "./prog filename" << std::endl;
    exit(-1);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        usage();
    }

    std::ifstream fin(argv[1]);
    Property property(fin);
    fin.close();
}
