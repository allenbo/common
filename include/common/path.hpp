#ifndef __COMMON_PATH_HPP__
#define __COMMON_PATH_HPP__

#include <stdlib.h>
#include <string.h>

#include <utility>

namespace COMMON {
    
class Path {
    public:
        static inline std::string sep() {
#ifdef Win32
            static std::string _sep = "\\";
#else
            static std::string _sep = "/";
#endif
            return _sep;
        }

        static std::pair<std::string, std::string> split(const std::string& path) {
            std::string::size_type pos = path.rfind(Path::sep());
            std::string dir = "", base = "";
            if (pos != std::string::npos) {
                dir = path.substr(0, pos);
                pos ++;
                base = path.substr(pos);
            } else {
                base = path.substr(0);
            }
            return make_pair(dir, base);
        }

        static std::string join(const std::string& dir, const std::string & base) {
            std::string rst = "";
            if (VarString::starts_with(base, Path::sep())) {
                rst = base;
            } else {
                if (VarString::ends_with(dir, Path::sep())) {
                    rst = dir + base;
                } else {
                    rst = dir + Path::sep() + base;
                }
            }
            return rst;
        }
};

}

#endif
