INCLUDE_PATH="your_raylib_path"
LIBRARY_PATH="your_raylib_path"
CFLAGS="-O3 -Wall -Wextra"
LIBS="-lraylib -lm" # add neccessary libs since this is not tested
gcc sandSim.c $CFLAGS -o sandSim -I $INCLUDE_PATH -L $LIBRARY_PATH $LIBS
