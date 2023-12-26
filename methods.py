import glob
import os
import subprocess
from collections import OrderedDict
import sys
import tempfile
import shutil


def using_gcc(env):
    return "gcc" in os.path.basename(env["CC"])


def using_clang(env):
    return "clang" in os.path.basename(env["CC"])


def using_apple_clang(env):
    if not using_clang(env):
        return False

    try:
        version = subprocess.check_output([env.subst(env["CXX"]), "--version"]).strip().decode("utf-8")
    except (subprocess.CalledProcessError, OSError):
        print("Couldn't parse CXX to infer compiler version.")
        return False
    
    return version.startswith("Apple")


def get_compiler_version(env):
    # TODO: Support msvc

    try:
        version = subprocess.check_output([env.subst(env["CXX"]), "-dumpversion"]).strip().decode("utf-8")
        version = int(version.split(".")[0])
    except (subprocess.CalledProcessError, OSError, ValueError):
        print("Couldn't parse CXX to infer compiler version.")
        return None

    return version


def add_source_files(self, sources, files, allow_gen=False, recursive=False):
    # Convert to list of absolute paths, expanding wildcards
    if isinstance(files, (str, bytes)):
        if files.startswith("#"):
            if "*" in files:
                print("Error: Wildcards can't be expanded in SCONS project-absolute path: '{}'".format(files))
                return
            files = [files]
        else:
            skip_gen_cpp = "*" in files
            dir_path = self.Dir(".").abspath
            files = sorted(glob.glob(dir_path + "/" + files, recursive=recursive))
            if skip_gen_cpp and not allow_gen:
                files = [f for f in files if not f.endswith(".gen.cpp")]
    
    for path in files:
        obj = self.Object(path)

        if obj in sources:
            print('Warning: Object "{}" is already included in environment sources'.format(obj))
            continue
        
        sources.append(obj)


def add_library(env, name, sources, **args):
    library = env.Library(name, sources, **args)
    env.NoCache(library)
    return library


def add_program(env, name, sources, **args):
    program = env.Program(name, sources, **args)
    env.NoCache(program)
    return program


def get_version_info(silent=False):
    build_name = "custom_build"
    if os.getenv("BUILD_NAME") != None:
        build_name = str(os.getenv("BUILD_NAME"))
        if not silent:
            print(f"Using custom build name: '{build_name}'.")

    import version

    version_info = {
        "short_name": str(version.short_name),
        "name": str(version.name),
        "major": int(version.major),
        "minor": int(version.minor),
        "patch": int(version.patch),
        "status": str(version.status),
        "build": str(build_name),
        "year": int(version.year),
    }

    # For dev snapshots (alpha, beta, RC, etc.) we do not commit status change to Git,
    # so this define provides a way to override it without having to modify the source.
    if os.getenv("VERSION_STATUS") != None:
        version_info["status"] = str(os.getenv("VERSION_STATUS"))
        if not silent:
            print(f"Using version status '{version_info['status']}', overriding the original '{version.status}'.")

    # Parse Git hash if we're in a Git repo.
    githash = ""
    gitfolder = ".git"

    if os.path.isfile(".git"):
        module_folder = open(".git", "r").readline().strip()
        if module_folder.startswith("gitdir: "):
            gitfolder = module_folder[8:]

    if os.path.isfile(os.path.join(gitfolder, "HEAD")):
        head = open(os.path.join(gitfolder, "HEAD"), "r", encoding="utf8").readline().strip()
        if head.startswith("ref: "):
            ref = head[5:]
            # If this directory is a Git worktree instead of a root clone.
            parts = gitfolder.split("/")
            if len(parts) > 2 and parts[-2] == "worktrees":
                gitfolder = "/".join(parts[0:-2])
            head = os.path.join(gitfolder, ref)
            packedrefs = os.path.join(gitfolder, "packed-refs")
            if os.path.isfile(head):
                githash = open(head, "r").readline().strip()
            elif os.path.isfile(packedrefs):
                # Git may pack refs into a single file. This code searches .git/packed-refs file for the current ref's hash.
                # https://mirrors.edge.kernel.org/pub/software/scm/git/docs/git-pack-refs.html
                for line in open(packedrefs, "r").read().splitlines():
                    if line.startswith("#"):
                        continue
                    (line_hash, line_ref) = line.split(" ")
                    if ref == line_ref:
                        githash = line_hash
                        break
        else:
            githash = head

    version_info["git_hash"] = githash

    if not os.path.isfile(".git") and os.getenv("OVERRIDE_GIT_HASH") != None:
        version_info["git_hash"] = str(os.getenv("OVERRIDE_GIT_HASH"))

    return version_info


