.PHONY: all configure build run clean

all: run

# Configure only if the build folder doesn't exist yet (CMake re-configures itself when CMakeLists.txt changes)
build/build.ninja:
	cmake --preset default

configure: build/build.ninja

build: configure
	cmake --build --preset default

run: build
	"$(CURDIR)/build/QuadM.exe"

clean:
	cmake -E remove_directory build
