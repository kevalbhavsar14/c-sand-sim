@echo off
rem This is the path for my windows installation of Raylib
set INCLUDE_PATH="C:\raylib\raylib\src"
set LIBRARY_PATH="C:\raylib\raylib\src"
set CFLAGS=-O3 -Wall -Werror -Wextra
set LIBS=-lraylib -lopengl32 -lgdi32 -lwinmm
@echo on
gcc sandSim.c %CFLAGS% -o sandSim.exe -I %INCLUDE_PATH% -L %LIBRARY_PATH% %LIBS%
