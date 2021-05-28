#!/usr/bin/env python
"""
Python script to help manage project settings

Usage:
    setup.py dev [-f]
    setup.py clean
    setup.py -h | --help | --version
"""
import os
import shutil
import subprocess
import sys
from pathlib import Path

from docopt import docopt

VERSION = "1.0.0"

BUILD_DIR = Path("./build/")
CMAKE_DB_PATH = Path("./build/compile_commands.json")


def dir_empty(dir_path):
    return not next(os.scandir(dir_path), None)


def clean():
    # Delete and recreate build dir to remove contents
    if BUILD_DIR.exists():
        shutil.rmtree(BUILD_DIR)
    BUILD_DIR.mkdir()

    symlink = Path(Path.cwd(), "compile_commands.json")
    try:
        symlink.unlink()
    except FileNotFoundError:
        pass


def dev():
    subprocess.run(
        [
            "cmake",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-B",
            str(BUILD_DIR),
        ]
    )
    Path(Path.cwd(), "compile_commands.json").symlink_to(CMAKE_DB_PATH)


if __name__ == "__main__":
    args = docopt(__doc__, version=VERSION)
    if not Path(Path.cwd(), "./CMakeLists.txt").exists():
        sys.exit("Missing CMakeLists.txt")

    if args.get("dev") or not args.get("clean"):
        if args.get("-f"):
            clean()
        elif not dir_empty(BUILD_DIR):
            sys.exit("Build dir already exists! Override with -f")
        dev()
    elif args.get("clean"):
        print("Cleaning up...")
        clean()
