#pragma once

#include <string>

// Case-insensitive string comparisons

struct anycase_char_traits : public std::char_traits<char> {
    static bool eq(char a, char b) { return std::tolower(a) == std::tolower(b); }
    static bool ne(char a, char b) { return std::tolower(a) != std::tolower(b); }
    static bool lt(char a, char b) { return std::tolower(a) < std::tolower(b); }

    static int compare(const char* a, const char* b, size_t length) {
        for (size_t idx = 0; idx < length; ++idx) {
            if (eq(a[idx], b[idx])) {
                continue;
            }
            else if (lt(a[idx], b[idx])) {
                return -1;
            }
            else { // gt
                return 1;
            }
        }

        return 0;
    }

    static const char* find(const char* str, size_t length, char ch) {
        for (size_t idx = 0; idx < length; ++idx) {
            if (eq(str[idx], ch)) {
                return &str[idx];
            }
        }

        return nullptr;
    }
};

typedef std::basic_string<char, anycase_char_traits> anycase_string;
typedef std::basic_string_view<char, anycase_char_traits> anycase_stringview;