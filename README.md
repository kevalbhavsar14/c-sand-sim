# Simple cellular sand simulation written in C using raylib

## Quick start
Raylib required to compile.
Install raylib from [here](https://www.raylib.com/).

### Windows
Change INCLUDE_PATH and LIBRARY_PATH for your raylib installation in `build.bat`\
Run in cmd or powershell
```cmd
.\build.bat
.\sandSim.exe
```

### Linux
Change INCLUDE_PATH and LIBRARY_PATH for your raylib installation in `build.sh`\
Run in terminal
```bash
chmod +x build.sh
./build.sh
./sandSim
```
> Note: The Linux version is not verified since I only have windows.
> I will not go through the effort of installing raylib on wsl or a vm, copying the repo and verifying
