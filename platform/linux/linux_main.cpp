#include "main/main.h"
#include "platform_linux.h"
#include "modules/register_modules.h"

#include <sstream>

int main(int argc, char** pp_argv)
{
    Main m;
    Platform& platform = Platform::get_singleton();

    m.parse_args(argc, pp_argv);
    initialize_modules();
    Result<> res = platform.init();
    
    if (!res) {
        std::stringstream ss;
        ss << res.unwrap_err();
        Logger::log_err("%s", ss.str().c_str());
    } else {
        platform.runloop();
    }

    platform.cleanup();
    cleanup_modules();

    return res ? 0 : 255;
}