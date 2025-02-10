============
Requirements
============
In order to properly build this project msys2, g++, gcc, mingw-make have to be enstalled.

Install msys2 then open ucrt64 and use commands below:
pacman -Syu
pacman -Su
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-make

===========================
Links to external libraries
===========================
https://github.com/troldal/OpenXLSX

=====================================================
PATCH FOR OPENXLSX for g++(MINGW32) in XLDocument.cpp 
=====================================================
// ===== External Includes ===== //
#include <algorithm>
#ifdef ENABLE_NOWIDE
#    include <nowide/fstream.hpp>
#endif
#if defined(_WIN32)
#    include <random>
#    ifndef __MINGW32__
#        define stat _stat
#    endif
// #    define stat _stat    // _stat should be available in standard environment on Windows
#endif
#include <pugixml.hpp>
#include <sys/stat.h>     // for stat, to test if a file exists and if a file is a directory


=====================
Tasks.json for VSCode
=====================
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build CMake Project",
            "type": "shell",
            "command": "cmd.exe",
            "args": [
                "/c", 
                "build.bat"
            ],
            "problemMatcher": [],
            "group": {
                "kind": "build",
                "isDefault": true
            }
        }
    ]
}

=======================
Modifications to VScode
=======================
ctrl+shift+p -> "C/C++: Edit Configurations (UI)" -> include path -> ${workspaceFolder}/**