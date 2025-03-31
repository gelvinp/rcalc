import os
import sys
from platform_methods import detect_arch
import subprocess
import methods

# To match other platforms
STACK_SIZE = 8388608
HEAP_SIZE = 8388608

def get_name():
    return "Windows"


def get_mingw_bin_prefix(prefix, arch):
    if not prefix:
        mingw_bin_prefix = ""
    elif prefix[-1] != "/":
        mingw_bin_prefix = prefix + "/bin/"
    else:
        mingw_bin_prefix = prefix + "bin/"

    if arch == "x86_64":
        mingw_bin_prefix += "x86_64-w64-mingw32-"
    elif arch == "x86_32":
        mingw_bin_prefix += "i686-w64-mingw32-"
    elif arch == "arm32":
        mingw_bin_prefix += "armv7-w64-mingw32-"
    elif arch == "arm64":
        mingw_bin_prefix += "aarch64-w64-mingw32-"

    return mingw_bin_prefix


def detect_build_env_arch():
    msvc_target_aliases = {
        "amd64": "x86_64",
        "i386": "x86_32",
        "i486": "x86_32",
        "i586": "x86_32",
        "i686": "x86_32",
        "x86": "x86_32",
        "x64": "x86_64",
        "x86_64": "x86_64"
    }
    if os.getenv("VCINSTALLDIR") or os.getenv("VCTOOLSINSTALLDIR"):
        if os.getenv("Platform"):
            msvc_arch = os.getenv("Platform").lower()
            if msvc_arch in msvc_target_aliases.keys():
                return msvc_target_aliases[msvc_arch]

        if os.getenv("VSCMD_ARG_TGT_ARCH"):
            msvc_arch = os.getenv("VSCMD_ARG_TGT_ARCH").lower()
            if msvc_arch in msvc_target_aliases.keys():
                return msvc_target_aliases[msvc_arch]

        # Pre VS 2017 checks.
        if os.getenv("VCINSTALLDIR"):
            PATH = os.getenv("PATH").upper()
            VCINSTALLDIR = os.getenv("VCINSTALLDIR").upper()
            path_arch = {
                "BIN\\x86_ARM;": "arm32",
                "BIN\\amd64_ARM;": "arm32",
                "BIN\\x86_ARM64;": "arm64",
                "BIN\\amd64_ARM64;": "arm64",
                "BIN\\x86_amd64;": "a86_64",
                "BIN\\amd64;": "x86_64",
                "BIN\\amd64_x86;": "x86_32",
                "BIN;": "x86_32",
            }
            for path, arch in path_arch.items():
                final_path = VCINSTALLDIR + path
                if final_path in PATH:
                    return arch

        # VS 2017 and newer.
        if os.getenv("VCTOOLSINSTALLDIR"):
            host_path_index = os.getenv("PATH").upper().find(os.getenv("VCTOOLSINSTALLDIR").upper() + "BIN\\HOST")
            if host_path_index > -1:
                first_path_arch = os.getenv("PATH").split(";")[0].rsplit("\\", 1)[-1].lower()
                return msvc_target_aliases[first_path_arch]

    msys_target_aliases = {
        "mingw32": "x86_32",
        "mingw64": "x86_64",
        "ucrt64": "x86_64",
        "clang64": "x86_64",
        "clang32": "x86_32"
    }

    if os.getenv("MSYSTEM"):
        msys_arch = os.getenv("MSYSTEM").lower()
        if msys_arch in msys_target_aliases.keys():
            return msys_target_aliases[msys_arch]

    return ""


def try_cmd(test, prefix, arch):
    if arch:
        try:
            out = subprocess.Popen(
                get_mingw_bin_prefix(prefix, arch) + test,
                shell=True,
                stderr=subprocess.PIPE,
                stdout=subprocess.PIPE,
            )
            out.communicate()
            if out.returncode == 0:
                return True
        except Exception:
            pass
    else:
        for a in ["x86_64", "x86_32"]:
            try:
                out = subprocess.Popen(
                    get_mingw_bin_prefix(prefix, a) + test,
                    shell=True,
                    stderr=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                )
                out.communicate()
                if out.returncode == 0:
                    return True
            except Exception:
                pass

    return False


def is_available():
    if os.name == "nt" or os.name == "posix":
        if os.getenv("VCINSTALLDIR"):  # MSVC, manual setup
            return True

        prefix = os.getenv("MINGW_PREFIX", "")

        if try_cmd("gcc --version", prefix, "") or try_cmd("clang --version", prefix, ""):
            if (os.name == "nt") != bool(os.system(f"{prefix}/bin/pkg-config --version > /dev/null")):
                print("Error: pkg-config not found. Windows (mingw) platform is unavailable.")
                return False
        
        return True

    return False


