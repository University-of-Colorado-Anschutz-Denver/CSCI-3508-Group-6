# Team Collaboration Log

Use this document throughout the project to demonstrate effective communication, balanced work, leadership, and each member's contribution. Entries should be factual and agreed upon by the team.

## Team Charter

**Project:** MultiMedia Metadata Manager (QuadM)

**Group leader:** Harrison Julius

**Communication channel(s):** Discord Server for general communication and planning, project meeting platform

**Regular meeting schedule:** Team meeting every Friday at 3pm (Discord), TA meeting Saturdays at 10am

**Expected response time:** 1.5 business days (36 hours)

**Decision method:** Consensus after discussion, leader selection if no agreement is reached

**Conflict resolution process:** 
1. Bring conflict up at team meeting, or schedule an additional meeting for discussion
2. Explain problem and each team member's perspective, lead by team leader
3. Attempt to reach consensus agreement on how to move forward by consolidating ideas through negotiation
4. If consensus cannot be reached, team leader will decide path forward

**Definition of done:** 
<ul><li>Work item has been reviewed by at least one team member and merged to main</li><li>Work item is traceable to at least one defined system requirement or diagram component</li><li>Work item has been tested by team and/or users for satisfaction</li></ul>

### Version Control Workflow

| Practice | Team agreement |
| --- | --- |
| Branch strategy | Create branches named `[branch-type]/[change-info]`. Branch types are feature, bugfix, and doc. Example branch name: `feature/xmp-integration` |
| Review rule | One review required per PR by any team member |
| Merge rule | Merge `feature` and `bugfix` first onto `testing` to test with other features, then onto `main` once issues are resolved and reviewed. `doc` branches can be merged directly onto `main` after review |
| Commit attribution | Committing should occur when any major isolated portion of a feature/bugfix is completed. Commit messages should be detailed enough to understand what it was at a glance, but not too long because we can just look at the code |
| Merge conflict process | Merge conflicts should be uncommon due to each person working on separate features/bugfixes; however, if one does occur involved devs should to communicate to get all functionality implemented before requesting review from another team member |

## Roles and Work Distribution

| Member | Role(s) | Planned responsibilities | Backup responsibility |
| --- | --- | --- | --- |
| Harrison Julius | Project Lead / Code Architect | <ul><li>Team management</li><li>GitHub management</li><li>Software engineering</li> | <ul><li>Research</li></ul> |
| Teja Kandimalla | GUI Designer | <ul><li>Design and implement user interface</li><li>Final say on GUI-related decisions</li><li>Software engineering</li> | <ul><li>WebDev Support</li></ul> |
| Em Kronquist | Research & Communication Head | <ul><li>Main contact for clients/testers</li><li>Documentation/research lead and final say</li><li>Software engineering</li></ul> | <ul><li>Programming/design support</li></ul> |
| Yannick Kuete | WebDev Lead | <ul><li>Website creation and management</li><li>Testing and automation</li><li>Software engineering</li></ul> | <ul><li>DevOps Management</li><li>Back-end engineering</li></ul> |

**Explain how the assignments are fair and how all members will participate in research, development, testing, writing, and presenting:** Each member is the "head" of a specific department and will be responsible for managing the rest of the team's contributions to their field. Each member will contribute to each area (for example, Em researched metadata formats and Teja researched GUI development for C++ applications, or Yannick created the website visuals and Harrison implemented the download function), and those contributions will be collated by the department head for the final project release.

## Milestones

| Milestone | Owner(s) | Target date | Completion criteria | Status |
| --- | --- | --- | --- | --- |
| Subject approval | All | 2026-09-21 | Approval recorded | DONE |
| Requirements baseline | All (EK Lead) | 2026-10-02 | Requirements reviewed | DONE |
| Design baseline | All (HJ Lead) | 2026-10-25 | Diagrams and schema reviewed | DONE |
| Implementation complete | All (HJ Lead) | 2026-11-15 | Required features work | DONE |
| Testing complete | All (YK/EK Lead) | 2026-11-22 | Exit criteria met | DONE |
| Report and presentation | All | 2026-11-18 | Final files reviewed | DONE |

