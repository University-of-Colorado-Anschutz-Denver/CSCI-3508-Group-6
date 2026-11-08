# System Design

## 1. Design Overview

**Project:** QuadM (MultiMedia Metadata Manager)

**Design goals:**
- Ease of use for non-technical users who aren't familiar with the command line interface
- Viewing and editing metadata for all common media file types in one application
- Allow easier search and sorting of media files

**Constraints:** [List technical, schedule, platform, security, or course constraints.]
- Application must run on Windows, Mac, and Linux systems
- Additional file types beyond the initial ones may not be within project scope to support editing for

Store editable diagram sources and readable exported images in `design-assets/`. Use relative links so diagrams display from a fresh repository copy.

## 2. Architecture

### Architecture Style

We are using a layered architecture where components in a layer only depend on the layers below them. This allows for us to easily implement handlers for additional file types during development without requiring changes to other layers. 

### Architecture Diagram

> [Insert a labeled architecture or component diagram here. Show major components and connections.]

**Diagram explanation:** [Explain each component, its responsibility, and how data or control moves through the system.]

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

Interfaces Between Layers

| Interface (Boundary) | Between | What Crosses It | On Failure |
| :---- | :---- | :---- | :---- |
| GUI ↔ API | SearchPanel / MetadataEditor → MetadataService | Requests to read, write, or search by tag. Returns a Metadata object or a list of matching files. | Success/fail result plus a readable error message for the GUI to display |
| API ↔ Backend Handlers | MetadataService → MetadataHandler | "Can you handle this file?" and "write these changed values" | Write reports failure; the service passes the message up to the GUI |
| Backend ↔ ExifTool | MetadataService / ExifToolHandler → ExifTool executable | File path in, JSON out | Timeout or error output is turned into an error message |
| Backend ↔ TagLib | TagLibHandler → TagLib | Writing tags to MP3/WAV files | Failure is reported back to the handler |

Interaction Flows

**Read: open a file and show its metadata**

| \# | From | To | What Happens |
| ----- | :---- | :---- | :---- |
| 1 | User | FileBrowser | Selects a file |
| 2 | FileBrowser | MetadataEditor | Opens the file in the editor |
| 3 | MetadataEditor | MetadataService | Calls readMetadata with the file path |
| 4 | MetadataService | ExifTool | Asks ExifTool to read the file; the result comes back as JSON |
| 5 | MetadataService | MetadataFilter | Removes tags that should not be shown |
| 6 | MetadataService | Metadata | Splits out the custom tags and loads the fields and tags into the Metadata object |
| 7 | MetadataEditor | User | Displays the fields and tags. If any step fails, the error message is shown instead. |

**Write: save edited metadata**

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

**Search by Tag: filter the current directory**

| \# | From | To | What Happens |
| ----- | :---- | :---- | :---- |
| 1 | User | SearchPanel | Enters a tag |
| 2 | SearchPanel | MetadataService | Calls findByTag with the current directory and the tag |
| 3 | MetadataService | ExifTool | Scans the folder for files that carry the tag |
| 4 | MetadataService | SearchPanel | Returns the paths of the matching files |
| 5 | SearchPanel | FileBrowser | Passes the matching paths on |
| 6 | FileBrowser | User | Shows only the matching files. If the search fails, the error message is shown instead. |

| Component | Responsibility | Inputs | Outputs | Related requirements |
| --- | --- | --- | --- | --- |
| [Component name] | [What it does] | [Inputs] | [Outputs] | [FR-01, NFR-01] |

## 3. UML Diagrams

Use the UML diagram types that accurately communicate your system. At minimum, include diagrams needed to explain user interactions, static structure, and important behavior. Label every diagram and keep it consistent with the code.

### 3.1 Use Case Diagram

> [Insert a use case diagram showing actors, system boundary, and major use cases.]

**Explanation:** [Describe actors, use cases, and important include/extend relationships.]

### 3.2 Class or Domain Model Diagram

> [Insert a class or domain model showing names, key attributes/operations, relationships, and multiplicities.]

**Explanation:** [Describe the most important classes/entities and relationships.]

### 3.3 Sequence Diagram(s)

> [Insert at least one sequence diagram for a major scenario or complex interaction.]

**Scenario:** [Name the use case and describe the successful or alternate flow shown.]

### 3.4 Additional Diagram(s)

> [Insert an activity, state, deployment, package, or other diagram if it helps explain the system. Otherwise explain why no additional diagram is necessary.]

## 4. Data and Schema Design

### Data Model or Schema Diagram

> [Insert an entity-relationship diagram, database schema, file schema, or object model. If the project stores no data, explain why this section is not applicable.]

| Entity/table/object | Important fields and types | Key or identifier | Relationships and constraints |
| --- | --- | --- | --- |
| [Name] | [field: type] | [Primary key/ID] | [Relationships, validation, uniqueness] |

**Data integrity and privacy:** [Explain validation, error prevention, sensitive data handling, and retention where relevant.]

## 5. Interface Design

### User Interface

> [Insert wireframes, mockups, navigation map, or screenshots with figure numbers.]

[Explain usability, accessibility, consistency, responsive behavior, and how the interface supports requirements.]

### Internal and External Interfaces

| Interface | Producer | Consumer | Data/contract | Error handling |
| --- | --- | --- | --- | --- |
| [API/function/file/event] | [Component] | [Component/user] | [Inputs and outputs] | [Failure behavior] |

## 6. Key Algorithms and Logic

[Describe important algorithms, workflows, validation rules, or state transitions. Use pseudocode only where it improves clarity.]

## 7. Security and Reliability

[Explain relevant authentication, authorization, input validation, error handling, backups, fault tolerance, logging, or recovery decisions. Mark non-applicable items and explain why.]

## 8. Design Decisions and Alternatives

| Decision | Alternatives considered | Chosen approach and reason | Tradeoffs |
| --- | --- | --- | --- |
| [Enter decision] | [Enter alternatives] | [Explain choice] | [Costs and limitations] |

## 9. Requirements-to-Design Mapping

Keep the full traceability record in `02-requirements-specification.md`. Use this table only to explain design-specific details.

| Requirement ID(s) | Design element | How the design satisfies the requirement |
| --- | --- | --- |
| [FR-01, NFR-01] | [Component/diagram/schema] | [Explanation] |

## 10. Design Review Checklist

- [ ] Architecture, UML diagrams, and schema are complete and readable.
- [ ] Diagram notation, labels, directions, and multiplicities are accurate.
- [ ] Every major requirement is represented in the design.
- [ ] Diagram names and relationships match the implementation.
- [ ] Interfaces, data validation, errors, and important quality attributes are addressed.
- [ ] Design choices and tradeoffs are justified.
