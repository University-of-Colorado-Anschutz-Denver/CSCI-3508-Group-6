# System Design

## 1. Design Overview

**Project:** QuadM (MultiMedia Metadata Manager)

**Design goals:**
- Ease of use for non-technical users who aren't familiar with the command line interface
- Viewing and editing metadata for all common media file types in one application
- Allow easier search and sorting of media files

**Constraints:**
- Application must run on Windows, Mac, and Linux systems
- Additional file types beyond the initial ones may not be within project scope to support editing for

## 2. Architecture

### Architecture Style

We are using a layered architecture where components in a layer only depend on the layers below them. This allows for us to easily implement handlers for additional file types during development without requiring changes to other layers. 

### Architecture Diagram

**See [APP-02-architecture.png](design-assets/APP-02-architecture.png)**

| Layer | Component | Function | Interactions |
| ----- | ----- | ----- | ----- |
| User | User | \- | Interacts with all GUI functions to Create, Read, Update, and Delete Metadata |
| GUI | SearchPanel | Search via tag to display only files with that tag | Calls findByTag on MetadataService and Passes Results to FileBrowser |
| GUI | FileBrowser | Browse the filesystem and select a file | Interacts with SearchPanel to change what files are displayed.Opens MetadataEditor when selecting a file |
| GUI | MetadataEditor | CRUD Metadata Functions | Opened by FileBrowser, talks to API layer for data.Stores Metadata data class |
| API | MetadataService | Single entry point for reading, writing, and tag searches. | ExifTool for Reading, MetadataFilter to filter through redundant metadata tags, and selects a handler for writing |
| Backend | Metadata | Data Class for MetadataTracks which values were edited, so only changes are written | MetadataService fills in Metadata class using backend tools and gets passed to MetadataEditor |
| Backend | MetadataHandler \<\<interface\>\> | Interface for writing metadata | Implemented by both handlers |
| Backend | ExifToolHandler | Writing metadata to images, MP4/MOV and other formats ExifTool can write to. | Writes via ExifTool |
| Backend | TagLibHandler | Writing metadata to files MP3 and WAV | Writes Via TagLib |
| Backend | MetadataFilter | Metadata filter to remove unnecessary metadata from being displayed | Used by MetadataService to filter through metadata provided by ExifTool |
| Backend | ExifToolWrapper | Wraps ExifTool Executable Calls in C++ | N/A |
| External Tools & Libraries | TagLib | C++ library for editing metadata | Static Library used by TagLibHandler |
| External Tools & Libraries | ExifTool | Executable for editing metadata | Executable used by MetadataService for reading and findByTag, and ExifToolHandler for writing |
| Disk | Media Files | Metadata Storage | Written by ExifTool and TagLib, read by ExifTool |

**See [WEB-05-architecture-diagram.png](design-assets/WEB-05-architecture-diagram.png)**

**Explanation:** The website is a static front end with two external services behind it: GitHub for releases and a managed database for feedback. There is no server for the team to run. The diagram shows the layers and what calls what.

| Layer | Component | Responsibility |
| :---- | :---- | :---- |
| Presentation | Pages (index, download, guide, feedback, about) and style.css | Render content, navigation, light and dark theme, responsive layout. |
| Application | main.js: ReleaseService, FeedbackService, FormValidator | Look up the latest release, validate and submit the form, show status. |
| Delivery | GitHub Pages (CDN) | Serve the static files over HTTPS. |
| External services | GitHub Releases API, Feedback endpoint | Installer files and version data; receive and store feedback. |
| Storage | Feedback database | Permanent store for feedback records. |
| Automation | GitHub Actions: website.yml, release.yml | Lint and test the site on each push; build and publish installers on a version tag. |

## 3. UML Diagrams

### 3.1 Use Case Diagram

**See [APP-04-use-case.png](design-assets/APP-04-use-case.png)**

**Explanation:** The system has one actor, the App User, who uses the QuadM desktop application. The App User can search files, view metadata, edit existing metadata, and add new metadata. View metadata includes Search files, because a file has to be found before its metadata can be shown. Edit existing metadata and Add new metadata extend View metadata, because they are optional actions taken on a file the user is viewing.

