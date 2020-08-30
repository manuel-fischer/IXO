#!/usr/bin/env python3
import os
import shutil

# include and lib directories should end in include and lib
STRICT_PATHS = True

def find_exe(name):
    cmd = "where" if os.name == "nt" else "which"
    with os.popen("%s %s"%(cmd, name)) as f:
        return f.read().strip()


def check_directory(path, suffix):
        if STRICT_PATHS and not path.endswith(suffix):
            return "{!r} is not ending in {!s}".format(path, suffix)
        if not os.path.isdir(path):
            return "{!r} is not a directory".format(path)
        if path == suffix:
            return("The path you have provided contains the files that should "
                   "be copied. The directory you should input here is the "
                   "directory the files should be copied to")
        return None


def input_yesno(prompt):
    while True:
        yesno = input(prompt)
        if yesno.lower().startswith("y"): return True
        if yesno.lower().startswith("n"): return False
        


def input_valid_directory(prompt, suffix):
    while True:
        path = input(prompt)
        path = path.rstrip("/\\")
        err = check_directory(path, suffix)
        if err is not None:
            print(err)
        else:
            return path


def noisy_copy(src, dst):
    print("copy", src, "-->", dst)
    try:
        shutil.copy2(src, dst)
    except:
        print("copy failed")
        raise


if __name__ == "__main__":        
    while True:
        if input_yesno("Auto detect paths [yn]? "):
            gcc_path    = find_exe("gcc")
            bin_dir     = os.path.dirname(gcc_path)
            com_dir     = os.path.dirname(bin_dir)
            lib_dir     = os.path.join(com_dir, "lib")
            include_dir = os.path.join(com_dir, "include")
            print("Using following paths")
            print(lib_dir)
            print(include_dir)
            if check_directory(lib_dir, "lib") is not None or \
               check_directory(include_dir, "include") is not None:
                print("These paths are not valid")
            elif input_yesno("confirm [yn]? "):
                break
        else:                
            print("Lookup your preferred include/ and lib/ directories, to which the "
                  "library should be copied to")
            lib_dir     = input_valid_directory("where is lib/? ", "lib")
            include_dir = input_valid_directory("where is include/? ", "include")
            break


    print("Building library")
    os.system("make build-lib")

    ixo_include_dir = include_dir+"/IXO"

    with open("includes.txt", "r") as f:
        include_files = [l for l in map(str.strip, f.readlines()) if l and l[0] != '#']

    try:
        noisy_copy("lib/libIXO.a", lib_dir)
        os.makedirs(ixo_include_dir,exist_ok = True)
        for inc_file in include_files:
            noisy_copy("include/"+inc_file, ixo_include_dir)

    except PermissionError:
        if os.name == 'nt':
            print("You might need to run the script as an admin")
        else:
            print("You might need to run the script as root/with sudo")
    else:
        print("Successfully installed library files")
