#include "common/loggingv2.hpp"
#include <iostream>
#include <vector>

using namespace COMMON;

namespace std {
std::ostream& operator<<(std::ostream& out, const std::vector<int>& vec){
    out << "[ ";
    for (auto& e : vec) {
      out << e << " ";
    }
    out << "]";
    return out;
}
}

int main() {
    LOG(INFO) << "Logging INFO " << 3 << " " << 2.3 << std::endl;
    std::vector<int> a = {1, 2, 3};
    LOG(INFO) << a << std::endl;
    CHECK_EQ(3, 3) << "3 != 4" << std::endl;
    CHECK_GT(4, 3);
    CHECK_GE(4, 3);
    CHECK_NE(4, 3);
    CHECK_LT(3, 4);
    CHECK_LE(3, 4);
    CHECK_LE(3, 3);
    int* ap = nullptr;
    CHECK_NOTNULL(ap);
}
