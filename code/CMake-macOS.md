# Setup and Build Guide For macOS

This is the macOS version of [CMake.md](CMake.md) (Windows).

## 1. Install the tools

Install the Xcode command line tools (the compiler) if you don't have them:

```bash
xcode-select --install
```

Install [Homebrew](https://brew.sh) if you don't have it, then install CMake, Ninja and Qt:

```bash
brew install cmake ninja qt
```

Check it worked:

```bash
cmake --version
ninja --version
```

> **Using the Qt online installer instead of Homebrew?** That works too. Install **Qt 6 → macOS** to the default location (`~/Qt`), then pass its path when configuring (see step 2).

## 2. Build and run

Open a terminal in the `code` folder and run:

```bash
cmake --preset macos
cmake --build --preset macos
./build/QuadM
```

- `cmake --preset macos` configures the project using the `macos` preset in `CMakePresets.json`. Run it once, and again only if you edit `CMakePresets.json` or delete `build/`.
- `cmake --build --preset macos` compiles. After the first build it only recompiles files that changed.
- New `.cpp` and `.h` files in `src/` and `include/` are picked up automatically. `CMakeLists.txt` re-configures itself when you edit it.

If CMake says it **can't find Qt6**, tell it where Qt is:

```bash
# Homebrew
cmake --preset macos -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"

# Qt online installer (adjust the version and folder to what you installed)
cmake --preset macos -DCMAKE_PREFIX_PATH="$HOME/Qt/6.12.0/macos"
```

If you change Qt, delete `build/` first (`rm -rf build`) so the old path isn't cached.

## 3. Shortcut with make (optional)

macOS includes `make`. The `Makefile` picks the `macos` preset automatically. In the `code` folder:

```bash
make          # configure (if needed), build, then run
make build    # configure and build only
make run      # build, then run
make clean    # delete build/
```

If you needed `-DCMAKE_PREFIX_PATH` in step 2, run the configure command by hand once before using `make`.

## 4. Using VS Code (optional)

Install the **CMake Tools** extension (`ms-vscode.cmake-tools`). It reads `CMakePresets.json`, builds and runs from the status bar, and gives IntelliSense the Qt include paths (this fixes `cannot open source file "QApplication"` errors).

When VS Code asks you to select a configure preset, choose **Qt 6 macOS (Ninja)**. Don't choose "Add a New Preset...". The Windows preset is hidden on macOS.

If it asks for a build preset, choose `macos`. If it can't find `CMakeLists.txt`, add this to `.vscode/settings.json`:

```json
"cmake.sourceDirectory": "${workspaceFolder}/code"
```

## 5. settings.json

For IntelliSense, create a folder called `.vscode` in the repository root and add a `settings.json` to it:

```json
{
  "C_Cpp.default.compileCommands": "${workspaceFolder}/code/build/compile_commands.json",
  "C_Cpp.default.intelliSenseMode": "macos-clang-arm64",
  "cmake.sourceDirectory": "${workspaceFolder}/code"
}
```

- Use `macos-clang-x64` instead of `macos-clang-arm64` if you have an Intel Mac.
- `compile_commands.json` is created by the build in step 2, so build once first.

## Troubleshooting

- **`zsh: command not found: brew`** – Close and reopen the terminal after installing Homebrew. On Apple Silicon, Homebrew may print a `eval "$(/opt/homebrew/bin/brew shellenv)"` line at the end of its install; run it and add it to `~/.zprofile`.
