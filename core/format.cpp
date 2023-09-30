#include "format.h"

#include <cstring>
#include <cstdarg>

namespace RCalc {

std::string fmt(const char* p_format, ...) {
    va_list args;
    va_list args2;

    va_start(args, p_format);
    va_copy(args2, args);

    int size = vsnprintf(nullptr, 0, p_format, args) + 1;
    va_end(args);

    if (size <= 512) {
        static char buf[512];
        memset(&buf[0], 0, 512);
        vsnprintf(&buf[0], 512, p_format, args2);
        va_end(args2);
        return std::string(&buf[0]);
    }
    // I tried snprintf'ing directly into a string and it caused The Problems I don't feel like solving right now.
    char* buf = (char*)malloc(sizeof(char) * size);
    memset(&buf[0], 0, size);
    vsnprintf(&buf[0], size, p_format, args2);
    va_end(args2);
    std::string str(&buf[0]);
    free(buf);
    return str;
}

}