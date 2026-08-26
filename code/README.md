# Source Code

Place all runnable application source code in this folder. You may create language-appropriate subfolders, but keep the structure simple and document it below. Do not place tests here; use `../tests/`.

## Implementation Summary

**Implemented system:** [Summarize what the software does.]

**Implemented requirements:** [List requirement IDs, such as FR-01 through FR-08.]

**Known limitations:** [List limitations or enter None known.]

## Technology Stack

| Technology | Version | Purpose | Selection reason |
| --- | --- | --- | --- |
| [Language/framework/tool] | [Version] | [Purpose] | [Reason] |

## Source Structure

Update this example to match the project.

```text
code/
|-- [entry-point-or-module]/
|-- [configuration-file]
|-- [dependency-file]
`-- README.md
```

| Path | Responsibility | Related requirements |
| --- | --- | --- |
| `[relative/path]` | [What this file/module does] | [FR-01, NFR-01] |

## Prerequisites

- [Required runtime/tool and exact supported version]
- [Required service or platform]
- [Other dependency]

## Setup

Starting from a fresh clone:

1. [Enter setup step.]
2. [Enter dependency installation command.]
3. Copy `[example configuration]` to `[local configuration]` and enter local values. Never commit secrets.
4. [Enter database or data setup step, or remove if not applicable.]

## Run the Application

```text
[Enter exact run command]
```

Expected result: [Describe how to confirm that the application started successfully.]

## Configuration

| Setting | Required | Example or allowed values | Purpose |
| --- | --- | --- | --- |
| `[SETTING_NAME]` | [Yes/No] | `[safe example]` | [Purpose] |

## Implementation Evidence

| Requirement ID | Feature | Main source location | How to demonstrate it |
| --- | --- | --- | --- |
| FR-01 | [Feature] | `[relative/path]` | [Steps and expected result] |

## Error Handling and Quality

[Describe input validation, exceptions/errors, logging, security checks, coding conventions, reviews, and any static analysis or formatting tools used.]

## Run Tests

Tests are stored in `../tests/`. Run them with:

```text
[Enter exact test command]
```

## Build or Package

```text
[Enter exact build/package command, or state Not applicable]
```

## Source Code Appendix

Before submission, follow `../documentation/appendix/README.md` to copy all application and test source code into one or more plain-text appendix files.

### Implementation Checklist

- [ ] Required features are complete and trace to requirements.
- [ ] A new user can set up and run the system using this README.
- [ ] Code is readable, consistently formatted, and appropriately documented.
- [ ] Inputs, errors, and failure conditions are handled.
- [ ] Dependencies and configuration are documented without exposing secrets.
- [ ] Build output, dependencies, and unrelated files are not committed.
- [ ] The plain-text source appendix matches the final application and test source code.
