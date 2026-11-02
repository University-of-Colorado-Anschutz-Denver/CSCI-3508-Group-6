# QuadM (MultiMedia Metadata Manager) - Progress Review

Team: Group 6 | Date: [YYYY-MM-DD] | Presentation: Oct 26 / 28 (7-8 min)

> Format notes for Google Docs: PDF should be technical and detailed. The slides reuse the same sections at a high level. Each section below is tagged with its rubric points.

## Rubric Map

| Rubric item                                   | Pts | Section |
| --------------------------------------------- | --- | ------- |
| UML Diagrams                                  | 8   | 1       |
| System Architecture                           | 6   | 2       |
| Database / Data Design                        | 4   | 3       |
| Design Consistency                            | 3   | 4       |
| Technology and Design Decisions               | 2   | 5       |
| Progress Review / Design Understanding        | 7   | 6, 7, 8 |
| Contribution record (required each milestone) | -   | 8       |

---

## 1. UML Diagrams (8)

Each diagram: figure number, caption, 2-4 sentence explanation. Names must match the code.

### Figure 1 - Use Case Diagram
[INSERT DIAGRAM]

Explanation: [actors, use cases, include/extend]

### Figure 2 - Class Diagram
[INSERT DIAGRAM]

Explanation: [key classes and relationships]

Classes in the diagram:

| Layer   | Class             | Stereotype | Notes                                             |
| ------- | ----------------- | ---------- | ------------------------------------------------- |
| GUI     | `SearchPanel`     |            | Search files by tag in a root directory           |
| GUI     | `FileBrowser`     |            | Navigate directories and select a file            |
| GUI     | `MetadataEditor`  |            | Edit fields and tags of one file; save / revert   |
| API     | `MetadataService` |            | Single entry point for the GUI                    |
| Backend | `Metadata`        |            | Fields, tags, change tracking                     |
| Backend | `MetadataHandler` | interface  | `canHandle()`, `write()`                          |
| Backend | `ExifToolHandler` |            | Writes everything else ExifTool supports          |
| Backend | `TagLibHandler`   |            | Writes MP3/WAV                                    |
| Backend | `MetadataFilter`  | utility    | `filterMetadata`, `isIgnoredTag`, `isReadOnlyTag` |
| Backend | `ExifTool`        | utility    | Runs the ExifTool process                         |
| Backend | `TagLib`          | external   | Third-party library; writes MP3/WAV tags          |

### Figure 3 - Sequence Diagram(s)
[INSERT DIAGRAM(S)]

Scenario(s) shown: [e.g. read metadata, edit and save, find by tag]

### Figure 4 - Additional (activity / state / component), optional
[INSERT DIAGRAM or delete]

---

## 2. System Architecture (6)

### Figure 5 - Architecture Diagram
[INSERT DIAGRAM]

| Component                                | Responsibility | Inputs | Outputs | Related requirements |
| ---------------------------------------- | -------------- | ------ | ------- | -------------------- |
| GUI (`MainWindow`)                       | [ ]            | [ ]    | [ ]     | [ ]                  |
| API layer (`MetadataService`)            | [ ]            | [ ]    | [ ]     | [ ]                  |
| Backend (`Metadata`, handlers, filter)   | [ ]            | [ ]    | [ ]     | [ ]                  |
| `ExifTool` wrapper / ExifTool executable | [ ]            | [ ]    | [ ]     | [ ]                  |
| TagLib (static library)                  | [ ]            | [ ]    | [ ]     | [ ]                  |
| Website (static HTML/CSS/JS)             | [ ]            | [ ]    | [ ]     | [ ]                  |

Architecture style: [layered / other] - why: [ ]

Interfaces between components:

| Interface                                                                       | Producer | Consumer | Data / contract     | Error handling            |
| ------------------------------------------------------------------------------- | -------- | -------- | ------------------- | ------------------------- |
| `MetadataService::readMetadata/writeMetadata/findByTag/scanFolderTags/canWrite` | [ ]      | [ ]      | [ ]                 | `bool` + `QString &error` |
| `MetadataHandler::canHandle/write`                                              | [ ]      | [ ]      | [ ]                 | [ ]                       |
| `ExifTool::readMetadata/writeTags/scanTags`                                     | [ ]      | [ ]      | JSON via `QProcess` | [ ]                       |

Interaction / control flow (read, write, search): [ ]

---

## 3. Database / Data Design (4)

