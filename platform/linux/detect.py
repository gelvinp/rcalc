import os
import sys
from platform_methods import detect_arch

def get_name():
    return "Linux"


def is_available():
    if os.name != "posix" or sys.platform == "darwin":
        return False
    
    if os.system("pkg-config --version > /dev/null"):
        print("Error: pkg-config not found. Linux platform is unavailable.")
        return False

    return sys.platform == "linux"


def get_opts():
    from SCons.Variables import BoolVariable, EnumVariable

    return [
        BoolVariable("use_llvm", "Use the LLVM compiler", False),
        BoolVariable("use_asan", "Compile with -fsanitize=address", False),
        BoolVariable("use_coverage", "Compile with the needed flags to measure test coverage", False),
        EnumVariable(
            "windowing",
            help="windowing system to use",
            default="x11",
            allowed_values=("x11"),
            ignorecase=2
        )
    ]


def get_flags():
    return [
        ("arch", detect_arch()),
    ]


def configure(env: "Environment"):
    # Validate architecture
    supported_arches = ["x86_32", "x86_64"]
    if env["arch"] not in supported_arches:
        print(
            "Unsupported CPU architecture %s for platform Linux. Supported architectures are: [%s]"
            % (env["arch"], ", ".join(supported_arches))
        )

        sys.exit(255)
    
    if "CXX" in env and "clang" in os.path.basename(env["CXX"]):
        env["use_llvm"] = True
    
    if env["use_llvm"]:
        if "clang++" not in os.path.basename(env["CXX"]):
            env["CC"] = "clang"
            env["CXX"] = "clang++"
        env.extra_suffix = ".llvm" + env.extra_suffix

    
    # LTO
    if env["target"] == "release":
        env.Append(LINKFLAGS=["-static-libgcc", "-static-libstdc++"])

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
    
    if not env["use_llvm"]:
        env["RANLIB"] = "gcc-ranlib"
        env["AR"] = "gcc-ar"
    
    env.Append(CCFLAGS=["-pipe"])

    if env["use_coverage"]:
        env.Append(CCFLAGS=["--coverage"])
        env.Append(LINKFLAGS=["--coverage"])

    env.Append(CPPDEFINES=["ENABLE_PLATFORM_LINUX"])
    env.Append(CPPDEFINES=["strtok_p=strtok_r"])

    if env["use_asan"]:
        env.extra_suffix = ".asan" + env.extra_suffix
        env.Append(CCFLAGS=["-fsanitize=address"])
        env.Append(LINKFLAGS=["-fsanitize=address"])

    # Cross compilation
    host_is_64bit = sys.maxsize > 2**32
    if host_is_64bit and env["arch"] == "x86_32":
        env.Append(CCFLAGS=["-m32"])
        env.Append(LINKFLAGS=["-m32", "-L/usr/lib/i386-linux-gnu"])
    elif not host_is_64bit and env["arch"] == "x86_64":
        env.Append(CCFLAGS=["-m64"])
        env.Append(LINKFLAGS=["-m64", "-L/usr/lib/i686-linux-gnu"])

    # RELRO
    if env["target"] == "release":
        env.Append(LINKFLAGS=["-Wl,-z,now"])
