#include "core/logger.h"
#include "core/version.h"
#include "main/main.h"
#include "modules/register_modules.h"
#include "app/renderers/renderer.h"
#include "app/application.h"
#include "assets/license.gen.h"
#include "core/memory/allocator.h"
#include "app/displayable/displayable.h"
#include "app/dump/dump.h"
#include "tests/test_main.h"

#include <iostream>
#include <string_view>
#include <sstream>
#include <cstring>


RCalc::Application* Main::p_application = nullptr;


int main(int argc, char** pp_argv)
{
    RCalc::Allocator::setup();
    RCalc::initialize_modules();
    
    Main m;
    RCalc::AppConfig config = m.parse_args(argc, pp_argv);

    #ifdef TESTS_ENABLED
    if (config.test_config && config.test_config->run_tests) {
        int res = RCalc::test_main(config);
        
        RCalc::cleanup_modules();
        RCalc::Allocator::cleanup();

        return res;
    }
    #endif

    Main::p_application = RCalc::Allocator::create<RCalc::Application>();;
    Main::get_app().set_max_stack_size(100000);
    RCalc::Result<> res = Main::get_app().early_init(config);
    
    if (!res) {
        std::stringstream ss;
        ss << res.unwrap_err();
        RCalc::Logger::log_err("%s", ss.str().c_str());
    } else {
        res = Main::get_app().init();
        if (res) { Main::get_app().run(); }

        RCalc::Allocator::set_noop_free(true);
        Main::get_app().cleanup();
        RCalc::Allocator::destroy(Main::p_application);
    }

    RCalc::cleanup_modules();
    RCalc::Logger::set_global_engine(nullptr);
    RCalc::Allocator::cleanup();

    return res ? 0 : 255;
}


RCalc::Application& Main::get_app() {
    if (p_application) { return *p_application; }
    throw std::logic_error("Main application is not ready! You must **only** call get_app() during or after Renderer::init()!");
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
    std::cout << "  --print-licenses            Print the open source license information.\n";
    std::cout << "\n";

    std::cout << "Run Options:\n";
    std::cout << "  --renderer <renderer>       Select an available renderer.\n";
    std::cout << "  --logfile <path>            Store application logs to the file at <path>\n";
    std::cout << "        Will overwrite any existing file at <path>.\n";
    std::cout << "        ImGui renderer will log to both this file and stdout.\n";
    std::cout << "        Terminal renderer will only log to this file, and won't log otherwise.\n";
    std::cout << "\n";

    #ifndef NDEBUG
    std::cout << "Debug Options:\n";
    std::cout << "  --dump-info                 Dump the application info to JSON.\n";
    std::cout << "\n";
    #endif

    #ifdef TESTS_ENABLED
    std::cout << "Test Options:\n";
    std::cout << "  --run-tests                 Run the tests.\n";
    std::cout << "  --filter <filter>           Use <filter> to select which tests to run.\n";
    std::cout << "        Can provide multiple times for AND logic, or provide comma separated for OR logic.\n";
    std::cout << "        Multiple tags in the same filter are combined with AND logic.\n";
    std::cout << "        Maximum of 8 filters can be provided at once.\n";
    std::cout << "\n";
    std::cout << "  --list-tests                List all tests.\n";
    std::cout << "  --list-tags                 List all test tags.\n";
    std::cout << "  --list-tests-with-tag <tag> List all tests with the given tag.\n";
    std::cout << "  --monochrome                Disable colors in the test output.\n";
    std::cout << "\n";
    #endif
}


void Main::print_version()
{
    std::cout << get_full_version_string() << "\n";
}


void Main::print_licenses()
{
    std::cout << RCalc::Assets::license_md << "\n";
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
            if (++arg == args.end()) {
                std::cout << "Error: No renderer specified!\n\n";
                print_help();
                exit(255);
            }
            config.renderer_name = validate_renderer(arg);
            continue;
        }

        if (arg->compare("--logfile") == 0)
        {
            if (++arg == args.end()) {
                std::cout << "Error: No logfile path specified!\n\n";
                print_help();
                exit(255);
            }
            config.logfile_path = *arg;
            continue;
        }

        if (
            arg->compare("--print-licenses") == 0
        ) {
            print_licenses();
            exit(0);
        }

        #ifndef NDEBUG
        if (
            arg->compare("--dump-info") == 0
        ) {
            Main::p_application = RCalc::Allocator::create<RCalc::Application>();;
            RCalc::Dump::dump_info();
            RCalc::Allocator::destroy(Main::p_application);
            exit(0);
        }
        #endif

        #ifdef TESTS_ENABLED
        if (
            arg->compare("--run-tests") == 0
        ) {
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            if (config.test_config->command_given.has_value()) {
                std::cout << "Ignoring '--run-tests', already given command '" << config.test_config->command_given.value() << "'\n";
                exit(255);
            }
            config.test_config->command_given = "--run-tests";
            config.test_config->run_tests = true;
            continue;
        }

        if (arg->compare("--filter") == 0)
        {
            if (++arg == args.end()) {
                std::cout << "Error: No filter specified!\n\n";
                print_help();
                exit(255);
            }
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            if (config.test_config->filter_count >= 8) {
                std::cout << "Error: Too many filters specified! (Maximum: 8)\n";
                exit(255);
            }
            config.test_config->filters[config.test_config->filter_count++] = *arg;
            continue;
        }

        if (
            arg->compare("--list-tests") == 0
        ) {
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            if (config.test_config->command_given.has_value()) {
                std::cout << "Ignoring '--run-tests', already given command '" << config.test_config->command_given.value() << "'\n";
                exit(255);
            }
            config.test_config->command_given = "--list-tests";
            config.test_config->list_tests = true;
            config.test_config->run_tests = true;
            continue;
        }

        if (
            arg->compare("--list-tags") == 0
        ) {
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            if (config.test_config->command_given.has_value()) {
                std::cout << "Ignoring '--run-tests', already given command '" << config.test_config->command_given.value() << "'\n";
                exit(255);
            }
            config.test_config->command_given = "--list-tags";
            config.test_config->list_tags = true;
            config.test_config->run_tests = true;
            continue;
        }

        if (
            arg->compare("--list-tests-with-tag") == 0
        ) {
            if (++arg == args.end()) {
                std::cout << "Error: No tag specified!\n\n";
                print_help();
                exit(255);
            }
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            if (config.test_config->command_given.has_value()) {
                std::cout << "Ignoring '--run-tests', already given command '" << config.test_config->command_given.value() << "'\n";
                exit(255);
            }
            config.test_config->command_given = "--list-tests-with-tag";
            config.test_config->list_tests = true;
            config.test_config->list_test_tag = (arg)->data();
            config.test_config->run_tests = true;
            continue;
        }

        if (
            arg->compare("--monochrome") == 0
        ) {
            if (!config.test_config.has_value()) {
                config.test_config = RCalc::TestConfig();
            }
            config.test_config->monochrome = true;
            continue;
        }
        #endif
        
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