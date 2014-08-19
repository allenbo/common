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
    int i = property.get_int("int_value", 0);
    double r = property.get_real("real_value", 0.0);
    std::string s = property.get("string_value");
    bool b = property.get_bool("bool_value", true);
    std::cout << "int:" << i << " real:" << r << " string:" << s << " bool:" << b << std::endl;
    fin.close();
}
