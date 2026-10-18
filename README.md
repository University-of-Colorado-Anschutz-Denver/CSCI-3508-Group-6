# Make files

## CMakeLists.txt

```
Made to be compatible with Mac. Added one line (line 22) to exclude files from the build firectory (idk if it the problem is just for me but without the exclusion, I run into a failed linker command). 
KEY FOR WORKING: 
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.qrc"

These three lines were refactored to match file path I was using. Will change soon to be in line with what everyone else has.

```

## CMakePresets.json

```
Changes made to have it compatible with Mac. Should generally work with the presets Harrison wrote on windows.
```
