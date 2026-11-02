# Backend Guide for the GUI Developer

How to use the QuadM metadata backend from the GUI. No Qt GUI experience is assumed.

To build the project first, see [CMake.md](CMake.md) (Windows) or [CMake-macOS.md](CMake-macOS.md) and [ExifToolReadMeInstructions.md](third_party/exiftool/README.md)

## 1. Words you'll see

| Word                            | Plain meaning                                                                                                                                                      |
| ------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| **Field**                       | One piece of info about a file, like Title or Artist. Each field has a name and a value.                                                                           |
| **Name of a field** (the "key") | The text name, like `ID3:Title`. The letters before the colon say where it is stored. The program uses it internally. Eventually the user should see just "Title". |
| **Tag**                         | A short label the user puts on a file, like `forest_zone_1`, so they can search for it later. A file can have many tags. Tags are separate from fields.            |
| **Read-only**                   | The user may look at it but not change it (for example the file size).                                                                                             |
| **Save**                        | Writes the changes into the actual file on disk.                                                                                                                   |

## 2. What the backend does for you

You never deal with ExifTool or TagLib. You ask one object, the `MetadataService`, and it answers.

```
Your GUI code  ──►  MetadataService  ──►  reads/writes the real file
```

You only need these calls. First, the data types used (a **type** says what kind of value goes in a spot):

| Type | Plain meaning | Example |
|---|---|---|
| `QString` | A piece of text | `"C:/test_files/door_slam.wav"` |
| `QStringList` | A list of texts | `{"forest_zone_1", "sfx"}` |
| `QMap<QString, QString>` | A list of (name, value) pairs, both text | `"ID3:Title"` and `"Door Slam"` |
| `bool` | `true` or `false` | `true` |
| `Metadata` | The object that holds one file's info | `Metadata data;` |
| `MetadataService` | The object that does the work | `MetadataService service;` |

A parameter written with `&` (like `Metadata &data` or `QString &error`) is an **output**. You pass in an empty variable, and the call fills it in. A parameter marked `const QString &` is an **input**. You just give it a value, and the call won't change it.

Create these variables first:

```cpp
MetadataService service;   // create once, e.g. as a member of MainWindow
Metadata data;             // one per open file
QString error;             // filled in by the calls when something goes wrong
QStringList files;         // filled in by findByTag
```

### The calls

| Call | Parameters (type: meaning) | Returns | What happens |
|---|---|---|---|
| `service.readMetadata(path, data, error)` | `const QString &path`: full path of the file<br>`Metadata &data`: **output**, filled with the file's info<br>`QString &error`: **output**, the reason if it failed | `bool` | Reads a file into `data`. `true` if it worked, `false` if not. |
| `data.fileType()` | none | `QString` | The kind of file, such as `"WAV"` or `"PNG"`. |
| `data.fields()` | none | `QMap<QString, QString>` | The list of fields (name and value pairs). |
| `data.get(name)` | `const QString &name`: a field name, such as `"ID3:Title"` | `QString` | The value of one field. Empty text if the field doesn't exist. |
| `data.set(name, value)` | `const QString &name`: the field name<br>`const QString &value`: the new value. Empty text means "remove this field from the file" | nothing | Changes a field in memory only. The file is not touched yet. |
| `data.tags()` | none | `QStringList` | The list of tags on the file. |
| `data.addTag(tag)` | `const QString &tag`: the tag to add, such as `"forest_zone_1"` | `bool` | Adds a tag in memory only. `false` if the tag is invalid or already there. |
| `data.removeTag(tag)` | `const QString &tag`: the tag to remove | nothing | Removes a tag in memory only. |
| `data.hasChanges()` | none | `bool` | `true` if there is anything to save. Use it to enable or disable the Save button. |
| `service.writeMetadata(path, data, error)` | `const QString &path`: full path of the file<br>`Metadata &data`: the edited info (it resets itself after a successful save)<br>`QString &error`: **output**, the reason if it failed | `bool` | Saves the changes into the file. `false` and `error` filled in on failure. |
| `service.findByTag(folder, tag, recursive, files, error)` | `const QString &folder`: the folder to search<br>`const QString &tag`: the tag to look for<br>`bool recursive`: `true` to also search subfolders<br>`QStringList &files`: **output**, the full paths of the files found<br>`QString &error`: **output**, the reason if it failed | `bool` | Finds the files that have the tag. `false` if the search failed. |

