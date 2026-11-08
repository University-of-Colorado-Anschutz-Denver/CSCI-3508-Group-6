# Software Requirements Specification

## 1. Document Information

| Field | Value |
| --- | --- |
| Project | QuadM (MultiMedia Metadata Manager) |
| Version | v0.3.0 |
| Project Lead | Harrison Julius |
| Developers | Harrison Julius, Teja Kandimalla, Emelie Kronquist, Yannick Kuete |
| Document Authors | Emelie Kronquist |
| Last updated | 2026-11-01 |
| Status | DRAFT |

## 2. Purpose and Scope

**Purpose:** QuadM is an application that allows users to view and manage metadata for various file formats. Currently, metadata information is inconvenient to view and even more difficult to edit, often requiring a full re-export from editing software. QuadM aims to make this process easier by displaying all metadata in a single window and enabling users to edit the metadata in-application, no outside application necessary. It will also allow users to search for files via metadata information by creating custom tags for files.

**System scope:** The system QuadM encompasses the desktop application. This will interface with users, who can control the application. It will also interface with the third-party tool ExifTool and the user's file storage system. The user can naviagte the files stored on their device in a file explorer-like format, view all metadata for supported file types, edit that metadata in the application, add custom metadata tags, and search for files via file name and metadata information. The system does not currently include functionality to search for multiple qualifiers at once, search multiple directories at once, or batch edit files.

## 3. Stakeholders and Users

| Stakeholder or user group | Needs | How requirements were gathered |
| --- | --- | --- |
| Media editors | <ul><li>Better organization for media files stored on device</li><li>Improved searching functionality in a file explorer</li></ul> | Personal experience, research, user testing |
| Film/music/photo collectors | <ul><li>Search files by fields other than file name</li><li>Easily view metadata to determine hidden file information</li></ul> | Interview |

## 4. Requirements Gathering Process

**Methods used:**
- Interviews with stakeholders/personal experience
    - Asking about current tools
    - Pain points in searching and organizing
- Researching other metadata tools
    - Do tools support multiple file formats?
    - Custom tagging to support searching
    - GUI designs and ease of use
- User acceptance testing to update requirements *(testing reports in [04-testing-report.md](04-testing-report.md))*
    - Prototype 1: Wireframe UI
    - Prototype 2: Read and Edit Standard Metadata
    - v0.3.0: IN PROGRESS

**Interview participants and dates:**
| Participant | Role | Topic | Date |
| --- | --- | --- | --- |
| Helen Gover | Music supervisor | <ul><li>What metadata information would be useful</li><li>Difficulties in searching for applicable tags and fields</li></ul> | 2026-09-07 |

**Key findings (KF):**

1. Existing tools have difficult to use GUIs or are command-line only
2. Tools don't often support multiple media types (ex. only supports most image files, not images+music+video)
3. Organizing large amounts of files is extremely difficult, requiring long file names and/or deep folder structures

### Elicitation Evidence Index

