## files

### .ui file
```
The template xml file. All the styling and widgets are managed here
```

### mainwindow (.cpp and .h)

```
This one is basically the class implemention for the mainwindow. Contains functions such as loadData, to read the directories from a .txt file, and loadDirectory, to get the filepath from the file and reuse it to populate subcontents.
Constuctor: Creates a window, then loads the data for the root. And for the content panel, when a folder is double clicked it makes sure its a directory and matches the name along with ensuring that "current root" is consistent to poulate the sub directory. If the user clicks a file it should run a preview method to populate that panel.  
Added funtionality to display the metadata. It uses exiftools to get key value pairs for the metadata. Then it processes keys by mapping them to their categoies mentioned in the key itself, to make it more readable. The file explorer now also allows to display files/folders from different drives, using QStorageInfo to pick up the names and path. The final functionality in this window is to open a new edit window and an immediate refresh when changes are saved using a signal. 
```

### metadataeditor
```
This creates a new window and populates the metadata similar to the mainwindow. Each field comes with its own delete button and there is a universal add button. It uses dialogue boxes to add fields and value. There is a check in place to add only valid field, for example any gibberish field name or a non useful field name would be rejected using a check in ExifTools. It also supports saving changes or cancel updates.  
```

### .qrc

```
Communicates the external resources like data files, images, and in this case a placeholder gif.
```

### bugs
```
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

v.0.2.0
Updated main window and fix a few bugs.
Populated metadata by making a request to ExifTools.
Added a scroll bar

v.0.3.0
Made the metadata viewer more readable.
Added drives change functionality for the file explorer
Created a new window and populated it with text bar style value editors. 
With the help of AI worked on he save, add, edit, and delete functionalities. 

```