Two helpers:

| Call | Parameters (type: meaning) | Returns | What happens |
|---|---|---|---|
| `MetadataService::isEditable(name)` | `const QString &name`: a field name | `bool` | `true` if the user may change that field. Use it to decide between a text box and plain text. Note it is written with the class name, not `service.`. |
| `service.canWrite(path)` | `const QString &path`: full path of the file | `bool` | `false` for file types that can't be saved at all (for example MKV). Make everything read-only for those. |

Example of a full call, so the types are clear:

```cpp
QString path = "C:/test_files/door_slam.wav";        // QString
bool worked = service.readMetadata(path, data, error); // returns a bool
if (!worked) {
    // error now holds the reason, for example "ExifTool was not found..."
}
```

Rules:

- Always check the `true`/`false` result and show `error` to the user.
- These calls can take a moment because they run another program (ExifTool). Don't call them hundreds of times in a loop.
- **Saving changes the file itself.** There is no backup and no undo. Test on copies of files.
- `addTag("Forest Zone 1")` stores `forest_zone_1`: lower case, spaces become `_`, odd symbols are removed.
- Tags are not part of `data.fields()`. Use `data.tags()`.

## 3. Test the backend (no GUI needed)

This temporary test prints text to the terminal (the black window), so you can see the backend working.

**Warning: the test changes the files it uses. Make copies of a `.wav` and a `.png` in a folder such as `C:\test_files` and use those.**

**Step 1.** Create `code/src/backendtest.cpp` and paste this in. Change the three paths at the top.

```cpp
#include "api/metadataservice.h"

#include <iostream>

// A small helper: prints a piece of text on its own line in the terminal
static void say(const QString &text)
{
    std::cout << text.toStdString() << std::endl;
}

void runBackendTest()
{
    // CHANGE THESE to copies of your own files
    QString wavFile = "C:/test_files/door_slam.wav";
    QString pngFile = "C:/test_files/screenshot.png";
    QString folder  = "C:/test_files";

    MetadataService service;   // the object we ask for everything
    QString error;             // if something goes wrong, the reason is put here
    Metadata data;             // this will hold the info about one file

    // ---- 1. READ the file ----
    bool worked = service.readMetadata(wavFile, data, error);
    if (!worked) {
        say("Could not read the file: " + error);
        return;   // stop the test
    }
    say("File type: " + data.fileType());

    // ---- 2. PRINT every field ----
    // data.fields() is a list of (name, value) pairs. This loop goes through the names one by one.
    QMap<QString, QString> fields = data.fields();
    QStringList names = fields.keys();

    for (int i = 0; i < names.size(); i++) {
        QString name = names[i];
        QString value = fields.value(name);

        QString label;
        if (MetadataService::isEditable(name))
            label = "[can edit]  ";
        else
            label = "[read-only] ";

        say(label + name + " = " + value);
    }

    // ---- 3. PRINT the tags ----
    say("Tags: " + data.tags().join(", "));

    // ---- 4. CHANGE things (only in memory, the file is not touched yet) ----
    data.set("ID3:Title", "Door Slam");
    data.addTag("Forest Zone 1");   // saved as "forest_zone_1"
    data.addTag("sfx");

    if (data.hasChanges())
        say("There are unsaved changes");

    // ---- 5. SAVE into the file ----
    worked = service.writeMetadata(wavFile, data, error);
    if (worked)
        say("Saved!");
    else
        say("Save failed: " + error);

    // ---- 6. READ AGAIN to prove it really changed ----
    Metadata again;
    service.readMetadata(wavFile, again, error);
    say("Title is now: " + again.get("ID3:Title"));
    say("Tags are now: " + again.tags().join(", "));

    // ---- 7. SEARCH a folder for a tag ----
    Metadata pngData;
    if (service.readMetadata(pngFile, pngData, error)) {
        pngData.addTag("forest_zone_1");
        service.writeMetadata(pngFile, pngData, error);
    }

    QStringList foundFiles;
    worked = service.findByTag(folder, "forest_zone_1", true, foundFiles, error);
    if (worked)
        say("Files with the tag forest_zone_1: " + foundFiles.join("  |  "));
    else
        say("Search failed: " + error);
}
```

