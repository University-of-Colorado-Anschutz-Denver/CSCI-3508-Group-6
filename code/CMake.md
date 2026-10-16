# Setup and Build Guide

## 1. Install CMake

```powershell
winget install Kitware.CMake
```

Close and reopen your terminal, then check it worked:

```powershell
cmake --version
```

## 2. Install Qt

Download the Qt online installer from https://www.qt.io/download-open-source and install it to the default location, `C:\Qt`.

When choosing components, select:
- **Qt 6.12.0 → MinGW 64-bit**
- **Developer and Designer Tools → MinGW 13.1.0 64-bit** (the compiler)
- **Developer and Designer Tools → Ninja** (the build tool)

You can skip other stuff (Sources, Debug Information Files, other kits). It saves several GB.

## 3. Build and run

Open a terminal in the `code` folder and run:

```powershell
cmake --preset default
cmake --build --preset default
.\build\QuadM.exe
```

- `cmake --preset default` configures the project. The compiler, Ninja and Qt paths come from `CMakePresets.json`, so you don't need to change your PATH. Run it once, and again only if you edit `CMakePresets.json` or delete `build\`.
- `cmake --build --preset default` compiles. After the first build it only recompiles files that changed.
- New `.cpp` and `.h` files in `src/` and `include/` are picked up automatically. `CMakeLists.txt` re-configures itself when you edit it.

If you installed a different Qt version, edit the `C:/Qt/6.12.0` and `mingw1310_64` paths in `CMakePresets.json`.

## 4. Shortcut with make (optional)

The `Makefile` runs the same commands for you. In the `code` folder:

```powershell
mingw32-make          # configure (if needed), build, then run
mingw32-make build    # configure and build only
mingw32-make run      # build, then run
mingw32-make clean    # delete build\
```

Windows doesn't include `make`. `mingw32-make` comes with Qt's MinGW at `C:\Qt\Tools\mingw1310_64\bin`. If the command isn't found, add that folder to your PATH, or run it by its full path.

## 5. Using VS Code (optional)

Install the **CMake Tools** extension (`ms-vscode.cmake-tools`). It reads `CMakePresets.json`, builds and runs from the status bar, and gives IntelliSense the Qt include paths (this fixes `cannot open source file "QApplication"` errors).

When VS Code asks you to select a configure preset, choose **Qt 6.12 MinGW (Ninja)**. Don't choose "Add a New Preset...".

![Select the Qt 6.12 MinGW (Ninja) configure preset](images/select-preset.png)

If it asks for a build preset, choose `default`. If it can't find `CMakeLists.txt`, add this to `.vscode/settings.json`:

```json
"cmake.sourceDirectory": "${workspaceFolder}/code"
```

## 6. Setting.json

For Intellicense Purposes, you can use this defualt settings.json. Put it in a new folder that you make called .vscode and add settings.json to it

```json
{
  "C_Cpp.default.compileCommands": "${workspaceFolder}/code/build/compile_commands.json",
  "C_Cpp.default.compilerPath": "C:/Qt/Tools/mingw1310_64/bin/g++.exe",
  "C_Cpp.default.intelliSenseMode": "windows-gcc-x64",
  "cmake.sourceDirectory": "[Path_To_Repository_Code_Directory]"
}

```

`${workspaceFolder}` can stay like that, do not change it, Change the section in brackets `[]`.

For Example: This is my Path`"C:/Users/Harrison Julius/Desktop/SEProjectGroupSix/code"`