Data-management approach: [no database - metadata stored inside the user's own files; explain]

Figure 6 - Data model / schema diagram: [INSERT DIAGRAM]

| Data object                         | Important fields and types                                                                                                      | Identifier | Constraints                                            |
| ----------------------------------- | ------------------------------------------------------------------------------------------------------------------------------- | ---------- | ------------------------------------------------------ |
| `Metadata`                          | fileType: QString; fields: QMap<QString,QString> ("Group:Tag" -> value); tags: QStringList; original copies for change tracking | file path  | [ ]                                                    |
| Tag (label)                         | normalized string                                                                                                               | -          | lower case, spaces -> `_`, only letters/digits/`_`/`-` |
| Tag storage in file                 | XMP `Subject` (images, video, etc.); ID3v2 custom frame `QuadM_Tags`, `;`-separated (MP3/WAV)                                   | -          | [ ]                                                    |
| Feedback storage (website, planned) | [ ]                                                                                                                             | [ ]        | [ ]                                                    |

Data integrity / privacy: [validation, read-only fields, no backup/undo on save, local only]

---

## 4. Design Consistency (3)

Requirements (from `02-requirements-specification.md`) vs. design:

| Requirement ID | Requirement (short)                     | Design element | Implementation | Status |
| -------------- | --------------------------------------- | -------------- | -------------- | ------ |
| FR-[ ]         | CRUD on metadata                        | [ ]            | [ ]            | [ ]    |
| FR-[ ]         | Sort/search by metadata and custom tags | [ ]            | [ ]            | [ ]    |
| FR-[ ]         | Website download / guide / feedback     | [ ]            | [ ]            | [ ]    |
| NFR-[ ]        | [ ]                                     | [ ]            | [ ]            | [ ]    |

Changes from the original proposal and why: [ ]

---

## 5. Technology and Design Decisions (2)

| Technology                          | Used for                                  | Justification                                |
| ----------------------------------- | ----------------------------------------- | -------------------------------------------- |
| C++17                               | Application                               | [ ]                                          |
| Qt 6 (Widgets)                      | GUI                                       | [ ]                                          |
| CMake (+ presets)                   | Build                                     | [ ]                                          |
| ExifTool (bundled)                  | Reading all formats; writing images/video | [ ]                                          |
| TagLib 2.0.2 (FetchContent, static) | Writing MP3/WAV                           | ExifTool cannot write them                   |
| HTML/CSS/JS (no build step)         | Website                                   | [proposal said React/Node - note any change] |
| GitHub / HacknPlan / Discord        | Collaboration                             | [ ]                                          |

| Decision                          | Alternatives | Chosen and why | Tradeoffs                              |
| --------------------------------- | ------------ | -------------- | -------------------------------------- |
| Handler list, first match wins    | [ ]          | [ ]            | [ ]                                    |
| Call ExifTool as external process | [ ]          | [ ]            | Slower per call; supports many formats |
| Tags kept separate from fields    | [ ]          | [ ]            | [ ]                                    |
| [ ]                               | [ ]          | [ ]            | [ ]                                    |

---

## 6. Implementation Status

| Feature                          | Status (Done / In progress / Planned) | Evidence (file / commit) |
| -------------------------------- | ------------------------------------- | ------------------------ |
| Backend read                     | [ ]                                   | [ ]                      |
| Backend write (ExifTool, TagLib) | [ ]                                   | [ ]                      |
| Tag add/remove and search        | [ ]                                   | [ ]                      |
| GUI file browser                 | [ ]                                   | [ ]                      |
| GUI connected to backend         | [ ]                                   | [ ]                      |
| Metadata editing UI              | [ ]                                   | [ ]                      |
| Website pages and tests          | [ ]                                   | [ ]                      |
| Installer                        | [ ]                                   | [ ]                      |
| Testing                          | [ ]                                   | [ ]                      |

Known issues / risks: [ ]

## 7. Next Development Steps

| Step | Owner | Target date |
| ---- | ----- | ----------- |
| [ ]  | [ ]   | [ ]         |

---

## 8. Contribution Record (since previous milestone)

| Member | Role   | Work completed since last milestone | Evidence (commit / file / PR) |
| ------ | ------ | ----------------------------------- | ----------------------------- |
| [Name] | [Role] | [ ]                                 | [ ]                           |
| [Name] | [Role] | [ ]                                 | [ ]                           |
| [Name] | [Role] | [ ]                                 | [ ]                           |
| [Name] | [Role] | [ ]                                 | [ ]                           |

---

## Presentation Checklist (7-8 min, high level)

- [ ] Same components as the PDF, light technical depth
- [ ] Updates on implementing the proposal
- [ ] Each member states their role and the work they did in it
- [ ] Be ready to explain any diagram, class, or decision shown
- [ ] Demo or screenshot of current progress
- [ ] Next steps

## Submission Checklist

- [ ] All figures numbered, readable, and consistent with code
- [ ] No leftover `[ ]` placeholders
- [ ] Exported to PDF and opens correctly
