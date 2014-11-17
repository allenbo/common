#include "common/all.hpp"
#include <iostream>
#include <assert.h>
using namespace COMMON;

int main() {
    std::string path = "test/test.cpp";
    std::pair<std::string, std::string> rst = Path::split(path);
    assert(rst.first == "test");
    assert(rst.second == "test.cpp");
    std::string dir = rst.first;
    std::string base = rst.second;
    assert(Path::join(dir, base) == path);
    assert(Path::join(dir + "/", base) == path);
    assert(Path::join(dir, "/" + base) == "/" + base);
}

