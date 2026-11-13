# Test Cases and Execution Record

Use this file for manual cases and as an index for automated tests. Give each case a stable ID and link it to one or more requirements.

## Test Cycle

| Field | Value |
| --- | --- |
| Project | QuadM (media metadata viewer and editor) |
| Build/version/commit | `tests/UI-tests` branch, based on 271c241 |
| Environment | Windows 11, Qt 6.12.0 (MinGW 13.1), ExifTool 13.59 |
| Test date(s) | 2026-11-10 |
| Tester(s) | Automated (`make test`); run by Harrison Julius |

## Detailed Test Cases

Copy this subsection for each case.

### TC-01: [Enter Descriptive Test Name]

| Field | Details |
| --- | --- |
| Related requirement(s) | [FR-01, NFR-01] |
| Test type | [Unit/Integration/System/Acceptance/Performance/etc.] |
| Priority | [High/Medium/Low] |
| Preconditions | [Required state and data] |
| Test data | [Inputs, using safe non-sensitive values] |

**Steps:**

1. [Enter action.]
2. [Enter action.]
3. [Enter action.]

**Expected result:** [Enter one specific, observable result.]

**Actual result:** [Record what happened during execution.]

**Status:** [Not run / Pass / Fail / Blocked]

**Evidence:** [Enter screenshot, log, automated test, issue, or commit reference.]

**Notes/defect:** [Enter defect ID and details, or None.]

**Execution history:**

| Run date | Build/commit | Tester | Result | Evidence/defect |
| --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Identifier] | [Name] | [Pass/Fail/Blocked] | [Reference] |

---

### TC-02: [Enter Boundary, Invalid, or Failure Case]

| Field | Details |
| --- | --- |
| Related requirement(s) | [Enter ID(s)] |
| Test type | [Enter type] |
| Priority | [High/Medium/Low] |
| Preconditions | [Required state and data] |
| Test data | [Boundary, malformed, missing, or failure-triggering input] |

**Steps:**

1. [Enter action.]
2. [Enter action.]

**Expected result:** [Describe safe error handling and unchanged/expected system state.]

**Actual result:** [Record what happened during execution.]

**Status:** [Not run / Pass / Fail / Blocked]

**Evidence:** [Enter reference.]

**Notes/defect:** [Enter defect ID and details, or None.]

**Execution history:**

| Run date | Build/commit | Tester | Result | Evidence/defect |
| --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Identifier] | [Name] | [Pass/Fail/Blocked] | [Reference] |

## Automated Test Index

UI tests (Qt Test) in [`code/tests/ui/tst_quadm_ui.cpp`](../code/tests/ui/tst_quadm_ui.cpp). Run with `make test` from the `code` folder; see [`code/tests/README.md`](../code/tests/README.md). The "Related requirement(s)" column names the feature; replace it with FR-xx IDs once the requirements specification is filled in.

