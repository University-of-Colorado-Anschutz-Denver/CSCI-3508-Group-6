# Tests

Keep automated tests, manual test cases, test data, and non-sensitive evidence in this folder. Summarize final coverage and results in `../documentation/04-testing-report.md`.

## Test Organization

Adapt this structure to the selected technology.

```text
tests/
|-- [unit/]
|-- [integration/]
|-- [system-or-e2e/]
|-- [fixtures-or-test-data/]
|-- evidence/
|-- test-cases.md
`-- README.md
```

Do not commit passwords, API keys, private user information, large generated reports, or environment-specific temporary output.

## Prerequisites

- [Required runtime and version]
- [Testing framework and version]
- [Required local service or safe test data]

## Run All Automated Tests

Starting from [enter working directory]:

```text
[Enter exact command]
```

Expected result: [Describe the success output and expected test count.]

## Run Test Groups

| Test group | Command | Purpose |
| --- | --- | --- |
| Unit | `[Enter command]` | Tests isolated functions/classes/components |
| Integration | `[Enter command]` | Tests interactions between components/services |
| System/end-to-end | `[Enter command]` | Tests complete user workflows |
| Coverage or quality | `[Enter command]` | Produces coverage or static quality results |

## Test Environment and Data

[Explain required configuration, test doubles, fixtures, accounts, seed data, cleanup, and how tests remain repeatable.]

## Evidence

| Evidence | Location | Notes |
| --- | --- | --- |
| Manual case results | `test-cases.md` | [Enter test cycle/date] |
| Automated result summary | `../documentation/04-testing-report.md` | [Enter version/commit] |
| Screenshots/curated log excerpts | `evidence/[relative/path]` | [Save useful, non-sensitive excerpts as image, Markdown, or `.txt` files] |

## Testing Checklist

- [ ] Every requirement maps to one or more tests.
- [ ] Tests include normal, boundary, invalid, and failure behavior.
- [ ] Expected outcomes are specific and verifiable.
- [ ] Tests are repeatable and independent where practical.
- [ ] Failed tests and known defects are not hidden.
- [ ] Test commands work from a fresh repository copy.
