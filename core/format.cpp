#include "format.h"

#include <algorithm>
#include <cstring>
#include <cstdarg>

namespace RCalc {

void fmt_real(Real value, int precision, RealFmtContainer buf) {
    if (std::log10(value) < 0) {
        std::fill(buf.begin(), buf.end(), '\0');
        snprintf(buf.data(), buf.size(), "%.*f", precision, value);

        RealFmtContainer::iterator decimal_point = std::find(buf.begin(), buf.end(), '.');
        RealFmtContainer::iterator first_nonzero_decimal = std::find_if(decimal_point, buf.end(), [](char ch) { return (ch != '0') && std::isdigit(ch); });
        RealFmtContainer::iterator last_nonzero = std::find_if(buf.rbegin(), RealFmtContainer::reverse_iterator { decimal_point }, [](char ch) { return (ch != '0') && std::isdigit(ch); }).base();

        int dist_fnz = (int)std::distance(decimal_point, first_nonzero_decimal);
        int dist_lnz = (int)std::distance(decimal_point, last_nonzero);

        std::fill(buf.begin(), buf.end(), '\0');
        if (dist_fnz < precision) {
            if (dist_lnz <= precision) {
                snprintf(buf.data(), buf.size(), "%.*f", std::min(dist_lnz - 1, precision), value);
                return;
            }

            snprintf(buf.data(), buf.size(), "%.*e", std::min(dist_lnz - dist_fnz - 1, precision), value);
            return;
        }
    }

    std::fill(buf.begin(), buf.end(), '\0');
    snprintf(buf.data(), buf.size(), "%.*g", precision, value);
}

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


std::string fmt_col_vec2(Vec2 value, int precision) {
    char buf[2][RealFmtContainerSize];
    fmt_real(value.x, precision, { buf[0] });
    fmt_real(value.y, precision, { buf[1] });
    
    int col_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        snprintf(nullptr, 0, "%s", buf[1])
    );

    return fmt(
        "| %*s |\n| %*s |",
        col_len, buf[0],
        col_len, buf[1]
    );
}

std::string fmt_col_vec3(Vec3 value, int precision) {
    char buf[3][RealFmtContainerSize];
    fmt_real(value.x, precision, { buf[0] });
    fmt_real(value.y, precision, { buf[1] });
    fmt_real(value.z, precision, { buf[2] });

    int col_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[1]),
            snprintf(nullptr, 0, "%s", buf[2])
        )
    );

    return fmt(
        "| %*s |\n| %*s |\n| %*s |",
        col_len, buf[0],
        col_len, buf[1],
        col_len, buf[2]
    );
}

std::string fmt_col_vec4(Vec4 value, int precision) {
    char buf[4][RealFmtContainerSize];
    fmt_real(value.x, precision, { buf[0] });
    fmt_real(value.y, precision, { buf[1] });
    fmt_real(value.z, precision, { buf[2] });
    fmt_real(value.w, precision, { buf[3] });

    int col_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[1]),
            std::max(
                snprintf(nullptr, 0, "%s", buf[2]),
                snprintf(nullptr, 0, "%s", buf[3])
            )
        )
    );

    return fmt(
        "| %*s |\n| %*s |\n| %*s |\n| %*s |",
        col_len, buf[0],
        col_len, buf[1],
        col_len, buf[2],
        col_len, buf[3]
    );
}


std::string fmt_mat2(Mat2 value, int precision, bool one_line) {
    char buf[4][RealFmtContainerSize];
    fmt_real(value[0].x, precision, { buf[0] });
    fmt_real(value[1].x, precision, { buf[1] });
    fmt_real(value[0].y, precision, { buf[2] });
    fmt_real(value[1].y, precision, { buf[3] });

    if (one_line) {
        return fmt(
            "mat2([%s, %s], [%s, %s])",
            buf[0], buf[1],
            buf[2], buf[3]
        );
    }

    int col0_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        snprintf(nullptr, 0, "%s", buf[2])
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%s", buf[1]),
        snprintf(nullptr, 0, "%s", buf[3])
    );

    return fmt(
        "| %*s, %*s |\n| %*s, %*s |",
        col0_len, buf[0], 
        col1_len, buf[1],
        col0_len, buf[2],
        col1_len, buf[3]
    );
}

