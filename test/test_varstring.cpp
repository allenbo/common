#include "common/varstring.hpp"
#include <cassert>

using namespace COMMON;

int main() {
    std::string left = "abc";
    std::string right = "def";
    VarString vs;
    vs.append(left).append(right);
    assert(vs.toString() == left+right);
}