**Step 2.** In `code/src/main.cpp` add two lines:

```cpp
#include "QuadMGUI/mainwindow.h"
void runBackendTest();          // <-- add this line below the includes

...
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    runBackendTest();           // <-- add this line right after QApplication a(...)
    ...
```

**Step 3.** Build and run from a terminal, so you can see the output:

```powershell
cd code
cmake --build --preset default
.\build\QuadM.exe
```

The text appears in the terminal, then the normal window opens.

**Step 4.** When you're done, delete `backendtest.cpp` and the two lines in `main.cpp`.

### What you should see

```
File type: WAV
[read-only] File:FileName = door_slam.wav
[can edit]  ID3:Title = ...
...
Tags:
There are unsaved changes
Saved!
Title is now: Door Slam
Tags are now: forest_zone_1, sfx
Files with the tag forest_zone_1: C:/test_files/door_slam.wav  |  C:/test_files/screenshot.png
```

If you see `Could not read the file: ...`, check that the path is right and the file exists.

### What the loop does

```cpp
for (int i = 0; i < names.size(); i++) {   // do this once for every field name in the list
    QString name = names[i];                // take the i-th name, e.g. "ID3:Title"
    QString value = fields.value(name);     // look up its value
    ...
}
```

It goes down the list one line at a time. The first time `i` is 0, the next time 1, and so on, until every field has been printed.

## 4. The metadata panel

This is a suggested layout for the metadata panel. The names in `( )` are the Qt class of each element. The names in `[ ]` are suggested `objectName`s. Set the object name in Qt Designer, and the code can then use it as `ui->metadataTableWidget`.

```
┌─ metadataPanel (QWidget) ───────────────────────────────────────────────┐
│  [metadataPanelLayout: QVBoxLayout, stacks everything top to bottom]    │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐    │
│  │ metadataHeadingLabel (QLabel)    "door_slam.wav"                │    │
│  └─────────────────────────────────────────────────────────────────┘    │
│                                                                         │
│  ┌─ metadataTableWidget (QTableWidget, 2 columns) ──────────────────┐   │
│  │  Name (column 0)          │ Value (column 1)                     │   │
│  ├───────────────────────────┼──────────────────────────────────────┤   │
│  │  File:FileName            │ door_slam.wav        (read-only)     │   │
│  │  File:FileSize            │ 17 kB                (read-only)     │   │
│  │  RIFF:SampleRate          │ 8000                 (read-only)     │   │
│  │  ID3:Title                │ Door Slam            (can edit)      │   │
│  │  ID3:Artist               │ Foley Studio         (can edit)      │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│   Each row is made of QTableWidgetItem objects, one per cell.           │
│                                                                         │
│  ┌─ tagsGroupBox (QGroupBox, title "Tags") ─────────────────────────┐   │
│  │  [tagsLayout: QVBoxLayout]                                       │   │
│  │  ┌─ tagsListWidget (QListWidget) ────────────────────────────┐   │   │
│  │  │  forest_zone_1                                            │   │   │
│  │  │  sfx                                                      │   │   │
│  │  └───────────────────────────────────────────────────────────┘   │   │
│  │  [tagInputLayout: QHBoxLayout, puts these three side by side]    │   │
│  │  ┌──────────────────────┐ ┌───────────┐ ┌──────────────┐        │   │
│  │  │ newTagLineEdit       │ │ addTag    │ │ removeTag    │        │   │
│  │  │ (QLineEdit)          │ │ Button    │ │ Button       │        │   │
│  │  │ type a tag here...   │ │(QPushBtn) │ │ (QPushBtn)   │        │   │
│  │  │                      │ │ "Add"     │ │ "Remove"     │        │   │
│  │  └──────────────────────┘ └───────────┘ └──────────────┘        │   │
│  └──────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  metadataStatusLabel (QLabel)   "Saved!"  or the error text             │
│                                                                         │
│  [buttonLayout: QHBoxLayout]                                            │
│        (spacer)        ┌──────────────┐  ┌──────────────┐               │
│                        │ revertButton │  │ saveButton   │               │
│                        │ (QPushButton)│  │ (QPushButton)│               │
│                        │  "Revert"    │  │  "Save"      │               │
│                        └──────────────┘  └──────────────┘               │
└─────────────────────────────────────────────────────────────────────────┘
```

