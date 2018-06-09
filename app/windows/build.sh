#!/bin/bash
# Can build with MSYS2 on Windows
gcc main.c rs232/rs232-win.c -o dist/dist.exe -luser32 -lgdiplus -lshlwapi

