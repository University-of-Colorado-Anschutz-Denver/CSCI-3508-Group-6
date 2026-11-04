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

**System scope:** [Describe the system boundary, major goals, and what is excluded.]
The system QuadM encompasses the desktop application. This will interface with users, who can control the application. It will also interface with the third-party tool ExifTool and the user's file storage system. The user can naviagte the files stored on their device in a file explorer-like format, view all metadata for supported file types, edit that metadata in the application, add custom metadata tags, and search for files via file name and metadata information. The system does not include functionality to search for multiple qualifiers at once.

## 3. Stakeholders and Users

| Stakeholder or user group | Needs | How requirements were gathered |
| --- | --- | --- |
| Media editors | <ul><li>Better organization for media files stored on device</li><li>Improved searching functionality in a file explorer</li></ul> | Interview, personal experience, research |
| Film/music/photo collectors | <ul><li>Search files by fields other than file name</li><li>Easily view metadata to determine hidden file information</li></ul> | Interview |

## 4. Requirements Gathering Process

**Methods used:** [Describe interviews, surveys, brainstorming, document analysis, prototypes, or other methods.]
<ul>
    <li></li>
</ul>

**Interview participants and dates:**
| Participant | Role | Date |
| --- | --- | --- |
| Helen Gover | Music supervisor; what metadata information would be useful | 2026-09-07 |

**Key findings:**

- [Finding]
- [Finding]
- [Finding]

### Elicitation Evidence Index

Preserve non-sensitive evidence such as anonymized interview notes, survey summaries, observation notes, prototype feedback, or research references.

| Evidence ID | Date | Method/source | Participants | Location or citation | Related finding/requirement |
| --- | --- | --- | --- | --- | --- |
| EL-01 | [YYYY-MM-DD] | [Method/source] | [Roles or anonymized group] | [Relative link/citation] | [Finding, US-01, FR-01] |

**Assumptions and constraints:**

- [Assumption or constraint]
- [Assumption or constraint]

## 5. User Stories or Use Cases

Use this format: "As a [type of user], I want [goal] so that [benefit]."

| ID | User story or use case | Priority | Acceptance criteria |
| --- | --- | --- | --- |
| US-01 | As a [user], I want [goal] so that [benefit]. | [Must/Should/Could] | Given [context], when [action], then [outcome]. |
| US-02 | [Enter story] | [Priority] | [Enter measurable criteria] |

## 6. Functional Requirements

Each requirement must describe one testable behavior. Use "shall" for required behavior.

| ID | Requirement | Source | Priority | Acceptance measure |
| --- | --- | --- | --- | --- |
| FR-01 | The system shall [perform a specific behavior]. | [Stakeholder/source] | [Must/Should/Could] | [Observable pass condition] |
| FR-02 | The system shall [perform a specific behavior]. | [Stakeholder/source] | [Must/Should/Could] | [Observable pass condition] |

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