std::string fmt_mat3(Mat3 value, int precision, bool one_line) {
    char buf[9][RealFmtContainerSize];
    fmt_real(value[0].x, precision, { buf[0] });
    fmt_real(value[1].x, precision, { buf[1] });
    fmt_real(value[2].x, precision, { buf[2] });
    fmt_real(value[0].y, precision, { buf[3] });
    fmt_real(value[1].y, precision, { buf[4] });
    fmt_real(value[2].y, precision, { buf[5] });
    fmt_real(value[0].z, precision, { buf[6] });
    fmt_real(value[1].z, precision, { buf[7] });
    fmt_real(value[2].z, precision, { buf[8] });

    if (one_line) {
        return fmt(
            "mat3([%s, %s, %s], [%s, %s, %s], [%s, %s, %s])",
            buf[0], buf[1], buf[2],
            buf[3], buf[4], buf[5],
            buf[6], buf[7], buf[8]
        );
    }

    int col0_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[3]),
            snprintf(nullptr, 0, "%s", buf[6])
        )
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%s", buf[1]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[4]),
            snprintf(nullptr, 0, "%s", buf[7])
        )
    );
    int col2_len = std::max(
        snprintf(nullptr, 0, "%s", buf[2]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[5]),
            snprintf(nullptr, 0, "%s", buf[8])
        )
    );

    return fmt(
        "| %*s, %*s, %*s |\n| %*s, %*s, %*s |\n| %*s, %*s, %*s |",
        col0_len, buf[0],
        col1_len, buf[1],
        col2_len, buf[2],
        col0_len, buf[3],
        col1_len, buf[4],
        col2_len, buf[5],
        col0_len, buf[6],
        col1_len, buf[7],
        col2_len, buf[8]
    );
}

std::string fmt_mat4(Mat4 value, int precision, bool one_line) {
    char buf[16][RealFmtContainerSize];
    fmt_real(value[0].x, precision, { buf[0] });
    fmt_real(value[1].x, precision, { buf[1] });
    fmt_real(value[2].x, precision, { buf[2] });
    fmt_real(value[3].x, precision, { buf[3] });
    fmt_real(value[0].y, precision, { buf[4] });
    fmt_real(value[1].y, precision, { buf[5] });
    fmt_real(value[2].y, precision, { buf[6] });
    fmt_real(value[3].y, precision, { buf[7] });
    fmt_real(value[0].z, precision, { buf[8] });
    fmt_real(value[1].z, precision, { buf[9] });
    fmt_real(value[2].z, precision, { buf[10] });
    fmt_real(value[3].z, precision, { buf[11] });
    fmt_real(value[0].w, precision, { buf[12] });
    fmt_real(value[1].w, precision, { buf[13] });
    fmt_real(value[2].w, precision, { buf[14] });
    fmt_real(value[3].w, precision, { buf[15] });

    if (one_line) {
        return fmt(
            "mat4([%s, %s, %s, %s], [%s, %s, %s, %s], [%s, %s, %s, %s], [%s, %s, %s, %s])",
            buf[0], buf[1], buf[2], buf[3],
            buf[4], buf[5], buf[6], buf[7],
            buf[8], buf[9], buf[10], buf[11],
            buf[12], buf[13], buf[14], buf[15]
        );
    }

    int col0_len = std::max(
        snprintf(nullptr, 0, "%s", buf[0]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[4]),
            std::max(
                snprintf(nullptr, 0, "%s", buf[8]),
                snprintf(nullptr, 0, "%s", buf[12])
            )
        )
    );
    int col1_len = std::max(
        snprintf(nullptr, 0, "%s", buf[1]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[5]),
            std::max(
                snprintf(nullptr, 0, "%s", buf[9]),
                snprintf(nullptr, 0, "%s", buf[13])
            )
        )
    );
    int col2_len = std::max(
        snprintf(nullptr, 0, "%s", buf[2]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[6]),
            std::max(
                snprintf(nullptr, 0, "%s", buf[10]),
                snprintf(nullptr, 0, "%s", buf[14])
            )
        )
    );
    int col3_len = std::max(
        snprintf(nullptr, 0, "%s", buf[3]),
        std::max(
            snprintf(nullptr, 0, "%s", buf[7]),
            std::max(
                snprintf(nullptr, 0, "%s", buf[11]),
                snprintf(nullptr, 0, "%s", buf[15])
            )
        )
    );

    return fmt(
        "| %*s, %*s, %*s, %*s |\n| %*s, %*s, %*s, %*s |\n| %*s, %*s, %*s, %*s |\n| %*s, %*s, %*s, %*s |",
        col0_len, buf[0],
        col1_len, buf[1],
        col2_len, buf[2],
        col3_len, buf[3],
        col0_len, buf[4],
        col1_len, buf[5],
        col2_len, buf[6],
        col3_len, buf[7],
        col0_len, buf[8],
        col1_len, buf[9],
        col2_len, buf[10],
        col3_len, buf[11],
        col0_len, buf[12],
        col1_len, buf[13],
        col2_len, buf[14],
        col3_len, buf[15]
    );
}

}