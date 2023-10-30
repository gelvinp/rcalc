#pragma once

#include <vector>

namespace RCalc {

struct HelpText {
    struct HelpSection {
        const char* header;
        const char* text;
    };

    static const char* program_info;
    static const std::vector<HelpSection> sections;
};

}