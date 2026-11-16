# UI Tests

Automated tests that open the real QuadM windows and use them like a person would: typing in the address bar, clicking and double-clicking files and folders, pressing buttons. They are written with Qt Test (`QTest`), which comes with Qt, so nothing extra needs installing.

The tests are in [ui/tst_quadm_ui.cpp](ui/tst_quadm_ui.cpp). They cover the main window, the metadata editor and the settings window. Every test has an ID (`UI-01`, ...) in a comment above it. The same IDs are listed in [../../tests/test-cases.md](../../tests/test-cases.md).

## Run them

From the `code` folder:

```bash
make test            # macOS / Linux
mingw32-make test    # Windows (from a terminal where Qt's MinGW is on PATH, like for `make`)
```

That builds the test program (`QuadMUiTests`) and runs it. The first build takes a while because it compiles the app code a second time. Normal builds (`make`, `cmake --build`) do **not** build the tests.

Other ways to run them:

```bash
cmake --build --preset default --target QuadMUiTests   # build only (use `macos` instead of `default` on a Mac)
ctest --test-dir build --output-on-failure             # run through CTest
```

**Windows:** the tests open real windows for a few seconds. Don't click or type while they run, or they can fail.
**Linux/macOS via `ctest`:** runs headless (Qt's offscreen platform), so no display is needed.

## Reading the results

The program exits with 0 when everything passes. Results are also written to files in `build/ui-test-results/` (one file per window), for example:

```text
PASS   : MainWindowTests::doubleClickFolder_opensIt_backReturns()
FAIL!  : MainWindowTests::addressBar_folderPath_listsFolder() Compared values are not the same
   Actual   (...) : "C:/some/path"
   Expected (...) : "C:/other/path"
Totals: 18 passed, 0 failed, 0 skipped
```

Use `PASS` / `FAIL` / `SKIP` lines and the totals as evidence in `documentation/04-testing-report.md`.

## Good to know

- **ExifTool:** some tests read real metadata and need ExifTool in `third_party/exiftool` (see its README). Without it those tests are reported as `SKIP`, not failures.
- **Your files are safe:** tests only create and rename files inside a temporary folder that is deleted afterwards. Settings are written to a temporary file, not your real QuadM settings.
- **Message boxes:** the editor shows pop-ups ("saved successfully", "already exists"). The tests close them automatically and check their text.

## Adding a test

Add a function to the matching class in `tst_quadm_ui.cpp` under `private slots:`. Find widgets by the names in the `.ui` files, for example `window->findChild<QLineEdit *>("filePathSearchBar")`, then drive them with `QTest::mouseClick`, `QTest::keyClicks` and check results with `QCOMPARE` / `QVERIFY`. Copy an existing test as a starting point.
