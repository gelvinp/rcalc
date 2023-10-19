EnsureSConsVersion(4, 4, 0)
EnsurePythonVersion(3, 10)

import atexit
import glob
import os
import sys
import time
from types import ModuleType
from collections import OrderedDict
from importlib.util import spec_from_file_location, module_from_spec

def _helper_module(name, path):
    spec = spec_from_file_location(name, path)
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    sys.modules[name] = module
    # Ensure the module's parents are in loaded to avoid loading the wrong parent
    # when doing "import foo.bar" while only "foo.bar" as declared as helper module
    child_module = module
    parent_name = name
    while True:
        try:
            parent_name, child_name = parent_name.rsplit(".", 1)
        except ValueError:
            break
        try:
            parent_module = sys.modules[parent_name]
        except KeyError:
            parent_module = ModuleType(parent_name)
            sys.modules[parent_name] = parent_module
        setattr(parent_module, child_name, child_module)

_helper_module("methods", "methods.py")
_helper_module("platform_methods", "platform_methods.py")
_helper_module("version", "version.py")

import methods
import platform_methods

# Get available platforms
available_platforms = []
platform_opts = {}
platform_flags = {}

available_renderers = []
renderer_priorities = {}

start_time = time.time()

for platform_path in sorted(glob.glob("platform/*")):
    if not os.path.isdir(platform_path) or not os.path.exists(platform_path + "/detect.py"):
        continue
    
    tmp_path = "./" + platform_path
    sys.path.insert(0, tmp_path)
    import detect

    if detect.is_available():
        platform_name = platform_path.replace("platform/", "")
        platform_name = platform_name.replace("platform\\", "")

        available_platforms += [platform_name]
        platform_opts[platform_name] = detect.get_opts()
        platform_flags[platform_name] = detect.get_flags()
    
    sys.path.remove(tmp_path)
    sys.modules.pop("detect")


for renderer_path in sorted(glob.glob("app/renderers/*")):
    if not os.path.isdir(renderer_path) or not os.path.exists(renderer_path + "/renderer.py"):
        continue
    
    tmp_path = "./" + renderer_path
    sys.path.insert(0, tmp_path)
    import renderer

    if renderer.is_available():
        renderer_name = renderer_path.replace("app/renderers/", "")
        renderer_name = renderer_name.replace("app\\renderers\\", "")
        available_renderers.append(renderer_name)
        renderer_priorities[renderer_name] = renderer.get_priority()
    
    sys.path.remove(tmp_path)
    sys.modules.pop("renderer")


env_base = Environment()
if "TERM" in os.environ:
    env_base["ENV"]["TERM"] = os.environ["TERM"]

env_base["ENV"]["PATH"] = os.environ["PATH"]

env_base["enabled_command_scopes"] = [
    "Application"
]
env_base["enabled_modules"] = [
    "bigint",
    "glm"
]


# Set up env functions
env_base.__class__.add_source_files = methods.add_source_files
env_base.__class__.add_library = methods.add_library
env_base.__class__.add_program = methods.add_program
env_base.__class__.CommandNoCache = methods.CommandNoCache
env_base.__class__.use_windows_spawn_fix = methods.use_windows_spawn_fix
env_base.__class__.Run = methods.Run


# Build options
opt_files = ["build_opts.py"]

profile = ARGUMENTS.get("profile", "")
if profile:
    if os.path.isfile(profile):
        opt_files.append(profile)
    elif os.path.isfile(profile + ".py"):
        opt_files.append(profile + ".py")

opts = Variables(opt_files, ARGUMENTS)

opts.Add("platform", "Target platform (%s)" % ("|".join(available_platforms)), "")
opts.Add(EnumVariable("target", "Compilation target", "debug", ("debug", "release")))
opts.Add(EnumVariable("arch", "CPU architecture", "auto", ["auto"] + platform_methods.architectures))
opts.Add("extra_suffix", "Extra suffix for all binary files", "")
opts.Add("default_renderer", "The default renderer to use on program start", "")
opts.Add("gperf_path", "The path to gperf for generating maps, leave blank to use std::map", "")

opts.Add("CXX", "C++ compiler")
opts.Add("CC", "C compiler")
opts.Add("LINK", "Linker")
opts.Add("CCFLAGS", "Flags for the C and C++ compilers")
opts.Add("CFLAGS", "Flags for the C compiler only")
opts.Add("CXXFLAGS", "Flags for the C++ compiler only")
opts.Add("LINKFLAGS", "Flags for the linker")

