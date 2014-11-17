#ifndef __COMMON_LOGGING_HPP__
#define __COMMON_LOGGING_HPP__

#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>

namespace COMMON {

#define LOGGING_DEBUG 0
#define LOGGING_INFO 1
#define LOGGING_WARN 2
#define LOGGING_ERROR 3
#define LOGGING_FATAL 4

#ifdef DEBUG
#undef DEBUG
#endif
#define DEBUG LOGGING_DEBUG

#ifdef INFO
#undef INFO
#endif
#define INFO LOGGING_INFO

#ifdef WARN
#undef WARN
#endif
#define WARN LOGGING_WARN

#ifdef ERROR
#undef ERROR
#endif
#define ERROR LOGGING_ERROR

#ifdef FATAL
#undef FATAL
#endif
#define FATAL LOGGING_FATAL

#ifdef NDEBUG
#define LIMIT LOGGING_INFO
#else
#define LIMIT LOGGING_DEBUG
#endif

#define LOG(severity) COMPACT_LOG_MESSAGE(severity)
#define COMPACT_LOG_MESSAGE(s) LOG_MESSAGE_ ## s(__FILE__, __LINE__)

#define LOG_MESSAGE_0(file, lineno) LogMessage::New(0, file, lineno)
#define LOG_MESSAGE_1(file, lineno) LogMessage::New(1, file, lineno)
#define LOG_MESSAGE_2(file, lineno) LogMessage::New(2, file, lineno)
#define LOG_MESSAGE_3(file, lineno) LogMessage::New(3, file, lineno)
#define LOG_MESSAGE_4(file, lineno) LogMessage::New(4, file, lineno)

class LogMessage {
    public:
        LogMessage(int severity, const char* filename, const int lineno) {
            _severity = severity;
            _filename = filename;
            _lineno = lineno;
            _stream = std::shared_ptr<std::ostringstream>(new std::ostringstream());
            (*this) << "[" << baseFilename(filename) << "|" << lineno << "] ";
        }

        LogMessage(const LogMessage& other) {
            _severity = other._severity;
            _filename = other._filename;
            _lineno = other._lineno;
            _stream = other._stream;
        }

        ~LogMessage() {
            if (_severity >= LIMIT) {
              std::cerr << _stream->str();
            }
            if (_severity == FATAL) {
              abort();
            }
        }

        static LogMessage New(int severity, const char* filename, const int lineno) {
            return LogMessage(severity, filename, lineno);
        }

        template<class T>
        LogMessage& operator<<(const T& t) {
            if (_severity >= LIMIT)
                (*_stream) << t;
            return *this;
        }

        typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
        typedef CoutType& (*StandardEndLine)(CoutType&);

        LogMessage& operator<<(StandardEndLine manip) {
            if (_severity >= LIMIT)
                manip(*_stream);
            return *this;
        }
    private:
        int _severity;
        const char* _filename;
        int _lineno;
        std::shared_ptr<std::ostringstream> _stream;

        const char* baseFilename(const char* path) {
            const char* p = path + strlen(path);
            while (p != path) {
                if (*p == '/') {
                    return p + 1;
                }
                p --;
            }
            return path;
        }
};

}

#endif