| ID | Date | Method/source | Description | Location or citation | Related finding/requirement |
| --- | --- | --- | --- | --- | --- |
| EL-01 | 2026-09-11 | Research (Metadata management libraries) | Audio files | <li>[Mutagen](https://github.com/quodlibet/mutagen)</li> | [Finding, US-01, FR-01] |
| EL-02 | 2026-09-11 | Research (Metadata management libraries) | XMP metadata type | <li>[XMP on Wikipedia](https://en.wikipedia.org/wiki/Extensible_Metadata_Platform)</li><li>[XMP SDK and Specifications from Adobe](https://developer.adobe.com/xmp/docs/)</li> | [Finding, US-01, FR-01] |
| EL-03 | 2026-09-11 | Research (General metadata information) | Metadata basics | <li>[Metadata on Wikipedia](https://en.wikipedia.org/wiki/Metadata)</li> | [Finding, US-01, FR-01] |
| EL-04 | 2026-09-11 | Research (Metadata management libraries) | Reading and writing metadata | <li>[ExifTool by Phil Harvey](https://exiftool.org)</li> | KF-01,  |
| EL-05 | 2026-10-02 | Research (Metadata management libraries) | Image files | <li>[Exiv2](https://exiv2.org/index.html)</li> | [Finding, US-01, FR-01] |

**Assumptions and constraints:**

- The application must run on desktop/laptop computers
- The metadata editing must contain checks to prevent corrupting files
- The custom metadata must be readable, if not writeable, by other applications so it is not hidden outside of our application

## 5. User Stories or Use Cases

| ID | User story or use case | Priority | Acceptance criteria |
| --- | --- | --- | --- |
| US-01 | As a media editor, I want to tag my files so that I can search for them easily. | Should | Given a folder on my device, when I search by a parameter (ie "forest:), then the application displays all files in the folder with the term "forest" in any metadata field. |
| US-02 | As a photographer, I want to view file metadata for images so that I can find information such as dimensions and camera type. | Must | Given a file opened with QuadM, when I select the file to view metadata, then I can see all metadata fields in an easy to understand layout. |
| US-03 | As a non-technical user, I want the application to require no additional downloads or windows so that I can install and use it easily. | Should | While using the QuadM application, when attempting to view and edit metadata, the processes should all run in-application and be updated in viewing windows automatically. |

## 6. Functional Requirements

| ID | Requirement | Source | Priority | Acceptance measure |
| --- | --- | --- | --- | --- |
| FR-01 | The system shall display all supported files and sub-folders within a specified directory. | US-02 | Must | When opening a folder, verify that all sub-folders and files of supported extensions are visible in the content panel. |
| FR-02 | The system shall include all necessary files in a single download package. | US-03, FR-01 | Should | Downloading the installer via the website should provide one executable that downloads all files. |
| FR-03 | The system shall make all metadata information for a valid file viewable in a single window. | US-03, FR-01 | Must | The information displayed for a file in the metadata panel should match the fields returned by ExifTool as well as blank fields. |
| FR-04 | The system shall allow users to edit valid metadata fields. | US-01 | Must | Upon saving a change to metadata in the editor window, both ExifTool and the QuadM metadata panel should display the updated information. |
| FR-05 | The system shall add custom tags to valid files. | US-01, FR-03 | Must | When adding a metadata tag with a valid name in the edit window and hitting save, the new tag should be displayed in both ExifTool and QuadM. |
| FR-06 | The system shall search for files via a single, one-to-one user text entry in all metadata fields. | US-01, US-02 | Must | When a user enters a term in the search bar, the system will display all files in the current directory with the input present in any metadata field. |
| FR-07 | The system shall search for files via multiple separate user parameters, displaying all files with all matching parameters in any metadata field. | FR-03, US-01 | Could | When entering multiple search parameters, the system will parse each separately, then display all files in the current directory that have matches for all inputs in any metadata field. |
| FR-08 | The system shall support the most common media file types (.wav, .mp3, .mp4, .mov, .png, .jpg). | US-01, US-02 | Must | Viewing and editing metadata is functional for the specified file types. |
| FR-09 | The system shall support the all file types readable by ExifTool. | FR-02, US-01, US-02 | Could | Viewing and editing metadata is functional for the full list of files readable by Exiftool, available at https://exiftool.org. |

## 7. Non-Functional Requirements

| ID | Quality attribute | Requirement | Measurement or target |
| --- | --- | --- | --- |
| NFR-01 | Performance | The system shall search a folder less than 1gb in 5 seconds or less. | Manual testing |
| NFR-02 | Usability | The system shall allow non-technical users to easily view metadata information. | User acceptance testing |
| NFR-03 | Compatibility | The system shall store metadata within the file so users can access the tags on other devices. | Manual testing |

## 8. External Interface Requirements

### User Interface

- Main window: Has three sections. Far left displays root directory of drive, center displays contents of current directory, and right shows metadata information.
- Edit window: Displays all metadata fields with text entry boxes to edit values, and a save button to confirm changes. Displays a popup confirming success or the relevant error.
- Settings window: Accessible through the top menu bar, which allows users to edit settings such as default search directory, font, and text size.
[Describe screens, accessibility expectations, navigation, and relevant mockups. Link or insert figures.]

### Software Interfaces

- ExifTool: Used to read metadata for all supported file types and write to non-audio files.
- TagLib: Used to write ID3 metadata, used in audio files.
- XMP: Metadata format that allows custom tags.
- ID3: Metadata format used in audio files that defines standard and custom fields.

### Hardware or Communication Interfaces

Not Applicable: No network connections or external device requirements. Uses local file storage and directly changes file values, so no additional files are created.

## 9. Data Requirements

- Metadata will be stored within the file directly to allow users to transfer data and tagging systems between devices.
- Initial supported formats will be common audio, video, and image formats (.wav, .mp3, .mp4, .mov, .png, .jpg), with plans to expand to more formats if time allows.
- Un-editable metadata fields will be greyed out in the UI to prevent erroneous input and file corruption. Un-editable fields vary for each file type, typically being fields that are calculated by the device based on file information.
- The application should use ExifTool to read metadata for all file types to keep display consistent.
- The application should use TagLib to edit tags on file types that ExifTool cannot write to.

## 10. Requirements Traceability Matrix

This is the project's canonical end-to-end traceability record. Update the design, implementation, and test references as work progresses; other documents should link back here rather than create a conflicting source of truth.

| Requirement ID | Design component | Implementation location | Test case(s) or method | Status |
| --- | --- | --- | --- | --- |
| FR-01 | [./design-assets/web-mockup-1](Web Mockup 1), [./design-assets/web-mockup-2](Web Mockup 2) | [`../code/src/QuadMGUI/mainwindow.cpp`] | [TC-01] | Verified |
| FR-02 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-03 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-04 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-05 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-06 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-07 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-08 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| FR-09 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| NFR-01 | [Design decision] | [`../code/path`] | [TC-02] | [Planned/Complete/Verified] |
| NFR-02 | [Design decision] | [`../code/path`] | [TC-02] | [Planned/Complete/Verified] |
| NFR-03 | [Design decision] | [`../code/path`] | [TC-02] | [Planned/Complete/Verified] |

## 11. Requirements Change Log

| Date | Source | Change or open question | Affected IDs | Decision and reason | Approved by |
| --- | --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Stakeholder/feedback/test] | [Added, changed, removed, or unresolved item] | [FR-01] | [Decision/rationale] | [Name or Pending] |
| 2026-09-11 | Meeting | Base requirements added | FR-01, FR-03, FR-04, FR-05, FR-06, FR-08, NFR-02 | Brainstorming base requirements | HJ |
| 2026-10-02 | Research | Added requirement for supporting more file types | FR-09 | ExifTool and TagLib decided for handling metadata, which both handle multiple types of files similarly | HJ, EK |
| 2026-10-25 | Research | Added installer requirement | FR-02 | Likely difficulties for users needing to install separate tools to make QuadM function | All |
| 2026-10-28 | v0.3.0 user acceptance test | Added requests for searching via multiple inputs | [FR-01] | [Decision/rationale] | [Name or Pending] |

*Changes initially logged in a separate document, transferred here on 2026-11-07*

## 12. Review and Approval

| Reviewer | Role | Review date | Result or comments |
| --- | --- | --- | --- |
| [Enter name] | [Stakeholder/team role] | [YYYY-MM-DD] | [Approved or changes requested] |

### Requirements Quality Checklist

- [ ] Every requirement has a unique ID.
- [ ] Requirements are clear, feasible, necessary, and testable.
- [ ] Acceptance criteria are measurable.
- [ ] Functional and non-functional requirements are both included.
- [ ] Conflicts, assumptions, constraints, and scope exclusions are documented.
- [ ] Every implemented requirement maps to design, code, and one or more tests.
