#pragma once

#include "app/application.h"

#include <string>
#include <string_view>
#include <vector>


class Main
{
public:
    RCalc::AppConfig parse_args(int argc, char** pp_argv);

    static RCalc::Application& get_app() { return *p_application; }

private:
    const char* p_name;
    static RCalc::Application* p_application;

    void print_help(bool print_description = true);
    void print_version();
    void print_licenses();

    void list_renderers();
    const char* validate_renderer(const std::vector<std::string_view>::const_iterator& name);

    RCalc::AppConfig parse_args_internal(const std::vector<std::string_view>& args);

    friend int main(int, char**);
};