## Meeting Log

| Date | Attendees | Topics and decisions | Assigned actions and owners | Due date |
| --- | --- | --- | --- | --- |
| 2026-09-11 | All | Project selection, proposal and presentation preparation | <ul><li>EK - Research on metadata tools</li><li>TK - Presentation slides</li><li>HJ - Project proposal documentation</li><li>YK - UML drafts</li></ul> | <ul><li>2026-09-17</li><li>2026-09-17</li><li>2026-09-17</li><li>2026-09-17</li></ul> |
| 2026-09-18 | HJ, TK, YK | Project proposal and presentation | <ul><li>All - Prepare selected presentation slides</li></ul> | <ul><li>2026-09-20</li></ul> |
| 2026-09-25 | All | Diagramming discussion | <ul><li>All - Prepare preliminary UML diagrams</li></ul> | <ul><li>2026-10-02</li></ul> |
| 2026-10-02 | All | Diagramming overview | <ul><li>HJ - Prepare finalized use-case and class diagrams based on preliminary versions</li></ul> | <ul><li>2026-10-03</li></ul> |
| 2026-10-09 | HJ, EK, TK | HacknPlan tasks review, tag format planning, GUI framework | <ul><li>TK - GUI research</li><li>EK - Research/test ExifTool integrations</li><li>All - Basic file explorer creation</li><li>YK - Website baseline creation</li></ul> | <ul><li>2026-10-16</li><li>2026-10-16</li><li>2026-10-16</li><li>2026-10-23</li></ul> |
| 2026-10-16 | HJ, EK, YK | ExifTool implementation management (provided interface vs calling directly) | <ul><li>HJ/EK - ExifTool metadata handlers implementation</li><li>TK - Frontend read metadata display</li></ul> | <ul><li>2026-10-23</li><li>2026-10-23</li></ul> |
| 2026-10-23 | HJ, TK, YK | Progress presentation prep | <ul><li>All - Presentation and submission prep</li></ul> | <ul><li>2026-10-25</li></ul> |
| 2026-10-24 | HJ, EK | TA Meeting #1 (Proposal feedback, UML discussions) | <ul><li>N/A</li></ul> | <ul><li>N/A</li></ul> |
| 2026-10-30 | HJ, EK, YK | V0.3.0 planning | <ul><li>EK - User testing for file explorer GUI</li><li>EK - Write user guides</li><li>HJ - Installers</li><li>TK - Frontend edit metadata display</li><li>YK - Website installer page prep</li></ul> | <ul><li>2026-11-06</li><li>2026-11-13</li><li>2026-11-06</li><li>2026-11-06</li><li>2026-11-06</li></li></ul> |
| 2026-11-06 | All | v1.0.0 and testing discussion | <ul><li>EK - Continue user testing</li><li>HJ - Mac Intel installer</li><li>HJ - UI test cases</li><li>TK - User feedback adjustments</li><li>YK - Website GUI </li></ul> | <ul><li>2026-11-11</li><li>2026-11-10</li><li>2026-11-10</li><li>2026-11-15</li><li>2026-11-10</li></li></ul> |

## Task and Contribution Log