**See [WEB-01-use-case-diagram.png](design-assets/WEB-01-use-case-diagram.png)**    

**Explanation:** Three actors use the site: a visitor, a returning user with the app installed, and a team member who reviews feedback. The highlighted use case is the one most visitors come for.

### 3.2 Class or Domain Model Diagram

**See [APP-01-class.png](design-assets/APP-01-class.png)**

**Explanation:** The classes are arranged in three layers: GUI, API, and Backend. The GUI classes only go through MetadataService and never touch the handlers or external tools. SearchPanel asks MetadataService to find the files carrying a metadata tag (findByTag) and passes the matching paths to FileBrowser, which then shows only those files. FileBrowser opens the selected file in MetadataEditor, and MetadataEditor talks only to MetadataService, the single entry point for reading, writing, and tag searches. MetadataEditor owns a Metadata object (composition) that holds the file's fields and tags and remembers the original values, so only the edited values are written back. MetadataService holds a list of MetadataHandler implementations (aggregation) and uses the first one whose canHandle accepts the file. TagLibHandler and ExifToolHandler both implement that interface, so supporting a new file format only requires adding a handler. All reading goes through the ExifTool utility, and MetadataFilter removes tags that shouldn't be shown.

**See [WEB-02-class-diagram.png](design-assets/WEB-02-class-diagram.png)**

**Explanation:** The browser code is split into small classes so each can be tested alone. ReleaseService and FeedbackService are the only two that talk to outside systems.

### 3.3 Sequence Diagram(s)

**See [WEB-03-sequence-download.png](design-assets/WEB-03-sequence-download.png)**

**Explanation:** The download page asks GitHub for the latest release at page load, then points the button at the asset URL. If the lookup fails, the button falls back to the Releases page.

**See [WEB-04-sequence-feedback.png](design-assets/WEB-04-sequence-feedback.png)**

**Explanation:** The form is validated in the browser, posted to the feedback endpoint, and written to the database. The visitor sees a status message either way.

## 4. Data and Schema Design

### Data Model or Schema Diagram

**See [APP-03-DFD.png](design-assets/APP-03-DFD.png)**

*Read: open a file and show its metadata*

| \# | From | To | What Happens |
| ----- | :---- | :---- | :---- |
| 1 | User | FileBrowser | Selects a file |
| 2 | FileBrowser | MetadataEditor | Opens the file in the editor |
| 3 | MetadataEditor | MetadataService | Calls readMetadata with the file path |
| 4 | MetadataService | ExifTool | Asks ExifTool to read the file; the result comes back as JSON |
| 5 | MetadataService | MetadataFilter | Removes tags that should not be shown |
| 6 | MetadataService | Metadata | Splits out the custom tags and loads the fields and tags into the Metadata object |
| 7 | MetadataEditor | User | Displays the fields and tags. If any step fails, the error message is shown instead. |

*Write: save edited metadata*

| \# | From | To | What Happens |
| ----- | :---- | :---- | :---- |
| 1 | User | MetadataEditor | Edits fields or adds/removes tags; the changes stay in the Metadata object (original values are kept) |
| 2 | User | MetadataEditor | Clicks Save |
| 3 | MetadataEditor | MetadataService | Calls writeMetadata with the file path and Metadata |
| 4 | MetadataService | MetadataService | Checks that every changed field is editable |
| 5 | MetadataService | MetadataHandler | Uses the first handler whose canHandle accepts the file |
| 6 | MetadataHandler | TagLib or ExifTool | TagLibHandler writes MP3/WAV through TagLib; ExifToolHandler writes all other supported formats through ExifTool. Only the changed values are written. |
| 7 | TagLib / ExifTool | Media file | Updates the metadata stored in the file |
| 8 | MetadataService | Metadata | Marks the changes as saved |
| 9 | MetadataEditor | User | Shows success. If any step fails, the error message is shown and the changes remain unsaved. |