for renderer in available_renderers:
    opts.Add(BoolVariable(f"enable_{renderer}_renderer", f"Enable the {renderer} renderer.", True))

opts.Update(env_base)

# Select platform
selected_platform = ""

if env_base["platform"] != "":
    selected_platform = env_base["platform"]
else:
    # Detect automatically
    if sys.platform == "linux":
        selected_platform = "linux"
    elif sys.platform == "darwin":
        selected_platform = "macos"
    elif sys.platform == "win32":
        selected_platform = "win"
    else:
        print("Could not detect platform automatically. Available platforms:")
        
        for platform_name in available_platforms:
            print("\t" + platform_name)
        
        print("\nPlease specify a valid platform with `platform=<platform_name>`")
    
    if selected_platform != "":
        print("Automatically detected platform: " + selected_platform)


if selected_platform in platform_opts:
    for opt in platform_opts[selected_platform]:
        if not opt[0] in opts.keys():
            opts.Add(opt)

# Select renderers
enabled_renderers = []
default_renderer = ""

for renderer in available_renderers:
    if env_base[f"enable_{renderer}_renderer"]:
        enabled_renderers.append(renderer)
        env_base.Append(CPPDEFINES=[f'RENDERER_{renderer.upper()}_ENABLED'])

        tmp_path = "./app/renderers/" + renderer
        sys.path.insert(0, tmp_path)
        import renderer

        renderer.configure(env_base)

        sys.path.remove(tmp_path)
        sys.modules.pop("renderer")



if len(enabled_renderers) == 0:
    print("Cannot compile executable target without any renderers!")
    sys.exit(255)

if env_base["default_renderer"]:
    if env_base["default_renderer"] in enabled_renderers:
        default_renderer = env_base["default_renderer"]
    else:
        print(f'Default renderer {env_base["default_renderer"]} is not enabled')
        sys.exit(255)
else:
    enabled_renderers.sort(key=lambda renderer: renderer_priorities[renderer], reverse=True)
    default_renderer = enabled_renderers[0]

opts.Update(env_base)
env_base["platform"] = selected_platform

env_base["enabled_renderers"] = enabled_renderers
env_base["default_renderer"] = default_renderer

env_base.Prepend(CPPPATH=["#"])


