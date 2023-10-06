import glob
import os
import subprocess
from collections import OrderedDict


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


def add_source_files(self, sources, files, allow_gen=False):
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
            files = sorted(glob.glob(dir_path + "/" + files))
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

void initialize_modules()
{
%s
}

void cleanup_modules()
{
%s
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