def get_opts():
    from SCons.Variables import BoolVariable, EnumVariable

    mingw = os.getenv("MINGW_PREFIX", "")

    return [
        ("mingw_prefix", "MinGW prefix", mingw),
        BoolVariable("use_llvm", "Use the LLVM compiler", False),
        EnumVariable("windows_subsystem", "Windows subsystem", "gui", ("gui", "console")),
        BoolVariable("use_mingw", "Use the Mingw compiler, even if MSVC is installed.", False),
        BoolVariable("use_asan", "Compile with the address sanitizer", False),
        (
            "target_win_version",
            "Targeted Windows version, >= 0x0601 (Windows 7)",
            "0x0601",
        )
    ]


def get_flags():
    return [
        ("arch", detect_arch()),
    ]


def build_res_file(target, source, env):
    arch_aliases = {
        "x86_32": "pe-i386",
        "x86_64": "pe-x86-64"
    }

    cmdbase = "windres --include-dir . --target=" + arch_aliases[env["arch"]]

    mingw_bin_prefix = get_mingw_bin_prefix(env["mingw_prefix"], env["arch"])

    for x in range(len(source)):
        ok = True
        # Try prefixed executable (MinGW on Linux).
        cmd = mingw_bin_prefix + cmdbase + " -i " + str(source[x]) + " -o " + str(target[x])
        try:
            out = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE).communicate()
            if len(out[1]):
                ok = False
        except Exception:
            ok = False

        # Try generic executable (MSYS2).
        if not ok:
            cmd = cmdbase + " -i " + str(source[x]) + " -o " + str(target[x])
            try:
                out = subprocess.Popen(cmd, shell=True, stderr=subprocess.PIPE).communicate()
                if len(out[1]):
                    return -1
            except Exception:
                return -1

    return 0


def setup_msvc_manual(env):
    """Running from VCVARS environment"""

    env_arch = detect_build_env_arch()
    if env["arch"] != env_arch:
        print(
            """
            Arch argument (%s) is not matching Native/Cross Compile Tools Prompt/Developer Console (or Visual Studio settings) that is being used to run SCons (%s).
            Run SCons again without arch argument (example: scons p=windows) and SCons will attempt to detect what MSVC compiler will be executed and inform you.
            """
            % (env["arch"], env_arch)
        )
        sys.exit(200)

    print("Found MSVC, arch %s" % (env_arch))


def setup_msvc_auto(env):
    """Set up MSVC using SCons's auto-detection logic"""

    # If MSVC_VERSION is set by SCons, we know MSVC is installed.
    # But we may want a different version or target arch.

    # Valid architectures for MSVC's TARGET_ARCH:
    # ['amd64', 'emt64', 'i386', 'i486', 'i586', 'i686', 'ia64', 'itanium', 'x86', 'x86_64']
    # Our x86_64 and arm64 are the same, and we need to map the 32-bit
    # architectures to other names since MSVC isn't as explicit.
    # The rest we don't need to worry about because they are
    # aliases or aren't supported.
    msvc_arch_aliases = {"x86_32": "x86"}
    if env["arch"] in msvc_arch_aliases.keys():
        env["TARGET_ARCH"] = msvc_arch_aliases[env["arch"]]
    else:
        env["TARGET_ARCH"] = env["arch"]

    # The env may have already been set up with default MSVC tools, so
    # reset a few things so we can set it up with the tools we want.
    # (Ideally we'd decide on the tool config before configuring any
    # environment, and just set the env up once, but this function runs
    # on an existing env so this is the simplest way.)
    env["MSVC_SETUP_RUN"] = False  # Need to set this to re-run the tool
    env["MSVS_VERSION"] = None
    env["MSVC_VERSION"] = None

    if "msvc_version" in env:
        env["MSVC_VERSION"] = env["msvc_version"]
    env.Tool("msvc")
    env.Tool("mssdk")  # we want the MS SDK

    # Note: actual compiler version can be found in env['MSVC_VERSION'], e.g. "14.1" for VS2015
    print("Found MSVC version %s, arch %s" % (env["MSVC_VERSION"], env["arch"]))


def setup_mingw(env):
    """Set up env for use with mingw"""

    env_arch = detect_build_env_arch()
    if os.getenv("MSYSTEM") == "MSYS":
        print(
            """
            Running from base MSYS2 console/environment, use target specific environment instead (e.g., mingw32, mingw64, clang32, clang64).
            """
        )
        sys.exit(201)

    if env_arch != "" and env["arch"] != env_arch:
        print(
            """
            Arch argument (%s) is not matching MSYS2 console/environment that is being used to run SCons (%s).
            Run SCons again without arch argument (example: scons p=windows) and SCons will attempt to detect what MSYS2 compiler will be executed and inform you.
            """
            % (env["arch"], env_arch)
        )
        sys.exit(202)

    if not try_cmd("gcc --version", env["mingw_prefix"], env["arch"]) and not try_cmd(
        "clang --version", env["mingw_prefix"], env["arch"]
    ):
        print(
            """
            No valid compilers found, use MINGW_PREFIX environment variable to set MinGW path.
            """
        )
        sys.exit(202)

    print("Using MinGW, arch %s" % (env["arch"]))


