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

Use this format: "As a [type of user], I want [goal] so that [benefit]."

| ID | User story or use case | Priority | Acceptance criteria |
| --- | --- | --- | --- |
| US-01 | As a [user], I want [goal] so that [benefit]. | [Must/Should/Could] | Given [context], when [action], then [outcome]. |
| US-01 | As a media editor, I want to tag my files so that I can search for them easily. | Should | Given a folder on my device, when I search by a parameter (ie "forest:), then the application displays all files in the folder with the term "forest" in any metadata field. |
| US-02 | As a photographer, I want to view file metadata for images so that I can find information such as dimensions and camera type. | Must | Given a file opened with QuadM, when I select the file to view metadata, then I can see all metadata fields in an easy to understand layout. |
| US-03 | As a [user], I want [goal] so that [benefit]. | [Must/Should/Could] | Given [context], when [action], then [outcome]. |

## 6. Functional Requirements

Each requirement must describe one testable behavior. Use "shall" for required behavior.

| ID | Requirement | Source | Priority | Acceptance measure |
| --- | --- | --- | --- | --- |
| FR-01 | The system shall [perform a specific behavior]. | [Stakeholder/source] | [Must/Should/Could] | [Observable pass condition] |
| FR-01 | The system shall display all files and sub-folders within a specified directory. | [Stakeholder/source] | Must | [Observable pass condition] |

## 7. Non-Functional Requirements

Include relevant quality attributes such as performance, reliability, usability, security, accessibility, compatibility, scalability, and maintainability. Make each requirement measurable.

| ID | Quality attribute | Requirement | Measurement or target |
| --- | --- | --- | --- |
| NFR-01 | [Performance] | The system shall [measurable expectation]. | [Target and test method] |
| NFR-02 | [Reliability] | The system shall [measurable expectation]. | [Target and test method] |

## 8. External Interface Requirements

### User Interface

[Describe screens, accessibility expectations, navigation, and relevant mockups. Link or insert figures.]

### Software Interfaces

[Describe APIs, services, libraries, file formats, and integrations.]

### Hardware or Communication Interfaces

[Describe hardware, devices, networks, or enter Not applicable with an explanation.]

## 9. Data Requirements

[Describe data inputs, outputs, validation, retention, privacy, and important data rules.]

## 10. Requirements Traceability Matrix

This is the project's canonical end-to-end traceability record. Update the design, implementation, and test references as work progresses; other documents should link back here rather than create a conflicting source of truth.

| Requirement ID | Design component | Implementation location | Test case(s) | Status |
| --- | --- | --- | --- | --- |
| FR-01 | [Diagram/module] | [`../code/path`] | [TC-01] | [Planned/Complete/Verified] |
| NFR-01 | [Design decision] | [`../code/path`] | [TC-02] | [Planned/Complete/Verified] |

## 11. Requirements Change Log

| Date | Source | Change or open question | Affected IDs | Decision and reason | Approved by |
| --- | --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Stakeholder/feedback/test] | [Added, changed, removed, or unresolved item] | [FR-01] | [Decision/rationale] | [Name or Pending] |

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