*Search by Tag: filter the current directory*

| \# | From | To | What Happens |
| ----- | :---- | :---- | :---- |
| 1 | User | SearchPanel | Enters a tag |
| 2 | SearchPanel | MetadataService | Calls findByTag with the current directory and the tag |
| 3 | MetadataService | ExifTool | Scans the folder for files that carry the tag |
| 4 | MetadataService | SearchPanel | Returns the paths of the matching files |
| 5 | SearchPanel | FileBrowser | Passes the matching paths on |
| 6 | FileBrowser | User | Shows only the matching files. If the search fails, the error message is shown instead. |

**See [WEB-06-data-model-diagram.png](design-assets/WEB-06-data-model-diagram.png)**

**Explanation:** There is one stored entity. A feedback record points to a release only by its version text, so the database never depends on GitHub being up. The website needs one small database, for feedback. Everything else it shows is either a static file or data that GitHub already holds, so the team does not copy or maintain it.

| Data | Where it lives | Why |
| :---- | :---- | :---- |
| Pages, styles, screenshots | Static files in code/website, versioned in Git | Changes are reviewed in pull requests and tested in CI. |
| Releases, installers, version number | GitHub Releases | Already produced by the release pipeline; the site only reads it. |
| Feedback records | Managed document database (Cloud Firestore), collection feedback | Permanent, free at this size, and needs no server of our own. |
| Media metadata and tags | In the user's own files (XMP or ID3v2), on the user's machine | Not a website concern; the website never receives user media or tags. |

| Field | Type | Rules |
| :---- | :---- | :---- |
| id | string | Generated by the database. |
| type | string | One of bug, feature, question, other. |
| message | string | Required, 1 to 2000 characters. |
| email | string, optional | Valid email format; used only to reply. |
| appVersion | string, optional | For example 0.1.0. |
| createdAt | timestamp | Set by the server, never by the browser. |
| status | string | new (default), read, resolved; changed only by the team. |

## 5. Interface Design

### User Interface

**See [web-mockup-1.jpg](design-assets/), [web-mockup-2.jpg](design-assets/web-mockup-1.jpg)**

**Explanation:** The user interface is presented in three panels: the far-left holds the user's root directory, the center panel shows the contents of the current directory, and the far-right displays metadata information for the selected file, if any. Later in development, the team added an Edit Metadata button to the top of the view panel, which opens a separate window for editing metadata fields and adding custom tags to reduce clutter in the GUI. The settings are also self-contained in a separate window, accessible through the top toolbar.

### Internal and External Interfaces

**App Interfaces**

| Interface (Boundary) | Between | What Crosses It | On Failure |
| :---- | :---- | :---- | :---- |
| GUI ↔ API | SearchPanel / MetadataEditor → MetadataService | Requests to read, write, or search by tag. Returns a Metadata object or a list of matching files. | Success/fail result plus a readable error message for the GUI to display |
| API ↔ Backend Handlers | MetadataService → MetadataHandler | "Can you handle this file?" and "write these changed values" | Write reports failure; the service passes the message up to the GUI |
| Backend ↔ ExifTool | MetadataService / ExifToolHandler → ExifTool executable | File path in, JSON out | Timeout or error output is turned into an error message |
| Backend ↔ TagLib | TagLibHandler → TagLib | Writing tags to MP3/WAV files | Failure is reported back to the handler |

**Web interfaces**

| Interface | From, to | Contract |
| :---- | :---- | :---- |
| Release lookup | ReleaseService, GitHub REST API | GET /repos/{owner}/{repo}/releases/latest returns the tag name and the asset download URLs (.exe, .AppImage). |
| Feedback submit | FeedbackService, feedback endpoint | POST with a JSON body (type, message, email?, version?); answers 2xx on success. |
| Download | Visitor's browser, GitHub release asset | A direct HTTPS link to the installer file. |
| Release publish | release.yml, GitHub Releases | A version tag vX.Y.Z triggers the Windows build and attaches the installer. |

