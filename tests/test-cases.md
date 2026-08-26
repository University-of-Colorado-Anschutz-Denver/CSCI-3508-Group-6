# Test Cases and Execution Record

Use this file for manual cases and as an index for automated tests. Give each case a stable ID and link it to one or more requirements.

## Test Cycle

| Field | Value |
| --- | --- |
| Project | [Enter project name] |
| Build/version/commit | [Enter identifier] |
| Environment | [Enter operating system, runtime, browser/device, and relevant configuration] |
| Test date(s) | [YYYY-MM-DD] |
| Tester(s) | [Enter names] |

## Detailed Test Cases

Copy this subsection for each case.

### TC-01: [Enter Descriptive Test Name]

| Field | Details |
| --- | --- |
| Related requirement(s) | [FR-01, NFR-01] |
| Test type | [Unit/Integration/System/Acceptance/Performance/etc.] |
| Priority | [High/Medium/Low] |
| Preconditions | [Required state and data] |
| Test data | [Inputs, using safe non-sensitive values] |

**Steps:**

1. [Enter action.]
2. [Enter action.]
3. [Enter action.]

**Expected result:** [Enter one specific, observable result.]

**Actual result:** [Record what happened during execution.]

**Status:** [Not run / Pass / Fail / Blocked]

**Evidence:** [Enter screenshot, log, automated test, issue, or commit reference.]

**Notes/defect:** [Enter defect ID and details, or None.]

**Execution history:**

| Run date | Build/commit | Tester | Result | Evidence/defect |
| --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Identifier] | [Name] | [Pass/Fail/Blocked] | [Reference] |

---

### TC-02: [Enter Boundary, Invalid, or Failure Case]

| Field | Details |
| --- | --- |
| Related requirement(s) | [Enter ID(s)] |
| Test type | [Enter type] |
| Priority | [High/Medium/Low] |
| Preconditions | [Required state and data] |
| Test data | [Boundary, malformed, missing, or failure-triggering input] |

**Steps:**

1. [Enter action.]
2. [Enter action.]

**Expected result:** [Describe safe error handling and unchanged/expected system state.]

**Actual result:** [Record what happened during execution.]

**Status:** [Not run / Pass / Fail / Blocked]

**Evidence:** [Enter reference.]

**Notes/defect:** [Enter defect ID and details, or None.]

**Execution history:**

| Run date | Build/commit | Tester | Result | Evidence/defect |
| --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Identifier] | [Name] | [Pass/Fail/Blocked] | [Reference] |

## Automated Test Index

| Test ID/name | File and test name | Related requirement(s) | What it verifies |
| --- | --- | --- | --- |
| [AT-01] | `[relative/path::test_name]` | [FR-01] | [Behavior] |

## Execution Summary

| Status | Count |
| --- | ---: |
| Pass | [Number] |
| Fail | [Number] |
| Blocked | [Number] |
| Not run | [Number] |
| **Total** | [Number] |

**Summary and next actions:** [Explain failures, retesting, and remaining risks. Transfer final conclusions to `../documentation/04-testing-report.md`.]
