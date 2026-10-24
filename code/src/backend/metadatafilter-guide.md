# Changing which metadata is shown

The lists that decide what the user sees are at the top of [metadatafilter.cpp](metadatafilter.cpp). This guide explains what they do, how to find out what to put in them, and how to change them safely. No C++ knowledge is needed to edit them.

## 1. The three lists

```cpp
const QSet<QString> kIgnoredGroups = { ... };   // hide a whole group
const QSet<QString> kIgnoredTags   = { ... };   // hide single tags
const QSet<QString> kReadOnlyTags  = { ... };   // show, but the user can't edit
```

| List | What goes in it | Example line |
|---|---|---|
| `kIgnoredGroups` | The **group** only (the part before the colon). Everything in that group is hidden. | `"System",` |
| `kIgnoredTags` | The **full name** `Group:Tag`. Just that one tag is hidden. | `"File:Directory",` |
| `kReadOnlyTags` | The **full name** `Group:Tag`. Shown, but with no edit box. | `"PNG:ImageWidth",` |

A tag that is in none of the lists is **shown and editable**.

How a tag is named: ExifTool names every tag `Group:Tag`. The group says where in the file it lives. For example `PNG:Gamma` is the tag `Gamma` in the PNG group, and `ID3:Title` is the title stored in the ID3 section of an MP3 or WAV.

Two kinds of tags are handled automatically, so you don't need to list them:

- Computed tags, whose name starts with `Composite:` (for example `Composite:ImageSize`), are always read-only, because ExifTool calculates them and can't write them.
- The tags the user adds to a file (the keyword list in image and video files, and `ID3:QuadM_Tags` in MP3 and WAV) are removed from the field list on purpose. The GUI shows them separately as tags.

## 2. How to find out what to change

You can't know which tags a file has until you look. Run ExifTool on a sample file and read what comes out.

### Windows (PowerShell)

Open PowerShell in the `code` folder. Change `$file` to a real file, then paste:

```powershell
$exiftool = ".\third_party\exiftool\exiftool.exe"
$file     = "C:\test_files\door_slam.wav"        # <-- change this
$out      = "$HOME\Desktop\sample-metadata.txt"

$json = & $exiftool -json -G $file | ConvertFrom-Json
$json[0].PSObject.Properties |
    ForEach-Object { "{0} = {1}" -f $_.Name, $_.Value } |
    Out-File $out -Encoding utf8

notepad $out
```

### macOS or Linux (Terminal)

Open a Terminal in the `code` folder. Change `FILE`, then paste:

```bash
EXIFTOOL=./third_party/exiftool/exiftool
FILE="$HOME/test_files/door_slam.wav"            # <-- change this

$EXIFTOOL -G -s "$FILE" \
  | sed -E 's/^\[([^]]+)\] +([^ ]+) +: /\1:\2 = /' \
  > ~/Desktop/sample-metadata.txt

open ~/Desktop/sample-metadata.txt
```

(If your copy of ExifTool is installed another way, replace the path in the first line with `exiftool`.)

### What you get

A text file with one tag per line, written in the exact form the lists use:

```
ExifTool:ExifToolVersion = 13.59
File:FileName = door_slam.wav
File:Directory = C:/test_files
File:FileSize = 17 kB
File:FileAccessDate = 2026:10:23 18:44:46-06:00
RIFF:SampleRate = 8000
ID3:Title = Door Slam
...
```

Everything on the left of the `=` can be copied straight into a list.

> This file shows **everything** ExifTool found, including tags that are already hidden. To see only what the app shows now, run the app and look at the panel, then compare.

## 3. Decide what to do with each line

Go through the file and ask three questions for every tag:

1. **Would a user ever want to read this?** If it's noise, hide it. Typical noise includes:
   - `ExifTool:*`, `System:*` (information about the tool and the computer),
   - folder paths, permissions and "last accessed" times,
   - internal numbers and flags that mean nothing to a person.
2. **Can it be written?** If it's useful but part of the file's structure (width, height, bit depth, file size), put it in `kReadOnlyTags`.
3. **Otherwise** leave it out of all lists. It will be shown and editable.

Quick reference:

| The tag is... | Do this |
|---|---|
| Useless noise | Add to `kIgnoredTags`. If a whole group is noise, add the group to `kIgnoredGroups`. |
| Useful, but can't change | Add to `kReadOnlyTags` |
| Useful and changeable | Do nothing |

**Test on more than one file.** The tags differ a lot between file types. Run the sample on at least a photo (JPG), a PNG, an MP3, a WAV and an MP4, and combine what you learn.

## 4. Making the change

1. Open [metadatafilter.cpp](metadatafilter.cpp).
2. Add (or delete) a line in the right list. Each entry is text in double quotes, followed by a comma:

   ```cpp
   const QSet<QString> kIgnoredTags = {
       "SourceFile",
       "File:Directory",
       "RIFF:AvgBytesPerSec",     // <-- new line you added
   };
   ```
3. Build and run (see [CMake.md](../../CMake.md) or [CMake-macOS.md](../../CMake-macOS.md)).
4. Open a file of that type and check the panel. Did the tag disappear? Is it greyed out?

## 5. Things that go wrong

- **Nothing changed.** The name must match **exactly**, including capital letters: `File:FileName`, not `file:filename`. Copy it from the sample file instead of typing it.
- **The tag is still there, but with a different group.** The same information can appear in several groups, such as `EXIF:Artist` and `XMP:Artist`. Hide each one separately.
- **Hiding a whole group hid too much.** Remove the group from `kIgnoredGroups` and hide individual tags instead.
- **Build error after editing.** Look for a missing comma, a missing quote, or a line outside the `{ }` braces.
- **A tag shows as editable but saving fails.** ExifTool can't write that tag. Put it in `kReadOnlyTags`.
- **A hidden tag is not in the data at all.** Hidden tags are removed before the GUI gets the data, so the GUI can't show them, even in a debug view.

## 6. Keep the lists tidy

- Keep each list roughly grouped (File tags together, PNG tags together, and so on).
- Add a short comment after a line when the reason isn't obvious:

  ```cpp
  "File:FileAccessDate",     // changes every time the file is read
  ```
- If you hide something, write down in the team log which file type you saw it in, so someone can check later whether it should come back.
