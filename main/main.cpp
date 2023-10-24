#include "core/logger.h"
#include "core/version.h"
#include "main/main.h"
#include "modules/register_modules.h"
#include "app/renderers/renderer.h"
#include "app/application.h"

#include <iostream>
#include <string_view>
#include <sstream>
#include <cstring>


int main(int argc, char** pp_argv)
{
    Main m;

    RCalc::AppConfig config = m.parse_args(argc, pp_argv);
    initialize_modules();

    Result<RCalc::Application*> res = RCalc::Application::create(config);
    
    if (!res) {
        std::stringstream ss;
        ss << res.unwrap_err();
        Logger::log_err("%s", ss.str().c_str());
    } else {
        RCalc::Application* p_application = res.unwrap();

        while (!p_application->close_requested()) {
            p_application->step();
        }

        p_application->cleanup();
    }

    cleanup_modules();

    return res ? 0 : 255;
}


RCalc::AppConfig Main::parse_args(int argc, char** pp_argv)
{
    p_name = pp_argv[0];
    std::vector<std::string_view> args;
    args.reserve(static_cast<size_t>(argc) - 1);

    for (int idx = 1; idx < argc; ++idx)
    {
        args.emplace_back(pp_argv[idx]);
    }

    RCalc::AppConfig config = parse_args_internal(args);

    if (config.quiet) {
        Logger::configure(Logger::LOG_ERROR);
    } else if (config.verbose) {
        Logger::configure(Logger::LOG_VERBOSE);
    }

    return config;
}


std::string get_full_version_string()
{
    return std::string { VERSION_FULL_BUILD } + "." + std::string { VERSION_HASH, 6 };
}


void Main::print_help(bool print_description)
{
    if (print_description)
    {
        std::cout << VERSION_NAME << " version " << get_full_version_string() << ".\n";
        std::cout << "Created by Patrick Gelvin, " << VERSION_YEAR << "\n";
        std::cout << "A lightweight RPN calculator\n";
        std::cout << "\n";
    }
    std::cout << "Usage: " << p_name << " [options]\n";
    std::cout << "\n";
    
    std::cout << "General Options:\n";
    std::cout << "  -h, --help                  Display this help message.\n";
    std::cout << "  -v, --version               Display the version string.\n";
    std::cout << "  --verbose                   Enable verbose logging.\n";
    std::cout << "  --quiet                     Silence all logging, except for errors.\n";
    std::cout << "  --list-renderers            List the available renderers.\n";
    std::cout << "\n";

    std::cout << "Run Options:\n";
    std::cout << "  --renderer <renderer>       Select an available renderer.\n";
    std::cout << "\n";
}


void Main::print_version()
{
    std::cout << get_full_version_string() << "\n";
}


void Main::list_renderers()
{
    std::cout << "Supported renderers:\n";

    for (const char* renderer : RCalc::Renderer::get_enabled_renderers()) {
        std::cout << "\t" << renderer << "\n";
    }

    std::cout << "\n";
    std::cout << "You can choose a renderer to use with the --renderer option.\n";
    std::cout << "  e.g. " << p_name << " --renderer " << RCalc::Renderer::default_renderer << "\n";
}


const char* Main::validate_renderer(const std::vector<std::string_view>::const_iterator& name)
{
    for (const char* renderer : RCalc::Renderer::get_enabled_renderers()) {
        if (name->compare(renderer) == 0) {
            return renderer;
        }
    }

    std::cout << "Error: \"" << *name << "\" is not a supported renderer.\n\n";
    list_renderers();
    exit(255);
}


RCalc::AppConfig Main::parse_args_internal(const std::vector<std::string_view>& args)
{
    RCalc::AppConfig config{};

    for (auto arg = args.begin(); arg < args.end(); arg++)
    {
        if (
            arg->compare("-h") == 0 ||
            arg->compare("--help") == 0
        ) {
            print_help();
            exit(0);
        }

        if (
            arg->compare("-v") == 0 ||
            arg->compare("--version") == 0
        ) {
            print_version();
            exit(0);
        }


        if (arg->compare("--verbose") == 0)
        {
            config.verbose = true;
            continue;
        }

        if (arg->compare("--quiet") == 0)
        {
            config.quiet = true;
            continue;
        }

        if (arg->compare("--list-renderers") == 0)
        {
            list_renderers();
            exit(0);
        }

        if (arg->compare("--renderer") == 0)
        {
            config.renderer_name = validate_renderer(++arg);
            continue;
        }

        
        std::cout << "Unrecognized option: " << *arg << "\n";
        print_help(false);
        exit(0);
    }

    if (config.verbose && config.quiet)
    {
        std::cout << "Error: Cannot specify both --quiet and --verbose at the same time.\n";
        exit(255);
    }

    if (config.renderer_name.empty()) {
        config.renderer_name = RCalc::Renderer::default_renderer;
    }

    return config;
}