def configure_msvc(env, vcvars_msvc_config):
    """Configure env to work with MSVC"""

    ## Build type
    env.Append(LINKFLAGS=["/ENTRY:WinMain"])

    if env["windows_subsystem"] == "gui":
        env.Append(LINKFLAGS=["/SUBSYSTEM:WINDOWS"])
    else:
        env.Append(LINKFLAGS=["/SUBSYSTEM:CONSOLE"])
        env.extra_suffix += ".terminal"

    ## Compile/link flags

    if env["target"] == "debug":
        env.AppendUnique(CCFLAGS=["/MTd"])
    else:
        env.AppendUnique(CCFLAGS=["/MT"])

    env.AppendUnique(CCFLAGS=["/Gd", "/GR", "/nologo"])
    env.AppendUnique(CCFLAGS=["/utf-8"])  # Force to use Unicode encoding.
    env.AppendUnique(CXXFLAGS=["/TP"])  # assume all sources are C++
    # Once it was thought that only debug builds would be too large,
    # but this has recently stopped being true. See the mingw function
    # for notes on why this shouldn't be enabled for gcc
    env.AppendUnique(CCFLAGS=["/bigobj"])

    if vcvars_msvc_config:  # should be automatic if SCons found it
        if os.getenv("WindowsSdkDir") is not None:
            env.Prepend(CPPPATH=[os.getenv("WindowsSdkDir") + "/Include"])
        else:
            print("Missing environment variable: WindowsSdkDir")

    env.AppendUnique(
        CPPDEFINES=[
            "ENABLE_PLATFORM_WINDOWS",
            "strtok_p=strtok_s",
            "WIN32",
            "MSVC",
            "WINVER=%s" % env["target_win_version"],
            "_WIN32_WINNT=%s" % env["target_win_version"],
        ]
    )
    env.AppendUnique(CPPDEFINES=["NOMINMAX"])  # disable bogus min/max WinDef.h macros
    if env["arch"] == "x86_64":
        env.AppendUnique(CPPDEFINES=["_WIN64"])

    ## Libs

    LIBS = ["Shell32", "Advapi32"]

    if env["target"] == "debug":
        LIBS.append("libucrtd")
    else:
        LIBS.append("libucrt")

    env.Append(LINKFLAGS=[p + env["LIBSUFFIX"] for p in LIBS])

    if vcvars_msvc_config:
        if os.getenv("WindowsSdkDir") is not None:
            env.Append(LIBPATH=[os.getenv("WindowsSdkDir") + "/Lib"])
        else:
            print("Missing environment variable: WindowsSdkDir")

    if vcvars_msvc_config:
        env.Prepend(CPPPATH=[p for p in os.getenv("INCLUDE").split(";")])
        env.Append(LIBPATH=[p for p in os.getenv("LIB").split(";")])

    # Sanitizers
    if env["use_asan"]:
        env.extra_suffix += ".san"
        env.Append(LINKFLAGS=["/INFERASANLIBS"])
        env.Append(CCFLAGS=["/fsanitize=address"])

    # Incremental linking fix
    env["BUILDERS"]["ProgramOriginal"] = env["BUILDERS"]["Program"]
    env["BUILDERS"]["Program"] = methods.precious_program

    env.AppendUnique(LINKFLAGS=["/STACK:" + str(STACK_SIZE)])
    env.AppendUnique(LINKFLAGS=["/HEAP:" + str(STACK_SIZE)])
    env["lto"] = "none"