def generate_version_header():
    version_info = get_version_info()

    # NOTE: It is safe to generate these files here, since this is still executed serially.

    f = open("core/version_generated.gen.h", "w")
    f.write(
        """/* THIS FILE IS GENERATED DO NOT EDIT */
#ifndef VERSION_GENERATED_GEN_H
#define VERSION_GENERATED_GEN_H
#define VERSION_SHORT_NAME "{short_name}"
#define VERSION_NAME "{name}"
#define VERSION_MAJOR {major}
#define VERSION_MINOR {minor}
#define VERSION_PATCH {patch}
#define VERSION_STATUS "{status}"
#define VERSION_BUILD "{build}"
#define VERSION_YEAR {year}
#endif // VERSION_GENERATED_GEN_H
""".format(
            **version_info
        )
    )
    f.close()

    fhash = open("core/version_hash.gen.cpp", "w")
    fhash.write(
        """/* THIS FILE IS GENERATED DO NOT EDIT */
#include "core/version.h"
const char *const VERSION_HASH = "{git_hash}";
""".format(
            **version_info
        )
    )
    fhash.close()


def detect_modules(search_path, enabled_modules):
    modules = OrderedDict()

    def add_module(path, registered):
        module_name = os.path.basename(path)
        module_path = path.replace("\\", "/")
        modules[module_name] = { "path": module_path, "registered": registered }
    
    def get_files(path):
        files = glob.glob(os.path.join(path, "*"))
        files.sort()
        return files
    
    if is_module(search_path, enabled_modules):
        add_module(search_path, is_module_registered(search_path))
    
    for path in get_files(search_path):
        if is_module(path, enabled_modules):
            add_module(path, is_module_registered(path))
    
    return modules
    

def is_module(path, enabled_modules):
    if not os.path.basename(path) in enabled_modules:
        return False

    if not os.path.isdir(path):
        return False
    
    if not os.path.exists(os.path.join(path, "SCsub")):
        return False
    
    return True


def is_module_registered(path):
    if not os.path.isdir(path):
        return False
    
    if not os.path.exists(os.path.join(path, "register_module.h")):
        return False
    
    return True


def write_modules(modules):
    includes_cpp = ""
    initialize_cpp = ""
    cleanup_cpp = ""

    for name, module in modules.items():
        if not module["registered"]:
            continue
            
        path = module["path"]
        try:
            with open(os.path.join(path, "register_module.h")):
                includes_cpp += '#include "' + path + '/register_module.h"\n'
                initialize_cpp += "\tinitialize_" + name + "_module();\n";
                cleanup_cpp += "\tcleanup_" + name + "_module();\n"
        except OSError:
            pass
    
    modules_cpp = """
// This file is generated. Any modifications will be overwritten.

#include "modules/register_modules.h"

%s

namespace RCalc {

void initialize_modules()
{
%s
}

void cleanup_modules()
{
%s
}

}
""" % (
        includes_cpp,
        initialize_cpp,
        cleanup_cpp
    )

    with open("modules/register_modules.gen.cpp", "w") as f:
        f.write(modules_cpp)


def CommandNoCache(env, target, sources, command, **args):
    result = env.Command(target, sources, command, **args)
    env.NoCache(result)
    return result


def Run(env, function, short_message, subprocess=True):
    from SCons.Script import Action
    from platform_methods import run_in_subprocess

    if not subprocess:
        return Action(function, short_message)
    else:
        return Action(run_in_subprocess(function), short_message)


def use_windows_spawn_fix(self, platform=None):
    if os.name != "nt":
        return  # not needed, only for windows

    # On Windows, due to the limited command line length, when creating a static library
    # from a very high number of objects SCons will invoke "ar" once per object file;
    # that makes object files with same names to be overwritten so the last wins and
    # the library loses symbols defined by overwritten objects.
    # By enabling quick append instead of the default mode (replacing), libraries will
    # got built correctly regardless the invocation strategy.
    # Furthermore, since SCons will rebuild the library from scratch when an object file
    # changes, no multiple versions of the same object file will be present.
    self.Replace(ARFLAGS="q")

    def mySubProcess(cmdline, env):
        startupinfo = subprocess.STARTUPINFO()
        startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        popen_args = {
            "stdin": subprocess.PIPE,
            "stdout": subprocess.PIPE,
            "stderr": subprocess.PIPE,
            "startupinfo": startupinfo,
            "shell": False,
            "env": env,
        }
        if sys.version_info >= (3, 7, 0):
            popen_args["text"] = True
        proc = subprocess.Popen(cmdline, **popen_args)
        _, err = proc.communicate()
        rv = proc.wait()
        if rv:
            print("=====")
            print(err)
            print("=====")
        return rv

    def mySpawn(sh, escape, cmd, args, env):
        newargs = " ".join(args[1:])
        cmdline = cmd + " " + newargs

        rv = 0
        env = {str(key): str(value) for key, value in iter(env.items())}
        if len(cmdline) > 32000 and cmd.endswith("ar"):
            cmdline = cmd + " " + args[1] + " " + args[2] + " "
            for i in range(3, len(args)):
                rv = mySubProcess(cmdline + args[i], env)
                if rv:
                    break
        else:
            rv = mySubProcess(cmdline, env)

        return rv

    self["SPAWN"] = mySpawn


