import os
import sys
from platform_methods import detect_arch
import shutil
import zipfile
import version


def get_name():
    return "MacOS"


def is_available():
    if sys.platform != "darwin":
        return False
    
    if os.system("pkg-config --version > /dev/null"):
        print("Error: pkg-config not found. MacOS platform is unavailable.")
        return False

    return True


def get_opts():
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("use_gcc", "Use the GCC compiler", False),
        BoolVariable("use_asan", "Compile with -fsanitize=address", False)
    ]


def get_flags():
    return [
        ("arch", detect_arch()),
    ]


def configure(env: "Environment"):
    # Validate architecture
    supported_arches = ["x86_64", "arm64"]
    if env["arch"] not in supported_arches:
        print(
            "Unsupported CPU architecture %s for platform Linux. Supported architectures are: [%s]"
            % (env["arch"], ", ".join(supported_arches))
        )

        sys.exit(255)
    
    if "CXX" in env and ("gcc-" in os.path.basename(env["CC"]) or "g++-" in os.path.basename(env["CXX"])):
        env["use_gcc"] = True
    
    if env["use_gcc"]:
        if (not "gcc-" in os.path.basename(env["CC"])) or (not "g++-" in os.path.basename(env["CXX"])) or (not "gcc-ranlib-" in os.path.basename(env["RANLIB"])) or (not "gcc-ar-" in os.path.basename(env["AR"])):
            print("To compile with GCC on MacOS, please set the 'CC', 'CXX', 'RANLIB', and 'AR' environment variables manually!\n\t(i.e. CC=gcc-13 CXX=g++-13, RANLIB=gcc-ranlib-13 AR=gcc-ar-13)")
            sys.exit(255)
        env.extra_suffix = ".gcc" + env.extra_suffix
    

    if env["arch"] == "arm64":
        env.Append(ASFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
        env.Append(CCFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
        env.Append(LINKFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
    elif env["arch"] == "x86_64":
        env.Append(ASFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])
        env.Append(CCFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])
        env.Append(LINKFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])

    
    # LTO
    if env["target"] == "release":
        if env["use_gcc"]:
            env.Append(CCFLAGS=["-flto"])

            if env.GetOption("num_jobs") > 1:
                env.Append(LINKFLAGS=["-flto=" + str(env.GetOption("num_jobs"))])
            else:
                env.Append(LINKFLAGS=["-flto"])

            env["lto"] = "full"
        else:
            env.Append(CCFLAGS=["-flto=thin"])
            env.Append(LINKFLAGS=["-flto=thin"])
            env["lto"] = "thin"
    else:
        env["lto"] = "none"
    
    # RPath
    if not env["use_gcc"]:
        env.Append(LINKFLAGS=["-rpath", "@executable_path/"])
    
    env.Append(CCFLAGS=["-pipe"])

    env.Append(CPPDEFINES=["ENABLE_PLATFORM_MACOS"])
    env.Append(CPPDEFINES=["UNREACHABLE=__builtin_unreachable"])
    env.Append(CPPDEFINES=["strtok_p=strtok_r"])

    if env["use_asan"]:
        env.extra_suffix = ".asan" + env.extra_suffix
        env.Append(CCFLAGS=["-fsanitize=address"])
        env.Append(LINKFLAGS=["-fsanitize=address"])


def post_build(target, source, env):
    bundle_name = "bin/RCalc" + env["PROGSUFFIX"]
    
    bundle_name += ".app"
    
    if os.path.isdir(bundle_name):
        shutil.rmtree(bundle_name)
    
    os.mkdir(bundle_name)

    src = str(source[0])
    dst = bundle_name + "/Contents/MacOS/rcalc"

    with zipfile.ZipFile("misc/macos_bundle.zip", "r") as zip:
        zip.extractall(bundle_name + "/")
    
    shutil.move(src, dst)

    # Make executable
    mode = os.stat(dst).st_mode
    mode |= (mode & 0o444) >> 2 # Convert read bits to execute bits
    os.chmod(dst, mode)

    # Fix the Info.plist file
    plist = []
    plist_path = bundle_name + "/Contents/Info.plist"
    with open(plist_path, 'r') as file:
        plist.extend(file)
    
    replace = {
        "$VERSION": f'{version.major}.{version.minor}.{version.patch}'
    }
    
    with open(plist_path, 'w') as file:
        for line in plist:
            for [replace_target, replace_with] in replace.items():
                line = line.replace(replace_target, replace_with)
            file.write(line)


def platform_clean(env):
    bundle_name = "bin/RCalc"

    if env["target"] == "debug":
        bundle_name += "_debug"
    
    bundle_name += ".app"
    
    if os.path.isdir(bundle_name):
        shutil.rmtree(bundle_name)