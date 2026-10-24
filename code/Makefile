.PHONY: all configure build run clean

# Windows (mingw32-make) uses the "default" preset; macOS uses "macos"
ifeq ($(OS),Windows_NT)
PRESET = default
EXE = $(CURDIR)/build/QuadM.exe
else
PRESET = macos
EXE = $(CURDIR)/build/QuadM
endif

all: run

# Configure only if the build folder doesn't exist yet (CMake re-configures itself when CMakeLists.txt changes)
build/build.ninja:
	cmake --preset $(PRESET)

configure: build/build.ninja

build: configure
	cmake --build --preset $(PRESET)

run: build
	"$(EXE)"

clean:
	cmake -E remove_directory build
