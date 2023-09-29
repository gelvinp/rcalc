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
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("use_llvm", "Use the LLVM compiler", False),
        BoolVariable("use_asan", "Compile with -fsanitize=address", False)
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
        if env["use_llvm"]:
            env.Append(CCFLAGS=["-flto=thin"])
            env.Append(LINKFLAGS=["-flto=thin"])
            env["lto"] = "thin"
        else:
            env.Append(CCFLAGS=["-flto"])

            if env.GetOption("num_jobs") > 1:
                env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
            else:
                env.Append(LINKFLAGS=["-flto"])
            
            env["RANLIB"] = "gcc-ranlib"
            env["AR"] = "gcc-ar"
            env["lto"] = "full"
    else:
        env["lto"] = "none"
    
    env.Append(CCFLAGS=["-pipe"])

    env.Append(CPPDEFINES=["ENABLE_PLATFORM_LINUX"])
    env.Append(CPPDEFINES=["UNREACHABLE=__builtin_unreachable"])

    if env["use_asan"]:
        env.extra_suffix = ".asan" + env.extra_suffix
        env.Append(CCFLAGS=["-fsanitize=address"])
        env.Append(LINKFLAGS=["-fsanitize=address"])

    deps = ['glfw3', 'gl', 'freetype2']
    if os.system(f"pkg-config --exists {' '.join(deps)}"):
        print("Error: Required libraries not found. Aborting.")
        sys.exit(255)
    env.ParseConfig(f"pkg-config {' '.join(deps)} --cflags --libs")

    # Cross compilation
    host_is_64bit = sys.maxsize > 2**32
    if host_is_64bit and env["arch"] == "x86_32":
        env.Append(CCFLAGS=["-m32"])
        env.Append(LINKFLAGS=["-m32", "-L/usr/lib/i386-linux-gnu"])
    elif not host_is_64bit and env["arch"] == "x86_64":
        env.Append(CCFLAGS=["-m64"])
        env.Append(LINKFLAGS=["-m64", "-L/usr/lib/i686-linux-gnu"])
    

    env.Append(IMGUI_BACKEND="glfw_opengl3")