| Date | Member | Contribution | Evidence/link | Reviewer | Status |
| --- | --- | --- | --- | --- | --- |
| 2026-09-11 | EK | Research on metadata types/standards; existing tools to edit metadata; libraries for metadata manipulation; basic materials for team to understand metadata | List of resources available in Discord | HJ | Accepted |
| 2026-09-14 - 2026-09-20 | All | Presentation slides prep and research | Presentation and 01-project-proposal.md submitted in Canvas | All | Accepted |
| 2026-09-14 - 2026-09-20 | All | Diagramming prototypes | Initial prototypes shared in Discord server, discussed in 2026-10-02 meeting, and finalized in submitted diagrams | All | Accepted |
| 2026-09-25 - 2026-10-02 | YK | CI/CD pipeline structuring | Implemented through GitHub Action | HJ | Accepted |
| 2026-10-16 | EK | Test implementation for ExifTool using ExifTool C++ Interface | Example files discussed in meeting (Interface not used in final version for various reasons, will be using custom implementation instead) | HJ | Accepted |
| 2026-10-23 - 2026-10-25 | All | Progress presentation prep | Materials submitted in Canvas | EK | Accepted |
| 2026-11-01 - 2026-11-11 | EK | User acceptance testing | Feedback presented to team, Kanban tasks adjusted/added, (04-testing-report.pdf)[/04-testing-report.pdf] | All | Accepted |
| 2026-11-01 - 2026-11-07 | All | Progress report video | Canvas submission | All | Done |
| 2026-11-12 - 2026-11-15 | All | Final report | Canvas submission | All | Done |

*Programming contributions are not listed here for brevity, as they are easily viewed in Git commits.*

## Decision Log

| Date | Decision | Options considered | Reason | Participants |
| --- | --- | --- | --- | --- |
| 2026-09-09, 2026-09-11 | Project selection (media metadata editor and search program, website for distribution and documentation) | <ul><li>Financial assistant and analyzer with chatbot integration</li><li>Family budget planner allowing multiple users and one year of budget planning</li><li>Metadata manager for media files [SELECTED]</li></ul> | Selected project had interested clients already; EK is familiar with the project domain; general team interest in subject matter | All |
| 2026-10-09 | GUI development toolkit | <ul><li>Qt [SELECTED]</li><li>FLTK</li><li>Dear ImGUI</li></ul> | Qt has it's own design studio and looks very polished, plus had easy to follow documentation and tutorials available | TK |
| 2026-10-17 | ExifTool implementation method | <ul><li>Custom calls to Perl implementation [SELECTED]</li><li>ExifTool C++ Interface</li></ul> | Interface required Windows users to install and run through Cygwin; creating custom implementation with Qt calls wasn't complicated and allowed more control | HJ, EK |

## Feedback and Communication Issues

| Date | Issue or feedback | Effect on project | Resolution and follow-up |
| --- | --- | --- | --- |
| 2026-09-09 | Misunderstanding of project domain | Slow start on developing project proposal and uncertain scope, as EK's explanation of problem to be solved was unclear | Problem was demonstrated in meeting on 2026-09-11 and made clear to team members, goals for development and scope adjusted to better reflect the solution desired |

## Final Contribution Summary

Complete this section before submission.

| Member | Research/requirements | Design | Code | Testing | Report/presentation | Overall contribution summary |
| --- | --- | --- | --- | --- | --- | --- |
| Harrison Julius | TagLib research | Architecture, class diagrams | Backend programming, final ExifTool and TagLib implementation | UI test cases | Progress and final report skeleton, backend demo for video | Project Management, Code Architect |
| Teja Kandimalla | GUI library research | Qt GUI choice, GUI layouts | Full GUI implementation | Manual GUI testing | GUI demo for video | GUI Designer |
| Em Kronquist | Metadata general info/project idea, stakeholder interviews, ExifTool research | Use case diagram | Web user guide, initial ExifTool strategy | User acceptance testing | Checkpoint video editing, progress presentation design | Research and Documentation, User Outreach |
| Yannick Kuete | Website requirements | Web UML diagrams | Full web implementation | GitHub Actions pipeline | Web demo for video | DevOps Management, WebDev |

*All team members meaningfully contributed to all reports and presentations, through writing, design, and actively presenting. Contributions listed here under the Report/Presentation section are tasks that went above the general tasks performed by all team members.*

**Team confirmation:** All team members reviewed and approved this document on 11/15/2026.
