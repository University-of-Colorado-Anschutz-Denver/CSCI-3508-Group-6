## Make files

### CMakeLists.txt

```
Made to be compatible with Mac. Added one line (line 22) to exclude files from the build firectory (idk if it the problem is just for me but without the exclusion, I run into a failed linker command). 
KEY FOR WORKING: 
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/QuadMGUI/*.qrc"

"list(FILTER QUADM_SOURCES EXCLUDE REGEX "/build/")"

These three lines were refactored to match file path I was using. Will change soon to be in line with what everyone else has.

```

### CMakePresets.json

```
Changes made to have it compatible with Mac. Should generally work with the presets Harrison wrote on windows.
```

### running
```
If on windows copy paste the og cmake files to the ones in the directory, but just update the .txt file to be in line with the file paths I have and be sure to include the "exclude line" 
```

## files

### .ui file
```
The template xml file. All the styling and widgets are managed here
```

### mainwindow (.cpp and .h)

```
This one is basically the class implemention for the mainwindow. Contains functions such as loadData, to read the directories from a .txt file, and loadDirectory, to get the filepath from the file and reuse it to populate subcontents.
Constuctor: Creates a window, then loads the data for the root. And for the content panel, when a folder is double clicked it makes sure its a directory and matches the name along with ensuring that "current root" is consistent to poulate the sub directory. If the user clicks a file it should run a preview method to populate that panel.   
```

### .qrc

```
Communicates the external resources like data files, images, and in this case a placeholder gif.
```

### bugs
```
when first running the program, the content panel displays the same contents as the root, and the back button smh doesn't work.
```

### Quick progress report
```
Selecting the GUI platform:
Considered FLTK and Dear ImGui. Those two were tempting as they have comparatively faster  startup time, and have less overhead (time and memory). But FLTK was for simple applications as it is lightweight, and Dear ImGui is open source but didn't go into too much details. But opted for Qt as it has its own design studio, and can produce  polished outputs. It also has some good documentation that I felt was easier to follow and there are quite a few videos on how to work with it.

v.0.1.0:
First read the QT docs and setup the environment. Then created a new project to get the window working. 
Then experimented with the design studio and the .ui file to gain some understanding on the framework. 
Then startted lining up the widgets to create a decent looking layout.
Following that, learnt how to incorporate external resources in the project. Then finally ran the .ui through chatGPT to fix some formatting and padding between the elements. Finally added the needed cpp to populate the two columns. 
```
