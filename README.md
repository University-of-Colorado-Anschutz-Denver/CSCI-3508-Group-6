# [Enter Project Name Here]

> Intro to Software Engineering group project template

Replace every bracketed placeholder in this repository with information about your project. Delete instructional text that does not belong in your final submission.

## Project Information

| Field | Enter your information |
| --- | --- |
| Course | CSCI 3508 |
| Instructor | Dr. Salah Boukhris |
| Semester | FA2026 |
| Project subject | Metadata Editing and Organization Application |
| Approval status | [Pending / Approved on YYYY-MM-DD] |
| Repository URL | https://github.com/University-of-Colorado-Anschutz-Denver/CSCI-3508-Group-6 |

## Team Members

| Name | Role | Primary responsibilities | Contact |
| --- | --- | --- | --- |
| Harrison Julius | Group Leader | [Enter responsibilities] | juliuszharrison@gmail.com |
| Teja Kandimalla | Team Member | [Enter responsibilities] | tejakandimalla20@gmail.com |
| Emelie Kronquist | Team Member | [Enter responsibilities] | emelie.kronquist@gmail.com |
| Yannick Kuete | Team Member | [Enter responsibilities] | yankuete@gmail.com |

## Project Summary

**Problem:** It is difficult to view and edit common metadata fields for media files, such as title, authors, and more, directly from a file explorer. There are also many hidden metadata fields for different types of media files. These hidden fields can be used to sort and tag files, but as-is there is to way to access them.

**Proposed solution:** Program to view metadata for media files (pngs, mp4s, wavs, etc), and edit common metadata fields like title, author, and comments. Will run as a .exe on a user's desktop/laptop and act as a more focused file explorer that allows users to see all metadata files natively. Allows users to create metadata tags for files and search by tags through specific directories on their machine. As a secondary goal, we aim to create a website to allow users to download the application, provide user guides, and allow users to provide feedback on the program.

**Target users:** Media editors (film, sound design, photographs, etc); people with large collections of images, music, video stored on their devices

**Course relevance:** The project will allow us to develop our computer science skills, including the usage of C++, JavaScript, new code libraries, GUI development, and our understanding of file metadata. We will also be able to use the process frameworks and DevOps strategies discussed in class to iterate on features, add new features based on changing requirements, implementing changes, using version control software, and getting real user feedback from stakeholders.

## Repository Structure

Keep project work in exactly these three top-level folders. Do not rename them or create another top-level work folder.

