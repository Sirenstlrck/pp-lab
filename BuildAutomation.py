#!/usr/bin/python3

# Straightforward :)

import argparse
import os
import subprocess
import sys
from enum import Enum
import shutil

class Platform(Enum):
    Linux = "Linux"
    Windows = "Win32"

def PlatformFromStr(s: str) -> Platform:
    if s == "linux" or s == "linux2":
        return Platform.Linux
    elif s == "win32":
        return Platform.Windows
    else:
        raise Exception("Unsupported platform")

class Generator(Enum):
    VS17_2022 = "Visual Studio 17 2022"
    UnixMakefiles = "Unix Makefiles"

def GeneratorFromPlatform(platform: Platform) -> Generator:
    match platform:
        case Platform.Windows:
            return  Generator.VS17_2022
        case Platform.Linux:
            return  Generator.UnixMakefiles

class Profile(Enum):
    Release = "Release"
    Debug = "Debug"

class RunArgs:
    generator: Generator
    profile: Profile
    useConanToolchainFile: bool
    outDir: str

def GetCmakeString(args: RunArgs) -> str:
    toolchainFileArg: str = ""
    if args.useConanToolchainFile:
        if args.generator == Generator.VS17_2022:
            toolchainFileArg = "-DCMAKE_TOOLCHAIN_FILE=build/generators/conan_toolchain.cmake"
        elif args.generator == Generator.UnixMakefiles:
            toolchainFileArg = f"-DCMAKE_TOOLCHAIN_FILE=build/{args.profile.value}/generators/conan_toolchain.cmake"

    if args.generator == Generator.VS17_2022:
        return f"cmake . -B {args.outDir} -G \"{args.generator.value}\" \"{toolchainFileArg}\""
    elif args.generator == Generator.UnixMakefiles:
        return f"cmake . -B {args.outDir} -G \"{args.generator.value}\" -DCMAKE_BUILD_TYPE={args.profile.value} \"{toolchainFileArg}\""
    else:
        raise Exception("Unsupported generator")

def GetBuildString(args: RunArgs) -> str:
    if args.generator == Generator.VS17_2022:
        return f"cmake --build {args.outDir}"# --profile {args.profile.value}"
    elif args.generator == Generator.UnixMakefiles:
        return f"cmake --build {args.outDir}"
    else:
        raise Exception("Unsupported generator")
    
def RunCommand(command: str) -> int:
    print(f"[Executing '{command}']")
    result = subprocess.run(command, shell=True)
    return result.returncode

if __name__ == "__main__":
    try:
        parser = argparse.ArgumentParser()
        
        parser.add_argument("-p", "--profile", help="Build profile [Debug, Release]")
        parser.add_argument("-o", "--out", help="Output directory name")
        parser.add_argument("-no-conan", action="store_true", help="Skip conan run and don't use conan toolchain file")
        parser.add_argument("-skip-conan", action="store_true", help="Skip conan run")
        parser.add_argument("-skip-cmake", action="store_true", help= "Skip build files generation")
        parser.add_argument("-skip-build", action="store_true", help="Skip build")
        parser.add_argument("-clean-first", action="store_true", help="Clear out folder first")
        args = parser.parse_args()
        
        runArgs = RunArgs()

        if args.no_conan:
            runArgs.useConanToolchainFile = False
            args.skip_conan = True
        else:
            runArgs.useConanToolchainFile = True
        
        platform = PlatformFromStr(sys.platform)
        runArgs.generator = GeneratorFromPlatform(platform)
        scriptDir = os.path.dirname(os.path.realpath(__file__))
        if not args.out:
            outDir = "out"
        else:
            outDir = runArgs.out

        if not args.profile:
            runArgs.profile = Profile.Debug
        else:
            runArgs.profile = args.profile

        runArgs.outDir = os.path.join(scriptDir, outDir)
        if args.clean_first and os.path.exists(runArgs.outDir):
            print(f"[Removing '{runArgs.outDir}' directory]")
            shutil.rmtree(runArgs.outDir)

        if not args.skip_conan:
            print("[Running conan]")
            res = RunCommand(f"conan install . --output-folder={runArgs.outDir} --build=missing --profile={runArgs.profile.value.lower()}")
            if res != 0:
                raise Exception("Error during conan running")
        else:
            print("[Skipping conan stage]")

        if not args.skip_cmake:
            print("[Running build files generation]")
            cmd = GetCmakeString(runArgs)
            res = RunCommand(cmd)
            if res != 0:
                raise Exception("Error during generation of build files")
        else:
            print("[Skipping build files generation]")

        if not args.skip_build:
            print("[Building]")
            if not os.path.exists(runArgs.outDir):
                raise Exception("Build folder does not exist")
            cmd = GetBuildString(runArgs)
            res = RunCommand(cmd)
            if res != 0:
                raise Exception("Error during build")
        else:
            print("[Skipping build stage]")

    except Exception as e:
        print(f"An error occured: '{e}'")