def build_static_lib(target, source, env):
    working_dir = tempfile.mkdtemp()
    env["staticlib_working_dir"] = working_dir

    object_dir = os.path.join(working_dir, "objects")
    os.mkdir(object_dir)

    for lib in source:
        subprocess.run(["ar", "-x", lib], capture_output=True, cwd=object_dir, check=True)
    
    objects = glob.glob(os.path.join(object_dir, "*.o"))

    lib = env.add_library(target[0], objects)

    env.CommandNoCache(
        "#bin/lib_rcalc",
        lib,
        env.Run(package_static_lib, "Packing static library.")
    )


def package_static_lib(target, source, env):
    working_dir = tempfile.mkdtemp()

    lib_dir = os.path.join(working_dir, "lib")
    os.mkdir(lib_dir)
    shutil.move(source[0], os.path.join(lib_dir, os.path.basename(source[0])))

    include_dir = os.path.join(working_dir, "include")
    os.mkdir(include_dir)

    headers = [
        "core/coredef.h",
        "core/error.h",
        "core/version.h",
        "core/types.h",
        "core/version_generated.gen.h",
        "core/value.h",
        "core/units/units.h",
        "core/display_tags.h",
        "core/logger.h",
        "core/format.h",
        "core/filter.h",
        "core/anycase_str.h",
        "core/memory/allocator.h",
        "core/memory/cowvec.h",
        "app/renderers/renderer.h",
        "app/operators/operators.h",
        "app/operators/stats.h",
        "app/application.h",
        "app/stack.h",
        "app/commands/commands.h",
        "app/displayable/displayable.h",
        "app/help_text.h",
        "app/autocomplete.h",
        "modules/register_modules.h",
        "modules/bigint/bigint.h",
        "assets/license.gen.h"
    ]

    copy_modules = ["glm"]

    modulemap = ['module RCalc {']

    for header in headers:
        header_dir = os.path.dirname(header)
        header_dir = os.path.join(include_dir, header_dir)
        os.makedirs(header_dir, exist_ok=True)
        shutil.copy(header, os.path.join(include_dir, header))

        modulemap.append(f'\theader "{header}"')
    
    for copy_module in copy_modules:
        module_path = os.path.join("modules", copy_module)
        shutil.copytree(module_path, os.path.join(include_dir, module_path))
    
    modulemap.extend([
        '',
        '\texport *',
        '}'
    ])

    with open(os.path.join(include_dir, "module.modulemap"), 'w') as f:
        f.write('\n'.join(modulemap))

    shutil.copy("LICENSE.md", working_dir)
    
    shutil.make_archive(target[0] + env.combined_suffix, "zip", working_dir, ".")

    shutil.rmtree(working_dir)
    shutil.rmtree(env["staticlib_working_dir"])


def detect_darwin_sdk_path(platform, env):
    sdk_name = ""
    if platform == "macos":
        sdk_name = "macosx"
        var_name = "MACOS_SDK_PATH"
    elif platform == "ios":
        sdk_name = "iphoneos"
        var_name = "IOS_SDK_PATH"
    elif platform == "iossimulator":
        sdk_name = "iphonesimulator"
        var_name = "IOS_SDK_PATH"
    else:
        raise Exception("Invalid platform argument passed to detect_darwin_sdk_path")

    if not env[var_name]:
        try:
            sdk_path = subprocess.check_output(["xcrun", "--sdk", sdk_name, "--show-sdk-path"]).strip().decode("utf-8")
            if sdk_path:
                env[var_name] = sdk_path
        except (subprocess.CalledProcessError, OSError):
            print("Failed to find SDK path while running xcrun --sdk {} --show-sdk-path.".format(sdk_name))
            raise


def precious_program(env, program, sources, **args):
    program = env.ProgramOriginal(program, sources, **args)
    env.Precious(program)
    return program