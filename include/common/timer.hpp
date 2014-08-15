#ifndef __COMMON_TIMER_HPP__
#define __COMMON_TIMER_HPP__

#include <string.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

#include "common/misc.hpp"

namespace COMMON {


class Timer {
    CLASS_NOCOPY(Timer)
    public:
        Timer() {
            _start = get_time_in_micro();
        }

        void start() {
            _start = get_time_in_micro();
        }

        int64_t elapsed() {
            _end = get_time_in_micro();
            return _end - _start;
        }

        static int64_t get_time_in_micro()
        {
#ifdef WIN32
             /* Windows */
            FILETIME ft;
            LARGE_INTEGER li;

            /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
             * to a LARGE_INTEGER structure. */
            GetSystemTimeAsFileTime(&ft);
            li.LowPart = ft.dwLowDateTime;
            li.HighPart = ft.dwHighDateTime;

            int64_t ret = li.QuadPart;
            ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
            ret /= 1000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

            return ret;
#else
            /* Linux */
            struct timeval tv;

            gettimeofday(&tv, NULL);

            int64_t ret = tv.tv_usec;
            /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
            ret += (tv.tv_sec * 1000000);
            return ret;
#endif
        } 
    private:
        int64_t _start;
        int64_t _end;
};

class ScopeTimer {
    public:
        ScopeTimer(const char* name)
            :_timer(), _name(name) 
        {
        }

        ~ScopeTimer() {
            int64_t elapsed = _timer.elapsed();
            fprintf(stderr, "[%s]: %fs\n", _name.c_str(), elapsed * 1.0 / 1000000);
        }

    private:
        Timer _timer;
        std::string  _name;
};

}

#endif
