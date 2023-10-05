#pragma once

#include "app/application.h"

#include <string>
#include <string_view>
#include <vector>


class Main
{
public:
    RCalc::AppConfig parse_args(int argc, char** pp_argv);

private:
    const char* p_name;

    void print_help(bool print_description = true);
    void print_version();

    void list_renderers();
    const char* validate_renderer(const std::vector<std::string_view>::const_iterator& name);

    RCalc::AppConfig parse_args_internal(const std::vector<std::string_view>& args);
};