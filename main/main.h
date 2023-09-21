#pragma once

#include "app/application.h"

#include <string_view>
#include <vector>


class Main
{
public:
    void parse_args(int argc, char** pp_argv);

private:
    const char* p_name;

    void print_help(bool print_description = true);
    void print_version();

    RCalc::AppConfig parse_args_internal(const std::vector<std::string_view>& args);
};