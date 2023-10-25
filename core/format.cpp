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


std::string fmt_mat2(Mat2 value) {
    int col0_len = std::max(
        snprintf(nullptr, 0, "%g", value[0].x),
        snprintf(nullptr, 0, "%g", value[0].y)
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%g", value[1].x),
        snprintf(nullptr, 0, "%g", value[1].y)
    );

    return fmt(
        "| %*g, %*g |\n| %*g, %*g |",
        col0_len, value[0].x, 
        col1_len, value[1].x,
        col0_len, value[0].y,
        col1_len, value[1].y
    );
}

std::string fmt_mat3(Mat3 value) {
    int col0_len = std::max(
        snprintf(nullptr, 0, "%g", value[0].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[0].y),
            snprintf(nullptr, 0, "%g", value[0].z)
        )
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%g", value[1].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[1].y),
            snprintf(nullptr, 0, "%g", value[1].z)
        )
    );
    int col2_len = std::max(
        snprintf(nullptr, 0, "%g", value[2].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[2].y),
            snprintf(nullptr, 0, "%g", value[2].z)
        )
    );

    return fmt(
        "| %*g, %*g, %*g |\n| %*g, %*g, %*g |\n| %*g, %*g, %*g |",
        col0_len, value[0].x,
        col1_len, value[1].x,
        col2_len, value[2].x,
        col0_len, value[0].y,
        col1_len, value[1].y,
        col2_len, value[2].y,
        col0_len, value[0].z,
        col1_len, value[1].z,
        col2_len, value[2].z
    );
}

std::string fmt_mat4(Mat4 value) {
    int col0_len = std::max(
        snprintf(nullptr, 0, "%g", value[0].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[0].y),
            std::max(
                snprintf(nullptr, 0, "%g", value[0].z),
                snprintf(nullptr, 0, "%g", value[0].w)
            )
        )
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%g", value[1].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[1].y),
            std::max(
                snprintf(nullptr, 0, "%g", value[1].z),
                snprintf(nullptr, 0, "%g", value[1].w)
            )
        )
    );
    int col2_len = std::max(
        snprintf(nullptr, 0, "%g", value[2].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[2].y),
            std::max(
                snprintf(nullptr, 0, "%g", value[2].z),
                snprintf(nullptr, 0, "%g", value[2].w)
            )
        )
    );
    int col3_len = std::max(
        snprintf(nullptr, 0, "%g", value[3].x),
        std::max(
            snprintf(nullptr, 0, "%g", value[3].y),
            std::max(
                snprintf(nullptr, 0, "%g", value[3].z),
                snprintf(nullptr, 0, "%g", value[3].w)
            )
        )
    );

    return fmt(
        "| %*g, %*g, %*g, %*g |\n| %*g, %*g, %*g, %*g |\n| %*g, %*g, %*g, %*g |\n| %*g, %*g, %*g, %*g |",
        col0_len, value[0].x,
        col1_len, value[1].x,
        col2_len, value[2].x,
        col3_len, value[3].x,
        col0_len, value[0].y,
        col1_len, value[1].y,
        col2_len, value[2].y,
        col3_len, value[3].y,
        col0_len, value[0].z,
        col1_len, value[1].z,
        col2_len, value[2].z,
        col3_len, value[3].z,
        col0_len, value[0].w,
        col1_len, value[1].w,
        col2_len, value[2].w,
        col3_len, value[3].w
    );
}

}