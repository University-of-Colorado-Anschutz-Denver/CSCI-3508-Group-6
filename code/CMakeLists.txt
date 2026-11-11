cmake_minimum_required(VERSION 3.16)
project(QuadM VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt's code generators (moc, uic, rcc) run automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Pick up every .cpp/.h/.ui/.qrc under src/ and include/; CONFIGURE_DEPENDS re-checks on each build
file(GLOB_RECURSE QUADM_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

add_executable(QuadM ${QUADM_SOURCES})
# Windows: Release builds are GUI programs (no terminal window); Debug keeps the console for qDebug() output
set_target_properties(QuadM PROPERTIES WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>)
target_precompile_headers(QuadM PRIVATE <QtWidgets>)
target_include_directories(QuadM PRIVATE include src)

# Copy ExifTool next to the exe (build/exiftool/) so dev and installed builds use the same layout.
# See third_party/exiftool/README.md for how to download it.
add_custom_target(copy_exiftool ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/exiftool" "${CMAKE_BINARY_DIR}/exiftool"
)
add_dependencies(QuadM copy_exiftool)
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/exiftool/exiftool.exe"
   AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/exiftool/exiftool")
    message(WARNING "ExifTool not found in third_party/exiftool - see third_party/exiftool/README.md")
endif()

# TagLib reads/writes MP3 and WAV tags (ExifTool can't write those). It is downloaded and built
# as part of the project (static), so nothing needs to be installed. Needs git and internet on the first configure.
include(FetchContent)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_BINDINGS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(taglib
    GIT_REPOSITORY https://github.com/taglib/taglib.git
    GIT_TAG v2.0.2
    GIT_SUBMODULES_RECURSE ON # TagLib 2 needs its utfcpp submodule
)
FetchContent_MakeAvailable(taglib)

# When built as a subproject TagLib has no TagLib::tag target and exports no include paths, so add them here.
# Include headers directly, e.g. #include <mpegfile.h>, #include <wavfile.h>
target_link_libraries(QuadM PRIVATE Qt6::Widgets tag)
target_compile_definitions(QuadM PRIVATE TAGLIB_STATIC)
target_include_directories(QuadM PRIVATE "${taglib_SOURCE_DIR}" "${taglib_SOURCE_DIR}/taglib"
    "${taglib_SOURCE_DIR}/taglib/toolkit" "${taglib_SOURCE_DIR}/taglib/mpeg"
    "${taglib_SOURCE_DIR}/taglib/mpeg/id3v2" "${taglib_SOURCE_DIR}/taglib/mpeg/id3v2/frames"
    "${taglib_SOURCE_DIR}/taglib/riff"
    "${taglib_SOURCE_DIR}/taglib/riff/wav" "${taglib_SOURCE_DIR}/taglib/riff/info"
    "${taglib_BINARY_DIR}")

# Copy the Qt DLLs next to the exe so it runs without Qt on PATH
if(WIN32)
    get_target_property(QT_QMAKE_EXE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXE windeployqt HINTS "${QT_BIN_DIR}")
    if(WINDEPLOYQT_EXE)
        add_custom_command(TARGET QuadM POST_BUILD
            COMMAND "${WINDEPLOYQT_EXE}" --no-translations --no-system-d3d-compiler
                    --no-opengl-sw "$<TARGET_FILE:QuadM>"
        )
    endif()
endif()

# UI tests (Qt Test). Not part of the normal build: they are only built on request, see tests/README.md.
find_package(Qt6 QUIET COMPONENTS Test)
if(Qt6Test_FOUND)
    enable_testing()
    set(QUADM_TEST_SOURCES ${QUADM_SOURCES})
    list(FILTER QUADM_TEST_SOURCES EXCLUDE REGEX "/src/main\\.cpp$") # the tests have their own main()
    add_executable(QuadMUiTests EXCLUDE_FROM_ALL tests/ui/tst_quadm_ui.cpp ${QUADM_TEST_SOURCES})
    target_precompile_headers(QuadMUiTests PRIVATE <QtWidgets>) # same as QuadM: some sources rely on it for includes
    target_include_directories(QuadMUiTests PRIVATE include src
        "${taglib_SOURCE_DIR}" "${taglib_SOURCE_DIR}/taglib"
        "${taglib_SOURCE_DIR}/taglib/toolkit" "${taglib_SOURCE_DIR}/taglib/mpeg"
        "${taglib_SOURCE_DIR}/taglib/mpeg/id3v2" "${taglib_SOURCE_DIR}/taglib/mpeg/id3v2/frames"
        "${taglib_SOURCE_DIR}/taglib/riff"
        "${taglib_SOURCE_DIR}/taglib/riff/wav" "${taglib_SOURCE_DIR}/taglib/riff/info"
        "${taglib_BINARY_DIR}")
    target_link_libraries(QuadMUiTests PRIVATE Qt6::Widgets Qt6::Test tag)
    target_compile_definitions(QuadMUiTests PRIVATE TAGLIB_STATIC)
    add_dependencies(QuadMUiTests copy_exiftool)
    if(WIN32 AND WINDEPLOYQT_EXE)
        add_custom_command(TARGET QuadMUiTests POST_BUILD
            COMMAND "${WINDEPLOYQT_EXE}" --no-translations --no-system-d3d-compiler
                    --no-opengl-sw "$<TARGET_FILE:QuadMUiTests>"
        )
    endif()
    # Run from the code folder (the app loads its theme CSS relative to it). Windows uses the
    # real window system; Linux/macOS use Qt's offscreen platform so no display is needed.
    add_test(NAME QuadMUiTests COMMAND QuadMUiTests WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NOT WIN32)
        set_tests_properties(QuadMUiTests PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    endif()
endif()

# Library example:
# add_library(mylib src/lib.cpp)
# target_link_libraries(QuadM PRIVATE mylib)