def configure_mingw(env):
    # Workaround for MinGW. See:
    # https://www.scons.org/wiki/LongCmdLinesOnWin32
    env.use_windows_spawn_fix()
    
    env.Tool("mingw")
    env.extra_suffix += ".mingw"

    ## Build type

    gcc_found = True

    if not env["use_llvm"] and not try_cmd("gcc --version", env["mingw_prefix"], env["arch"]):
        print("gcc not found, checking for clang...")
        gcc_found = False
        env["use_llvm"] = True

    if env["use_llvm"] and not try_cmd("clang --version", env["mingw_prefix"], env["arch"]):
        if gcc_found:
            print("clang not found, falling back to gcc...")
        else:
            print("Neither gcc or clang were found, aborting...")
            sys.exit(255)
        
        env["use_llvm"] = False

    if env["windows_subsystem"] == "gui":
        env.Append(LINKFLAGS=["-Wl,--subsystem,windows"])
    else:
        env.Append(LINKFLAGS=["-Wl,--subsystem,console"])
        env.extra_suffix += ".terminal"

    ## Compiler configuration

    mingw_bin_prefix = get_mingw_bin_prefix(env["mingw_prefix"], env["arch"])

    if env["use_llvm"]:
        env["CC"] = mingw_bin_prefix + "clang"
        env["CXX"] = mingw_bin_prefix + "clang++"
        if try_cmd("as --version", env["mingw_prefix"], env["arch"]):
            env["AS"] = mingw_bin_prefix + "as"
        if try_cmd("ar --version", env["mingw_prefix"], env["arch"]):
            env["AR"] = mingw_bin_prefix + "ar"
        if try_cmd("ranlib --version", env["mingw_prefix"], env["arch"]):
            env["RANLIB"] = mingw_bin_prefix + "ranlib"
        env.extra_suffix = ".llvm" + env.extra_suffix
    else:
        env["CC"] = mingw_bin_prefix + "gcc"
        env["CXX"] = mingw_bin_prefix + "g++"
        if try_cmd("as --version", env["mingw_prefix"], env["arch"]):
            env["AS"] = mingw_bin_prefix + "as"
        if try_cmd("gcc-ar --version", env["mingw_prefix"], env["arch"]):
            env["AR"] = mingw_bin_prefix + "gcc-ar"
        if try_cmd("gcc-ranlib --version", env["mingw_prefix"], env["arch"]):
            env["RANLIB"] = mingw_bin_prefix + "gcc-ranlib"

    ## LTO
    if env["target"] == "release":
        if env["use_llvm"]:
            if env["use_lto"] == "no":
                env["lto"] = "none"
            else:
                env["lto"] = "thin"
                env.Append(CCFLAGS=["-flto=thin"])
                env.Append(LINKFLAGS=["-flto=thin"])
        else:
            if env["use_lto"] == "no":
                env["lto"] = "none"
            else:
                env.Append(CCFLAGS=["-flto"])

                if env.GetOption("num_jobs") > 1:
                    env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
                else:
                    env.Append(LINKFLAGS=["-flto"])
                env["lto"] = "full"
    else:
        if env["use_llvm"]:
            if env["use_lto"] != "yes":
                env["lto"] = "none"
            else:
                env["lto"] = "thin"
                env.Append(CCFLAGS=["-flto=thin"])
                env.Append(LINKFLAGS=["-flto=thin"])
        else:
            if env["use_lto"] != "yes":
                env["lto"] = "none"
            else:
                env.Append(CCFLAGS=["-flto"])

                if env.GetOption("num_jobs") > 1:
                    env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
                else:
                    env.Append(LINKFLAGS=["-flto"])
                env["lto"] = "full"
    
    env.Append(CCFLAGS=["-pipe"])

    env.Append(CPPDEFINES=["ENABLE_PLATFORM_WINDOWS", "USING_MINGW"])
    env.Append(CPPDEFINES=["strtok_p=strtok_r"])

    env.Append(LINKFLAGS=["-Wl,--stack," + str(STACK_SIZE)])

    ## Compile flags

    if not env["use_llvm"]:
        env.Append(CCFLAGS=["-mwindows"])

    env.Append(LIBS=["mingw32"])
    env.Append(LINKFLAGS=["-static", "-static-libgcc", "-static-libstdc++"])
    env.Append(BUILDERS={"RES": env.Builder(action=build_res_file, suffix=".o", src_suffix=".rc")})


def configure(env: "Environment"):
    # Validate arch.
    supported_arches = ["x86_32", "x86_64"]
    if env["arch"] not in supported_arches:
        print(
            'Unsupported CPU architecture "%s" for Windows. Supported architectures are: %s.'
            % (env["arch"], ", ".join(supported_arches))
        )
        sys.exit()

    if os.name == "nt":
        env["ENV"] = os.environ  # this makes build less repeatable, but simplifies some things
        env["ENV"]["TMP"] = os.environ["TMP"]

    # First figure out which compiler, version, and target arch we're using
    if os.getenv("VCINSTALLDIR") and detect_build_env_arch() and not env["use_mingw"]:
        setup_msvc_manual(env)
        env.msvc = True
        vcvars_msvc_config = True
    elif env.get("MSVC_VERSION", "") and not env["use_mingw"]:
        setup_msvc_auto(env)
        env.msvc = True
        vcvars_msvc_config = False
    else:
        setup_mingw(env)
        env.msvc = False

    # Now set compiler/linker flags
    if env.msvc:
        configure_msvc(env, vcvars_msvc_config)

    else:  # MinGW
        configure_mingw(env)
