extern int main(int argc, char** pp_argv);

#include <Windows.h>
#include "core/coredef.h"

EXTERN_C BOOL WINAPI _CRT_INIT(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

// For some reason, I cannot use the UCRT entry point. 
// All I get is `unresolved external symbol mainCRTStartup`.
// After many hours, I am giving up, and doing this instead.
// This is not a place of honor, no highly-esteemed deeds
// are commemorated here. Nothing valued is here.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);
    UNUSED(nCmdShow);
    _CRT_INIT(hInstance, DLL_PROCESS_ATTACH, NULL);

    int argc;

    LPWSTR* wc_argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (wc_argv == nullptr) {
        return 255;
    }

    auto pp_argv = new char*[argc];
    for (int index = 0; index < argc; ++index) {
        int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wc_argv[index], -1, nullptr, 0, nullptr, nullptr);
        char* utf8_buf = new char[utf8_len + 1];
        WideCharToMultiByte(CP_UTF8, 0, wc_argv[index], -1, utf8_buf, utf8_len, nullptr, nullptr);
        utf8_buf[utf8_len] = '\0';
        pp_argv[index] = utf8_buf;
    }

    int result = main(argc, pp_argv);

    for (int index = 0; index < argc; ++index) {
        delete[] pp_argv[index];
    }
    delete[] pp_argv;

    LocalFree(wc_argv);

    _CRT_INIT(hInstance, DLL_PROCESS_DETACH, NULL);

    ExitProcess(result);
}