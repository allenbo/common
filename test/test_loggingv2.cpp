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
    LOG(FATAL) << "FATAL INFO" << " " << 3.3 << std::endl;
}