# Configure build
if selected_platform in available_platforms:
    tmp_path = "./platform/" + selected_platform
    sys.path.insert(0, tmp_path)
    import detect

    env = env_base.Clone()

    # Detect if -j is user specified
    initial_num_jobs = env.GetOption("num_jobs")
    altered_num_jobs = initial_num_jobs + 1
    env.SetOption("num_jobs", altered_num_jobs)
    
    if (env.GetOption("num_jobs") == altered_num_jobs):
        # Not specified by user
        cpu_count = os.cpu_count()

        if cpu_count is None:
            print("Couldn't auto-detect CPU count to configure parallelism, defaulting to 1. Specify with `-j<num_jobs>")
            env.SetOption("num_jobs", 1)
        else:
            safe_cpu_count = max(cpu_count - 1, 1)
            print("Detected %d CPU cores for parallelism, using %d cores by default. Override with `-j<num_jobs>" % (cpu_count, safe_cpu_count))
            env.SetOption("num_jobs", safe_cpu_count)
    
    env.extra_suffix = ""
    if env["extra_suffix"] != "":
        env.extra_suffix += "." + env["extra_suffix"]
    
    # Split flags into array
    CCFLAGS = env.get("CCFLAGS", "")
    env["CCFLAGS"] = ""
    env.Append(CCFLAGS=str(CCFLAGS).split())

    CFLAGS = env.get("CFLAGS", "")
    env["CFLAGS"] = ""
    env.Append(CFLAGS=str(CFLAGS).split())

    CXXFLAGS = env.get("CXXFLAGS", "")
    env["CXXFLAGS"] = ""
    env.Append(CXXFLAGS=str(CXXFLAGS).split())

    LINKFLAGS = env.get("LINKFLAGS", "")
    env["LINKFLAGS"] = ""
    env.Append(LINKFLAGS=str(LINKFLAGS).split())

    # Platform specific flags
    for flag in platform_flags[selected_platform]:
        # Overwrite if command line specifies non-auto value
        if (not flag[0] in ARGUMENTS) or ARGUMENTS[flag[0]] == "auto":
            env[flag[0]] = flag[1]
    
    detect.configure(env)

    # Now that everything else has had a chance to configure, we can properly detect and configure modules
    env.module_list = methods.detect_modules("modules", env["enabled_modules"])
    methods.write_modules(env.module_list)

    for module_name in env.module_list:
        env.Append(CPPDEFINES=[f'MODULE_{module_name.upper()}_ENABLED'])

        if os.path.exists("./modules/" + module_name + "/module.py"):
            tmp_path = "./modules/" + module_name
            sys.path.insert(0, tmp_path)
            import module

            if "get_opts" in dir(module):
                for opt in module.get_opts(env):
                    if opt[0] in opts.args.keys():
                        env[opt[0]] = opts.args[opt[0]]
                    else:
                        opts.Add(opt)
                        env[opt[0]] = opt[2]

            sys.path.remove(tmp_path)
            sys.modules.pop("module")

    for module_name in env.module_list:
        if os.path.exists("./modules/" + module_name + "/module.py"):
            tmp_path = "./modules/" + module_name
            sys.path.insert(0, tmp_path)
            import module

            module.configure(env)

            sys.path.remove(tmp_path)
            sys.modules.pop("module")

    Help(opts.GenerateHelpText(env_base))

    print("Building for %s-%s-%s" % (selected_platform, env["arch"], env["target"]))

    # TODO: support msvc

    if env["target"] == "debug":
        env.Append(CCFLAGS=["-gdwarf-4", "-g3", "-Og"])
    else:
        if methods.using_apple_clang(env):
            env.Append(LINKFLAGS=["-Wl,-S", "-Wl,-x", "-Wl,-dead_strip"])
        elif methods.using_clang(env):
            env.Append(LINKFLAGS=["-s"])
        
        env.Append(CCFLAGS=["-O3"])
    
    if env["lto"] != "none":
        print("Using LTO: " + env["lto"])
    
    env.Prepend(CFLAGS=["-std=c17"])
    env.Prepend(CXXFLAGS=["-std=c++20"])

    cc_version = methods.get_compiler_version(env) or -1

    if cc_version == -1:
        print("Couldn't detect compiler version, skipping version checks. This may have consequences.")
    elif cc_version < 10:
        # Both GCC and clang added support for c++20 in version 10, a coinkidink
        print("Detected GCC version < 10, which does not support C++20")
        sys.exit(255)
    
    # Compiler warnings

    # TODO: support msvc

    env.Append(CCFLAGS=["-Wall", "-Wextra"])
    if env["target"] == "release":
        env.Append(CCFLAGS=["-Werror"])
        env.Append(CPPDEFINES=["NDEBUG"])

    if hasattr(detect, "get_program_suffix"):
        suffix = "." + detect.get_program_suffix()
    else:
        suffix = "." + selected_platform
    
    suffix += "." + env["target"]
    suffix += "." + env["arch"]
    suffix += env.extra_suffix

    methods.generate_version_header()

    env["PROGSUFFIX"] = suffix + env["PROGSUFFIX"]
    env["OBJSUFFIX"] = suffix + env["OBJSUFFIX"]
    env["LIBSUFFIX"] = suffix + env["LIBSUFFIX"]

    # compile_commands.json
    env.Tool("compilation_db")
    env.Alias("compiledb", env.CompilationDatabase())
    env.NoClean("compile_commands.json")

    Export("env")
    
    SConscript("assets/SCsub")
    SConscript("modules/SCsub")
    SConscript("core/SCsub")
    SConscript("app/SCsub")
    SConscript("platform/SCsub")
    SConscript("main/SCsub")

    # Prevent from using C compiler (can't infer without sources)
    env["CC"] = env["CXX"]
    exe = env.add_program("#bin/rcalc", [])

    if hasattr(detect, "post_build"):
        post_build = env.CommandNoCache('post_build', exe, detect.post_build)
    
    if env.GetOption('clean'):
        if hasattr(detect, "platform_clean"):
            detect.platform_clean(env)

elif selected_platform != "":
    if selected_platform == "list":
        print("The following platforms are available:\n")
    else:
        print('Invalid target platform "' + selected_platform + '".')
        print("The following platforms are available:\n")
    
    for platform_name in available_platforms:
        print("\t" + platform_name)

    print("\nPlease specify a valid platform with `platform=<platform_name>`")
    
    if selected_platform == "list":
        sys.exit(0)
    else:
        sys.exit(255)


def print_elapsed_time():
    elapsed_time_sec = round(time.time() - start_time, 3)
    time_ms = round((elapsed_time_sec % 1) * 1000)
    print("[Time elapsed: {}.{:03}]".format(time.strftime("%H:%M:%S", time.gmtime(elapsed_time_sec)), time_ms))

atexit.register(print_elapsed_time)