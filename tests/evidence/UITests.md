********* Start testing of MainWindowTests *********
Config: Using QtTest library 6.12.0, Qt 6.12.0 (x86_64-little_endian-llp64 shared (dynamic) release build; by GCC 15.1.0), windows 11
PASS   : MainWindowTests::initTestCase()
PASS   : MainWindowTests::startup_showsDrivesAndPlaceholder()
PASS   : MainWindowTests::addressBar_folderPath_listsFolder()
PASS   : MainWindowTests::fileList_foldersFirst_unsupportedFilesHidden()
PASS   : MainWindowTests::doubleClickFolder_opensIt_backReturns()
QDEBUG : MainWindowTests::clickFile_showsMetadataPanel() File path: "C:/Users/Harrison Julius/AppData/Local/Temp/QuadMUiTests-xVWfqt/photo_alpha.png"
PASS   : MainWindowTests::clickFile_showsMetadataPanel()
QDEBUG : MainWindowTests::clickFile_withExifTool_showsMediaMetadata() File path: "C:/Users/Harrison Julius/AppData/Local/Temp/QuadMUiTests-xVWfqt/photo_alpha.png"
PASS   : MainWindowTests::clickFile_withExifTool_showsMediaMetadata()
QDEBUG : MainWindowTests::addressBar_invalidPath_changesNothing() Path does not exist: "C:/Users/Harrison Julius/AppData/Local/Temp/QuadMUiTests-xVWfqt/does-not-exist"
PASS   : MainWindowTests::addressBar_invalidPath_changesNothing()
QDEBUG : MainWindowTests::addressBar_unsupportedFile_changesNothing() Unsupported file type or invalid path: "C:/Users/Harrison Julius/AppData/Local/Temp/QuadMUiTests-xVWfqt/notes.txt"
PASS   : MainWindowTests::addressBar_unsupportedFile_changesNothing()
PASS   : MainWindowTests::addressBar_filePath_showsOnlyThatFileAndItsMetadata()
PASS   : MainWindowTests::addressBar_emptyText_changesNothing()
PASS   : MainWindowTests::tagFilter_hidesNonMatchingFiles_keepsFolders_andClears()
PASS   : MainWindowTests::tagFilter_noMatch_hidesAllFiles()
PASS   : MainWindowTests::editButton_withoutSelectedFile_doesNothing()
QDEBUG : MainWindowTests::editButton_withSelectedFile_opensEditor() File path: "C:/Users/Harrison Julius/AppData/Local/Temp/QuadMUiTests-xVWfqt/photo_alpha.png"
PASS   : MainWindowTests::editButton_withSelectedFile_opensEditor()
PASS   : MainWindowTests::settingsMenu_opensSettingsWindow()
PASS   : MainWindowTests::rootFolderList_clickLoadsFolder()
PASS   : MainWindowTests::cleanupTestCase()
Totals: 18 passed, 0 failed, 0 skipped, 0 blacklisted, 5252ms
********* Finished testing of MainWindowTests *********
********* Start testing of MetadataEditorTests *********
Config: Using QtTest library 6.12.0, Qt 6.12.0 (x86_64-little_endian-llp64 shared (dynamic) release build; by GCC 15.1.0), windows 11
PASS   : MetadataEditorTests::initTestCase()
PASS   : MetadataEditorTests::opens_showsFileNameAndType()
PASS   : MetadataEditorTests::missingFile_showsNotFound()
PASS   : MetadataEditorTests::cancelButton_closesWindow()
PASS   : MetadataEditorTests::fields_listed_withEditableFileName()
PASS   : MetadataEditorTests::readOnlyFields_haveDisabledDeleteButton()
PASS   : MetadataEditorTests::deleteButton_removesRowFromEditor()
PASS   : MetadataEditorTests::save_withoutChanges_saysNoChanges()
PASS   : MetadataEditorTests::save_renamedFile_renamesOnDisk()
PASS   : MetadataEditorTests::save_renameToExistingName_isRejected()
PASS   : MetadataEditorTests::cleanupTestCase()
Totals: 11 passed, 0 failed, 0 skipped, 0 blacklisted, 1675ms
********* Finished testing of MetadataEditorTests *********
********* Start testing of SettingsTests *********
Config: Using QtTest library 6.12.0, Qt 6.12.0 (x86_64-little_endian-llp64 shared (dynamic) release build; by GCC 15.1.0), windows 11
PASS   : SettingsTests::initTestCase()
PASS   : SettingsTests::themeAndDriveChoices_available()
PASS   : SettingsTests::save_storesChoices_emitsSignal_andCloses()
PASS   : SettingsTests::reopen_showsSavedTheme()
PASS   : SettingsTests::cleanupTestCase()
Totals: 5 passed, 0 failed, 0 skipped, 0 blacklisted, 461ms
********* Finished testing of SettingsTests *********