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
target_link_libraries(QuadM PRIVATE Qt6::Widgets)

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

# Library example:
# add_library(mylib src/lib.cpp)
# target_link_libraries(QuadM PRIVATE mylib)