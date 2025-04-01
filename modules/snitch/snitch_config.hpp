#ifndef SNITCH_CONFIG_HPP
#define SNITCH_CONFIG_HPP

#include <version> // for C++ feature check macros

#include <cstddef> // Needed for MacOS (for some reason)

// These are defined from build-time configuration.
// clang-format off
#define SNITCH_VERSION "1.3.2"
#define SNITCH_FULL_VERSION "1.3.2.cec4a04"
#define SNITCH_VERSION_MAJOR 1
#define SNITCH_VERSION_MINOR 3
#define SNITCH_VERSION_PATCH 2


#define SNITCH_MAX_TEST_CASES 5000

#define SNITCH_MAX_NESTED_SECTIONS 8

#define SNITCH_MAX_EXPR_LENGTH 1024

#define SNITCH_MAX_MESSAGE_LENGTH 1024

#define SNITCH_MAX_TEST_NAME_LENGTH 1024

#define SNITCH_MAX_TAG_LENGTH 256

#define SNITCH_MAX_CAPTURES 8

#define SNITCH_MAX_CAPTURE_LENGTH 256

#define SNITCH_MAX_UNIQUE_TAGS 1024

#define SNITCH_MAX_COMMAND_LINE_ARGS 1024

#define SNITCH_MAX_REGISTERED_REPORTERS 8

#define SNITCH_MAX_PATH_LENGTH 1024

#define SNITCH_MAX_REPORTER_SIZE_BYTES 128

#define SNITCH_DEFINE_MAIN 0

#define SNITCH_WITH_EXCEPTIONS 1

#define SNITCH_WITH_TIMINGS 1

#define SNITCH_WITH_SHORTHAND_MACROS 1

#define SNITCH_DEFAULT_WITH_COLOR 1

#define SNITCH_CONSTEXPR_FLOAT_USE_BITCAST 1

#define SNITCH_APPEND_TO_CHARS 1

#define SNITCH_DECOMPOSE_SUCCESSFUL_ASSERTIONS 0

#define SNITCH_WITH_ALL_REPORTERS 0

#define SNITCH_WITH_TEAMCITY_REPORTER 0

#define SNITCH_WITH_CATCH2_REPORTER 0

#define SNITCH_WITH_MULTITHREADING 1

#define SNITCH_SHARED_LIBRARY 0

#define SNITCH_ENABLE 1

// clang-format on

#if defined(_MSC_VER)
#    if defined(_KERNEL_MODE) || (defined(_HAS_EXCEPTIONS) && !_HAS_EXCEPTIONS)
#        define SNITCH_EXCEPTIONS_NOT_AVAILABLE
#    endif
#elif defined(__clang__) || defined(__GNUC__)
#    if !defined(__EXCEPTIONS)
#        define SNITCH_EXCEPTIONS_NOT_AVAILABLE
#    endif
#endif

#if defined(SNITCH_EXCEPTIONS_NOT_AVAILABLE)
#    undef SNITCH_WITH_EXCEPTIONS
#    define SNITCH_WITH_EXCEPTIONS 0
#endif

#if defined(SNITCH_WITH_MULTITHREADING)
#    define SNITCH_THREAD_LOCAL thread_local
#else
#    define SNITCH_THREAD_LOCAL
#endif

#if !defined(__cpp_lib_bit_cast)
#    undef SNITCH_CONSTEXPR_FLOAT_USE_BITCAST
#    define SNITCH_CONSTEXPR_FLOAT_USE_BITCAST 0
#endif

#if (!defined(__cpp_lib_to_chars)) || (defined(_GLIBCXX_RELEASE) && _GLIBCXX_RELEASE <= 11) ||     \
(defined(_LIBCPP_VERSION) && _LIBCPP_VERSION <= 14000) ||                                      \
(defined(_MSC_VER) && _MSC_VER <= 1924)
#    undef SNITCH_APPEND_TO_CHARS
#    define SNITCH_APPEND_TO_CHARS 0
#endif

#if SNITCH_SHARED_LIBRARY
#    if defined(_MSC_VER)
#        if defined(SNITCH_EXPORTS)
#            define SNITCH_EXPORT __declspec(dllexport)
#        else
#            define SNITCH_EXPORT __declspec(dllimport)
#        endif
#    elif defined(__clang__) || defined(__GNUC__)
#        define SNITCH_EXPORT [[gnu::visibility("default")]]
#    else
#        define SNITCH_EXPORT
#    endif
#else
#    define SNITCH_EXPORT
#endif

#endif