### What each element does

| Element (Qt class) | Suggested name         | What it does                                                                                                                                                     | Backend call                                  |
| ------------------ | ---------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------- |
| `QLabel`           | `metadataHeadingLabel` | Shows the name of the selected file.                                                                                                                             | none                                          |
| `QTableWidget`     | `metadataTableWidget`  | Shows one row per field: name in column 0, value in column 1. Rows where `isEditable(name)` is `true` can be edited. The others are not editable and shown grey. | `data.fields()`, `isEditable()`, `data.set()` |
| `QListWidget`      | `tagsListWidget`       | Shows the file's tags, one per line.                                                                                                                             | `data.tags()`                                 |
| `QLineEdit`        | `newTagLineEdit`       | Where the user types a new tag.                                                                                                                                  | none                                          |
| `QPushButton`      | `addTagButton`         | Adds the typed tag to the list.                                                                                                                                  | `data.addTag(text)`                           |
| `QPushButton`      | `removeTagButton`      | Removes the selected tag.                                                                                                                                        | `data.removeTag(text)`                        |
| `QLabel`           | `metadataStatusLabel`  | Shows "Saved!" or an error message.                                                                                                                              | the `error` text                              |
| `QPushButton`      | `saveButton`           | Saves. Disabled while `data.hasChanges()` is `false`.                                                                                                            | `service.writeMetadata()`                     |
| `QPushButton`      | `revertButton`         | Throws away edits by reading the file again.                                                                                                                     | `service.readMetadata()`                      |

### When things happen

| Event                                                  | What to do                                                                                                        |
| ------------------------------------------------------ | ----------------------------------------------------------------------------------------------------------------- |
| User clicks a file in the file list                    | Call `readMetadata`, then fill the table and the tag list.                                                        |
| User edits a cell in the table                         | Call `data.set(name, newText)`, then update the Save button.                                                      |
| User clicks **Add** (or presses Enter in the text box) | Call `data.addTag(text)`. If it returns `false`, say so in `metadataStatusLabel`. Otherwise refresh the tag list. |
| User clicks **Remove**                                 | Call `data.removeTag(...)` for the selected tag, then refresh the tag list.                                       |
| User clicks **Save**                                   | Call `writeMetadata`. Show "Saved!" or the error.                                                                 |
| User clicks **Revert**                                 | Call `readMetadata` again and refill everything.                                                                  |

In Qt, "when X happens do Y" is written with `connect(...)`. The existing code in `mainwindow.cpp` already does this for the back button and the file lists, so use it as a pattern.

### Tips for building the table

- Each row needs two `QTableWidgetItem`s (one per column). To make one read-only, remove its edit permission:
  `item->setFlags(item->flags() & ~Qt::ItemIsEditable);`
- While you fill the table with code, the table may report "cell changed" for every cell you add. Turn that off while filling with `ui->metadataTableWidget->blockSignals(true);` and back on with `false` when done. Otherwise those fake edits count as changes.
- The existing `metadataPlaceholderLabel` (the cat GIF) in `MainWindow.ui` can be replaced by this panel.

### Search panel (separate from the metadata panel)

| Element (Qt class) | Suggested name            | What it does           | Backend call             |
| ------------------ | ------------------------- | ---------------------- | ------------------------ |
| `QLineEdit`        | `searchTagLineEdit`       | The tag to look for.   | none                     |
| `QPushButton`      | `searchButton`            | Starts the search.     | `service.findByTag(...)` |
| `QListWidget`      | `searchResultsListWidget` | Shows the files found. | the `files` list         |

## 5. Where the code is

```
src/api/metadataservice.*       the only class the GUI uses
src/backend/metadata.*          the Metadata object: fields, tags, and what was changed
src/backend/metadatahandler.h   the "interface": canHandle(path), write(path, data)
src/backend/taglibhandler.*     saves MP3 and WAV files
src/backend/exiftoolhandler.*   saves PNG, JPG, MP4 and other formats
src/backend/metadatafilter.*    which fields are hidden or read-only (edit the lists here)
src/exiftool.*                  runs ExifTool
```

Note: the file list in the window still shows fake entries from `testdata.txt`. To try the panel with a real file, call `readMetadata` with a real path until the file browser shows real files.
