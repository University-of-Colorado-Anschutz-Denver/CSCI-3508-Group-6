# Testing Report

## 1. Testing Summary

| Field | Value |
| --- | --- |
| Project | [Enter project name] |
| Test period | [YYYY-MM-DD to YYYY-MM-DD] |
| Testers | [Enter names] |
| Build/version tested | [Enter version or commit] |
| Overall result | [Pass / Conditional pass / Fail] |

**Quality assessment:** [Summarize what was tested, major findings, and whether the software is ready for demonstration or release.]

## 2. Test Strategy

**Objectives:** [Explain what testing must establish.]

**Scope:** [List features and quality attributes included and excluded.]

**Test levels and types:**

| Test type | Purpose | Tool or method | Responsible member |
| --- | --- | --- | --- |
| Unit | [Purpose] | [Framework/manual method] | [Name] |
| Integration | [Purpose] | [Framework/manual method] | [Name] |
| System/end-to-end | [Purpose] | [Framework/manual method] | [Name] |
| Acceptance/usability | [Purpose] | [Participants/method] | [Name] |
| Non-functional | [Performance, security, accessibility, etc.] | [Tool/method] | [Name] |

**Test environment:** [Record hardware, operating system, browser/runtime, dependencies, data, and configuration. Do not include secrets.]

**Entry and exit criteria:** [Define when testing can start and what must be true for testing to finish.]

## 3. Requirements Coverage

Link every requirement to one or more cases in `../tests/test-cases.md` or automated test files. Keep the canonical end-to-end mapping in `02-requirements-specification.md`.

| Requirement ID | Test case or automated test | Test type | Result | Evidence |
| --- | --- | --- | --- | --- |
| FR-01 | [TC-01 or file/test name] | [Unit/integration/etc.] | [Pass/Fail/Blocked] | [Relative path or link] |
| NFR-01 | [TC-02 or file/test name] | [Performance/etc.] | [Pass/Fail/Blocked] | [Relative path or link] |

### Coverage Summary

| Requirement group | Total | Covered by tests | Passed | Uncovered/blocked IDs |
| --- | ---: | ---: | ---: | --- |
| Functional | [Number] | [Number] | [Number] | [IDs or None] |
| Non-functional | [Number] | [Number] | [Number] | [IDs or None] |

## 4. Test Execution Results

| Metric | Result |
| --- | ---: |
| Total cases run | [Number] |
| Passed | [Number] |
| Failed | [Number] |
| Blocked/skipped | [Number] |
| Automated code coverage | [Line/branch percentage and target, or Not applicable with reason] |

**Automated test command:** `[Enter the exact command]`

**Result summary:** [Describe patterns, failures, reliability across repeated runs, and important observations.]

## 5. Defects and Resolutions

| Defect ID | Description | Severity | Related requirement/test | Resolution | Retest build/date | Verification result/evidence |
| --- | --- | --- | --- | --- | --- | --- |
| BUG-01 | [Describe actual vs. expected behavior] | [Critical/High/Medium/Low] | [FR-01/TC-01] | [Change or accepted-risk reason] | [Version, YYYY-MM-DD, or Pending] | [Pass/Fail/Pending and reference] |

## 6. Reliability and Responsiveness

**Reliability evidence:** [Describe repeated tests, error handling, recovery behavior, and any known intermittent failures.]

**Performance or response-time evidence:** [Record targets, method, measurements, and results where relevant.]

**Compatibility and responsive-interface evidence:** [Record devices, screen sizes, operating systems, or browsers tested where relevant.]

## 7. Feedback and Resulting Changes

Record instructor, peer, stakeholder, or user feedback and show how the team responded.

| Date | Feedback source | Feedback | Team decision | Change/owner | Verification date and evidence |
| --- | --- | --- | --- | --- | --- |
| [YYYY-MM-DD] | [Person/group] | [Feedback received] | [Accepted/rejected/deferred and why] | [Action and owner] | [Date plus requirement/test/commit] |

## 8. Limitations and Remaining Risks

- [Untested behavior, known defect, environmental limitation, or remaining risk]
- [Explain impact and recommended future action]

## 9. Testing Conclusion

[State whether the implementation satisfies its requirements and support the conclusion with evidence.]

### Testing Completeness Checklist

- [ ] Normal, boundary, invalid, and failure cases are included.
- [ ] Functional and non-functional requirements are covered.
- [ ] Expected and actual results are recorded.
- [ ] Automated tests can be run using documented commands.
- [ ] Failed cases and defects have clear dispositions.
- [ ] Feedback and the team's response are documented.
- [ ] Test evidence contains no passwords, keys, or private user data.