```text
.
|-- documentation/   Requirements, design, reports, teamwork, and final files
|-- code/            Application source code and implementation instructions
`-- tests/           Automated tests, manual test cases, and test evidence
```

See each folder's `README.md` before adding files.

## Create and Clone Your Team Repository

The group leader must create the team's shared, team-owned repository from this template before anyone adds project work. Every student, including the group leader, must then clone that new team repository, not this original course template.

Follow the illustrated, step-by-step guide in [`documentation/00-clone-and-setup.md`](documentation/00-clone-and-setup.md). It includes GitHub interface illustrations, Windows and macOS/Linux commands, collaborator setup, verification steps, and troubleshooting.

## Start Here

1. ~~Have the group leader create the team-owned repository and give the other students access; then have every student clone it by following the guide above.~~
2. Fill in the project and team information above.
3. Complete `documentation/01-project-proposal.md` and obtain subject approval before development.
4. Assign responsibilities and record them in `documentation/06-team-collaboration-log.md`.
5. Document requirements before designing and implementing the system.
6. Keep application source code in `code/` and test code in `tests/`.
7. Update documentation throughout the project, not only before the deadline.
8. Complete `documentation/08-rubric-checklist.md` before submission.

## GitHub Best Practices for Beginners

Use the following workflow for every task. It keeps the team synchronized and makes each student's contribution visible. Run these commands from inside the cloned repository.

### 1. Pull Before Starting Work

First, confirm that you do not have unfinished changes:

```console
git status
```

If the working tree is clean, update `main` and create a branch for one specific task:

```console
git switch main
git pull --ff-only
git switch -c docs/project-proposal
```

Replace `docs/project-proposal` with a short branch name that describes the task. Examples include `feature/user-login`, `fix/login-error`, `test/login-validation`, and `docs/requirements`.

Do not pull, switch branches, or discard files when `git status` shows changes you do not understand. Ask a teammate or instructor for help first. If `git pull --ff-only` fails, stop and get help; do not force the update.

### 2. Make and Review a Small Change

Keep each branch focused on one feature, fix, test, or documentation task. Before committing, inspect what changed:

```console
git status
git diff
```

Stage only the intended files instead of automatically staging everything:

```console
git add README.md documentation/01-project-proposal.md
git diff --staged
```

Check that the staged diff contains no unrelated files, passwords, tokens, private data, generated files, or debugging output.

### 3. Commit with a Clear Message

Commit a complete, understandable unit of work:

```console
git commit -m "Add project proposal and team details"
```

Write a short message that says what the commit accomplishes. Prefer messages such as `Add login validation tests` over vague messages such as `update`, `changes`, or `stuff`.

### 4. Push the Task Branch

Push your branch, not `main`:

```console
git push -u origin docs/project-proposal
```

The first push uses `-u` to connect the local and GitHub branches. Later pushes from the same branch can use `git push`.

If Git rejects a push, do not force-push. Run `git status`, read the full error, and confirm that nobody else is using the same branch. Ask a teammate or instructor before trying to combine diverged work.

### 5. Open and Review a Pull Request

1. Open the team repository on GitHub and create a pull request from the task branch into `main`.
2. Explain what changed, why it changed, which requirements it supports, and how it was tested.
3. Review the **Files changed** tab. If it includes accidental changes, return to the task branch, correct them, and review the new diff before merging.
4. Ask at least one teammate to review the pull request.
5. Address review comments on the same branch, then inspect, stage, commit, and push the corrections as described above.
6. Merge only after the work is approved and checks or tests pass.

Do not use another student's GitHub account or approve work you did not review.

### 6. Update After the Pull Request Is Merged

After the pull request is merged, confirm that your working tree is clean and update `main`:

```console
git status
git switch main
git pull --ff-only
```

Create a new branch from the updated `main` for the next task. Do not reuse the merged task branch.

### Safety and Teamwork Rules

- Never commit or push project work directly to `main`.
- Never commit passwords, API keys, access tokens, personal data, or `.env` files. Also exclude dependencies, build output, editor files, and large generated artifacts.
- Do not share a task branch unless the team agrees first, and communicate before editing the same file or section as another teammate.
- Resolve merge conflicts carefully with the affected teammate; test the result before committing it.
- Never use force-push, `git reset --hard`, or file-deletion commands to recover from a Git problem. Stop and ask a teammate or instructor for help.
- Do not rewrite or claim another student's commits. Git history is part of the teamwork evidence.

For cloning, authentication, command explanations, and troubleshooting, see [`documentation/00-clone-and-setup.md`](documentation/00-clone-and-setup.md).

## Grading Roadmap (200 Points)

| Category | Points | Required evidence in this repository |
| --- | ---: | --- |
| Requirements Gathering | 20 | `documentation/02-requirements-specification.md` |
| System Design | 30 | UML diagrams, architecture, and schema in `documentation/03-system-design.md` |
| Implementation | 60 | Working, readable source code in `code/` with setup instructions |
| Testing | 40 | Test strategy and results in `documentation/04-testing-report.md`; test cases and evidence in `tests/` |
| Final Report & Documentation | 20 | Completed report based on `documentation/05-final-report-template.md`, exported as PDF |
| Presentation and Teamwork | 30 | Presentation based on `documentation/07-presentation-outline.md` and contribution evidence in `documentation/06-team-collaboration-log.md` |
| **Total** | **200** | Complete every row in `documentation/08-rubric-checklist.md` |

## Required Final Deliverables

Use these final submission paths unless your instructor specifies otherwise. Keep the PDF and PowerPoint in `documentation/deliverables/` and the plain-text source appendix in `documentation/appendix/`.

| Deliverable | Required format | Expected path |
| --- | --- | --- |
| Comprehensive project report | PDF | `documentation/deliverables/final-report.pdf` |
| Project presentation | PowerPoint (`.pptx`) | `documentation/deliverables/project-presentation.pptx` |
| Application and test source code appendix | Plain text (`.txt`) | `documentation/appendix/source-code-appendix.txt` or clearly numbered `.txt` files |

The original, runnable files must also remain in `code/` and `tests/`. The plain-text appendix is an additional submission requirement, not a replacement for the runnable files.

## Submission Checklist

- [ ] The project subject was approved and the approval was recorded.
- [ ] All four team members and their contributions are identified.
- [ ] Requirements are clear, complete, testable, and traceable.
- [ ] UML diagrams, architecture, and data/schema design match the implementation.
- [ ] Source code runs by following the instructions in `code/README.md`.
- [ ] Tests cover the requirements and include recorded results.
- [ ] Feedback and resulting changes are documented.
- [ ] The final report has been proofread and exported to PDF.
- [ ] The PowerPoint presentation is complete and has been rehearsed.
- [ ] All application and test source code appears in the plain-text appendix.
- [ ] No bracketed placeholders or template instructions remain in final files.
- [ ] All links and file paths work from a fresh copy of the repository.
