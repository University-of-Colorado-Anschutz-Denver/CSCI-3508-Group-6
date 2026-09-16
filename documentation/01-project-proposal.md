# Project Subject Proposal

Complete this proposal and submit it for approval before beginning the full project.

## Submission Information

| Field | Response |
| --- | --- |
| Proposed project title | [Enter title] |
| Group leader | Harrison Julius |
| Team members | Harrison Julius, Teja Kandimalla, Em Kronquist, Yannick Kuete |
| Date submitted | [YYYY-MM-DD] |
| Instructor decision | [Pending / Approved / Revisions requested] |
| Approval date | [YYYY-MM-DD or N/A] |

## Proposed Subject

An executable that allows all CRUD (Create, Read, Update, Delete) on metadata values on viable file types. Has a file explorer that can sort by metadata and custom tag metadata in files. Also has a public website where users can download the executable (likely an installer) for their specific Operating System. Website may also include user guides and feedback section.

## Problem and Motivation

It is difficult to view and edit common metadata fields for media files, such as title, authors, and more, directly from a file explorer. There are also many hidden metadata fields for different types of media files. These hidden fields can be used to sort and tag files, but as-is there is to way to access them.

## Proposed Software System

Program:  View metadata for media files (pngs, mp4s, wavs, etc), and edit common metadata fields like title, author, and comments. Will run as a .exe on a user's desktop/laptop and act as a more focused file explorer that allows users to see all metadata files natively. Allows users to create metadata tags for files and search by tags through specific directories on their machine.

Targeted Users: Media editors (film, sound design, photographs, etc); people with large collections of images, music, video stored on their devices

As a secondary goal, we aim to create a website to allow users to download the application, provide user guides, and allow users to provide feedback on the program.

## Connection to Course Objectives

This Project Demonstrates most, if not all software engineering concepts. We must find the requirement for the product for exactly what users want. We must design the architecture we will go forth with. We will need to implement it in code. We will need to test it for bugs or feature requirements. Testing may also include stakeholder testing. GitHub is being used for version control. Harrison Julius will be the primary source for project management. We will likely need to change the functionality and scope as the project continues and we will know based on user testing.

## Initial Scope

### In Scope

- GUI Metadata Manager for editing media metadata files
- File Manager that allows for sorting via metadata
- Website to host the application installer, and possibly user guides/feedback system

### Out of Scope

- Possibly Out of Scope: Allowing adding Tag metadata to files that don't traditionally support meta data: .txts for example.
- Shared File Systems across devices - Requires a lot of server space that we do not have access to.

## Expected Technology

| Area | Proposed choice | Reason |
| --- | --- | --- |
| Programming language(s) | C++, JS | Primarily C++ for executable, Primarily Javascript for website |
| Framework(s) | React and Node.js | React Framework and Node.js for the website portion |
| Data storage | N/A | Since this is not a service and it uses the users files and file system, no need for a database. (If We do the Feedback portion of the website, that will need a database, likely SQL_Lite3 for ease of use |
| Testing tools | GitHub | Use Git/Github tools to test the code upon each push/pull request |
| Collaboration tools | Hacknplan, Github, Discord | Hacknplan as the Kanban (could change, just using it for familiarity), GitHub for code collaboration, and Discord as the hub for communication and meetings |

## Feasibility and Risks

| Risk | Likelihood | Impact | Initial response |
| --- | --- | --- | --- |
| [Enter risk] | [Low/Medium/High] | [Low/Medium/High] | [How will the team reduce it?] |

## Instructor Feedback and Approval

**Feedback:** [Record instructor feedback here.]

**Required revisions:** [List revisions or enter None.]

**Approval evidence:** [Enter the approval date and describe or link to the approval message.]
