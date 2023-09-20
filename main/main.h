#pragma once

#include "app/application.h"
#include "core/error.h"
#include "platform/platform.h"

#include <string_view>
#include <vector>


class Main
{
public:
    AppConfig parse_args(int argc, char** pp_argv);
    Result<> run(AppConfig config);

private:
    const char* p_name;

    void print_help(bool print_description = true);
    void print_version();

    AppConfig parse_args_internal(const std::vector<std::string_view>& args);
};