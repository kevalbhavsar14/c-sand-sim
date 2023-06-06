INCLUDE_PATH="C:\raylib\raylib\src"
LIBRARY_PATH="C:\raylib\raylib\src"
CFLAGS="-O3 -Wall -Werror -Wextra"
LIBS="-lraylib -lopengl32 -lgdi32 -lwinmm"
gcc sandSim.c $CFLAGS -o sandSim -I $INCLUDE_PATH -L $LIBRARY_PATH $LIBS