| Test ID | Test name (`Class::function`) | Related requirement(s) | What it verifies |
| --- | --- | --- | --- |
| UI-01 | `MainWindowTests::startup_showsDrivesAndPlaceholder` | Application start | Drive list is filled, file list and Back button are hidden, placeholder animation shows |
| UI-02 | `MainWindowTests::addressBar_folderPath_listsFolder` | Folder browsing | Typing a folder path and pressing Enter opens that folder |
| UI-03 | `MainWindowTests::fileList_foldersFirst_unsupportedFilesHidden` | Folder browsing, file support | Folders are listed before files; non-media files (.txt) are not shown |
| UI-04 | `MainWindowTests::doubleClickFolder_opensIt_backReturns` | Folder browsing | Double-clicking a folder opens it; Back appears and returns to the previous folder |
| UI-05 | `MainWindowTests::clickFile_showsMetadataPanel` | View metadata | Clicking a file shows its name, type and details in the metadata panel |
| UI-06 | `MainWindowTests::clickFile_withExifTool_showsMediaMetadata` | View metadata | ExifTool metadata (e.g. Image Width) appears for a clicked image |
| UI-07 | `MainWindowTests::addressBar_invalidPath_changesNothing` | Folder browsing (invalid input) | A path that does not exist leaves the current view unchanged |
| UI-08 | `MainWindowTests::addressBar_unsupportedFile_changesNothing` | File support (invalid input) | Entering the path of an unsupported file leaves the view unchanged |
| UI-09 | `MainWindowTests::addressBar_filePath_showsOnlyThatFileAndItsMetadata` | Folder browsing, view metadata | Entering a media file path selects it and shows its metadata |
| UI-10 | `MainWindowTests::addressBar_emptyText_changesNothing` | Folder browsing (boundary) | Pressing Enter on a blank address bar does nothing |
| UI-11 | `MainWindowTests::tagFilter_hidesNonMatchingFiles_keepsFolders_andClears` | Search / filter | Filter hides non-matching files, keeps folders, and clearing it shows everything again |
| UI-12 | `MainWindowTests::tagFilter_noMatch_hidesAllFiles` | Search / filter (no results) | A filter that matches nothing hides all files but keeps folders |
| UI-13 | `MainWindowTests::editButton_withoutSelectedFile_doesNothing` | Edit metadata (invalid state) | The edit button does nothing when no file is selected |
| UI-14 | `MainWindowTests::editButton_withSelectedFile_opensEditor` | Edit metadata | The edit button opens the metadata editor for the selected file |
| UI-15 | `MainWindowTests::settingsMenu_opensSettingsWindow` | Settings | Settings > Open Window opens the settings window |
| UI-16 | `MainWindowTests::rootFolderList_clickLoadsFolder` | Folder browsing | Clicking a folder in the root panel loads it |
| UI-20 | `MetadataEditorTests::opens_showsFileNameAndType` | Edit metadata | The editor shows the file name, type and window title |
| UI-21 | `MetadataEditorTests::missingFile_showsNotFound` | Edit metadata (failure) | A missing file shows "File not found." |
| UI-22 | `MetadataEditorTests::cancelButton_closesWindow` | Edit metadata | Cancel closes the editor |
| UI-23 | `MetadataEditorTests::fields_listed_withEditableFileName` | Edit metadata | Metadata fields are listed and the file name is editable |
| UI-24 | `MetadataEditorTests::readOnlyFields_haveDisabledDeleteButton` | Edit metadata | Read-only fields cannot be deleted |
| UI-25 | `MetadataEditorTests::deleteButton_removesRowFromEditor` | Edit metadata | Delete removes the field row from the editor |
| UI-26 | `MetadataEditorTests::save_withoutChanges_saysNoChanges` | Edit metadata (boundary) | Saving with no edits shows "No changes to save." and saves nothing |
| UI-27 | `MetadataEditorTests::save_renamedFile_renamesOnDisk` | Edit metadata | Changing the file name and saving renames the file and reports success |
| UI-28 | `MetadataEditorTests::save_renameToExistingName_isRejected` | Edit metadata (invalid input) | Renaming to a name that already exists is refused; both files stay unchanged |
| UI-30 | `SettingsTests::themeAndDriveChoices_available` | Settings | Theme (Dark/Light) and drive choices are offered |
| UI-31 | `SettingsTests::save_storesChoices_emitsSignal_andCloses` | Settings | Save stores the choice, notifies the main window, and closes |
| UI-32 | `SettingsTests::reopen_showsSavedTheme` | Settings | Reopening settings shows the previously saved theme |

## Execution Summary

Run on 2026-11-10 with `make test` (Windows 11, Qt 6.12.0, ExifTool present).

| Status | Count |
| --- | ---: |
| Pass | 28 |
| Fail | 0 |
| Blocked | 0 |
| Not run | 0 |
| **Total** | 28 |

**Summary and next actions:** All 28 automated UI tests pass. Not covered by automation: the right-click "Copy File Path" menu, the visual look of the Dark/Light themes, and the installers; test these manually. Transfer final conclusions to `../documentation/04-testing-report.md`.
