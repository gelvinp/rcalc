import os
import sys
from platform_methods import detect_arch
import shutil
import zipfile
import version


def get_name():
    return "MacOS"


def is_available():
    if (sys.platform != "darwin") and ("OSXCROSS_ROOT" not in os.environ):
        return False
    
    if os.system("pkg-config --version > /dev/null"):
        print("Error: pkg-config not found. MacOS platform is unavailable.")
        return False

    return True


def get_opts():
    from SCons.Variables import BoolVariable

    return [
        BoolVariable("use_asan", "Compile with -fsanitize=address", False),
        BoolVariable("use_coverage", "Compile with the needed flags to measure test coverage", False),
        ("osxcross_sdk", "OSXCross SDK version", "darwin24.4"),
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
            "Unsupported CPU architecture %s for platform MacOS. Supported architectures are: [%s]"
            % (env["arch"], ", ".join(supported_arches))
        )

        sys.exit(255)
    
    if "OSXCROSS_ROOT" in os.environ:
        root = os.environ.get("OSXCROSS_ROOT", "")

        if env["arch"] == "arm64":
            prefix = root + "/target/bin/arm64-apple-" + env["osxcross_sdk"] + "-"
        else:
            prefix = root + "/target/bin/x86_64-apple-" + env["osxcross_sdk"] + "-"
        
        env["CC"] = prefix + "cc"
        env["CXX"] = prefix + "c++"
        env["AR"] = prefix + "ar"
        env["RANLIB"] = prefix + "ranlib"
        env["AS"] = prefix + "as"
    else:
        env["CC"] = "clang"
        env["CXX"] = "clang++"
    

    if env["arch"] == "arm64":
        env.Append(ASFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
        env.Append(CCFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
        env.Append(LINKFLAGS=["-arch", "arm64", "-mmacosx-version-min=11.0"])
    elif env["arch"] == "x86_64":
        env.Append(ASFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])
        env.Append(CCFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])
        env.Append(LINKFLAGS=["-arch", "x86_64", "-mmacosx-version-min=10.13"])

    
    # LTO
    if "OSXCROSS_ROOT" in os.environ:
        # LTO is not working with osxcross :/
        # https://github.com/tpoechtrager/osxcross/issues/366
        env["lto"] = "none"
    else:
        if (env["target"] == "release"):
            if env["use_lto"] == "no":
                env["lto"] = "none"
            else:
                env["lto"] = "thin"
                env.Append(CCFLAGS=["-flto=thin"])
                env.Append(LINKFLAGS=["-flto=thin"])
        else:
            if env["use_lto"] != "yes":
                env["lto"] = "none"
            else:
                env["lto"] = "thin"
                env.Append(CCFLAGS=["-flto=thin"])
                env.Append(LINKFLAGS=["-flto=thin"])
    
    env.Append(LINKFLAGS=["-rpath", "@executable_path/"])
    
    env.Append(CCFLAGS=["-pipe"])

    if env["use_coverage"]:
        env.Append(CCFLAGS=["--coverage"])
        env.Append(LINKFLAGS=["--coverage"])

    env.Append(CPPDEFINES=["ENABLE_PLATFORM_MACOS"])
    env.Append(CPPDEFINES=["strtok_p=strtok_r"])

    if env["use_asan"]:
        env.extra_suffix = ".asan" + env.extra_suffix
        env.Append(CCFLAGS=["-fsanitize=address"])
        env.Append(LINKFLAGS=["-fsanitize=address"])


def post_build(target, source, env):
    bundle_name = "bin/RCalc" + env["PROGSUFFIX"] + ".app"
    
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

    if env["arch"] == "arm64":
        replace["$MINIMUM_SYSTEM_VERSION"] = "11.0"
    elif env["arch"] == "x86_64":
        replace["$MINIMUM_SYSTEM_VERSION"] = "10.13"
    
    with open(plist_path, 'w') as file:
        for line in plist:
            for [replace_target, replace_with] in replace.items():
                line = line.replace(replace_target, replace_with)
            file.write(line)


def platform_clean(env):
    bundle_name = "bin/RCalc" + env["PROGSUFFIX"] + ".app"
    
    if os.path.isdir(bundle_name):
        shutil.rmtree(bundle_name)