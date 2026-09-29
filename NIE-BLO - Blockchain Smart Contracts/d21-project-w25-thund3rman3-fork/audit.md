# Janečkova metoda D21 — Security Audit
**Auditor:** Adam Čapka  
**Repository:** [Link](https://github.com/Ackee-Blockchain-Education/d21-project-w25-thund3rman3)  
**Commit audited:** 8523f6b  
**Audit date:** 27.12.2025

---

## 1. Overview

This document presents the findings of a security audit of the reviewed smart contracts.

### 1.1 Audit Methodology

The audit was conducted using the following steps:

1. **Specification review**  
    The intended behavior of the system was derived from the assignment description. The audit process began by identifying the expected functional properties of the implementation, including voting rules, role responsibilities, and state transitions.

2. **Static analysis**  
   Static analysis was performed using the **Wake** framework. Wake was used to identify potential vulnerabilities, and code quality issues, which were then manually reviewed and either confirmed or dismissed during the subsequent manual analysis phase.

3. **Manual code review**  
   The codebase was inspected line by line and reviewed from a functional and security-oriented perspective, with a focus on identifying logic flaws, access control issues, and edge cases relevant to the security risks of the given assignment.

4. **Local deployment and interaction**  
   The contracts were deployed and executed in a local **Wake** development environment. Key functions and interaction flows were manually exercised to observe runtime behavior and validate assumptions made during the static and manual analysis phases.

5. **Unit and fuzz testing**  
   Existing unit tests were reviewed to assess test coverage and correctness. In addition, targeted fuzz and invariant tests were implemented to verify key security properties and system invariants under a wide range of inputs.

---

### 1.2 Finding Classification

Each finding is classified based on **Impact** and **Likelihood**, resulting in a final **Severity**.

| Impact \ Likelihood | High | Medium | Low | N/A |
|--------------------|------|--------|-----|-----|
| High | Critical | High | Medium | – |
| Medium | High | Medium | Low | – |
| Low | Medium | Low | Low | – |
| Warning | – | – | – | Warning |
| Info | – | – | – | Info |

**Impact definitions:**  
- **High:** Violations that compromise the integrity of the voting process, such as enabling unauthorized voting, bypassing voting rules (e.g. vote limits, negative vote constraints), manipulating results, or preventing correct vote finalization.
- **Medium:** Issues that allow limited rule violations, unfair advantages, denial of service to specific participants, or inconsistencies in voting outcomes without fully compromising the election.
- **Low:** Minor deviations from expected behavior that do not materially affect voting correctness, fairness, or final results, or are easily recoverable without impacting the overall outcome.

**Likelihood definitions:**  
- **High:** The issue can be exploited easily by any participant without special conditions.
- **Medium:** Exploitation requires specific timing, state, or interaction conditions.
- **Low:** Exploitation requires strict or unlikely conditions.

---

## 2. Executive Summary
The audited contracts implement the core mechanics of the D21 voting method
correctly and enforce most access control, voting limit, and time-based
constraints as intended by the assignment specification.

However, the audit identified two high-severity logic issues that violate
core specification requirements and may affect the fairness and integrity
of the voting process. Specifically, a single address can register multiple
subjects, and under certain conditions a subject may vote for itself.
Both issues can be exploited by untrusted participants without requiring
privileged access. Informational findings related to code quality and maintainability
were also noted.

All implemented fuzz tests completed successfully without detecting additional
issues beyond those documented. Several unit tests intentionally failed to
demonstrate the identified specification violations.

Overall, the system is close to a correct implementation of the intended
design but requires targeted fixes to fully comply with the assignment
requirements and ensure voting fairness.

### 2.1 Audit Scope
- **Audited commit:** 8523f6b
- **In-scope contracts:**
  - `D21.sol`
  - `IVoteD21.sol`

---

### 2.2 Summary of Results
The first step was to run static analysis using Wake. The analysis did not report any detections for the audited commit.

---

### 2.3 Findings Count by Severity

| Critical | High | Medium | Low | Warning | Info | Total |
|--------|------|--------|-----|---------|------|-------|
| 0 | 2 | 0 | 0 | 0 | 2 | 4 |

---

## 3. Findings Summary

| ID | Title | Severity | Status |
|----|------|----------|--------|
| H-01 | Multiple subjects can be registered by the same address | High | Open |
| H-02 | Voting to self is not prevented | High | Open |
| I-01 | Typo in custom error name | Info | Open |
| I-02 | Duplicated validation and vote logic | Info | Open |

---

## 4. System Overview

The D21 voting system implements Janeček’s voting method, allowing registered
voters to cast a limited number of positive and negative votes for registered
subjects within a fixed voting period. The contract owner is responsible for
voter registration and starting the voting phase, while subjects and voters are
otherwise untrusted participants.

---

## 5. Trust Model

- **Owner / Admin**
  - Fully trusted.
  - Responsible for registering voters and starting the voting period.
  - Cannot influence votes directly.

- **Voters**
  - Untrusted.
  - Any registered voter may attempt to exploit logical flaws to gain unfair advantage.

- **Subjects**
  - Untrusted.
  - May attempt to influence outcomes by registering multiple subjects or voting for themselves.

- **External contracts**
  - None assumed.

- **Attackers**
  - Arbitrary EOAs or contracts.
  - No special privileges assumed.

---

## 6. Findings

### H-01: Multiple subjects can be registered by the same address
**Severity:** High  
**Impact:** High  
**Likelihood:** Medium 
**Status:** Open
**Target:** D21.sol  
**Type:** Logic / Specification violation

#### Description
Subjects are identified by an address derived from the hash of their name
rather than being associated with the address that registered them. As a
result, the contract does not enforce the requirement that only one subject
can be registered per address, as stated in the assignment specification.

A single account can register an arbitrary number of subjects by submitting
different names, since no mapping exists between the registering address
and the created subject.

#### Impact
This behavior violates a core system rule and allows a single participant
to create multiple subjects, which can negatively affect the integrity and
fairness of the voting process.

#### Likelihood
Exploitation requires intentional registration of multiple subjects
by the same participant. While straightforward, it requires deliberate
misuse and is not triggered by default system behavior.

#### Severity rationale
This issue has **High impact** (violates a core specification requirement and
affects voting integrity) and **High likelihood** (can be exploited easily by
any user). According to the severity classification matrix, this results in a
**High severity** finding.

#### Proof of Concept
A single address can register multiple subjects by calling `addSubject`
multiple times with different names. This was reproduced using an audit
test (e.g., `tests/test_audit.py::test_poc_multiple_subjects_per_address`),
where the second registration from the same address succeeds.

#### Recommendation
Bind each subject to the registering address (`msg.sender`) and enforce that
a single address can register at most one subject. This can be achieved by
using the sender address as the subject identifier and rejecting repeated
registrations from the same address.

### H-02: Voting to self is not prevented
**Severity:** High  
**Impact:** High  
**Likelihood:** Medium 
**Status:** Open  
**Target:** D21.sol  
**Type:** Logic / Specification violation

#### Description
The assignment specification explicitly states that voting to self is not
allowed (UC12). However, the contract does not reliably prevent a voter from
casting a vote for their own subject.

Due to the subject identity model, subjects are not consistently represented
by the registering address. As a result, the self-voting restriction is not
effectively enforced, allowing a voter to cast a positive or negative vote
for a subject they registered themselves.

#### Impact
This issue allows participants to directly increase or decrease their own
voting score, violating the fairness and integrity of the voting process.
Self-voting can influence final results and undermines the intended guarantees
of the D21 voting method.

#### Likelihood
Exploitation requires a participant to both register a subject and
actively vote for it. While easy to perform, it requires deliberate action
and specific conditions.

#### Severity rationale
The issue has **High impact** (enables unfair self-voting) and **High likelihood** (exploitable by any subject owner). Based on the classification matrix, this corresponds to a **High severity** issue.

#### Proof of Concept
The issue was reproduced using audit-specific tests
(`test_vote_positive`, `test_vote_negative`, `test_vote_batch`), where a voter successfully casts a vote for their own registered subject without the transaction reverting.

#### Recommendation
Explicitly associate each subject with its registering address (owner) and
enforce a check preventing `msg.sender` from voting for subjects they own.

### I-01: Typo in custom error name
**Severity:** Informational  
**Status:** Open  
**Target:** D21.sol  
**Type:** Code quality

#### Description
The custom error `NeedOwnerPriviledges` contains a typo (“Priviledges”
instead of “Privileges”). While this does not affect contract behavior, it
reduces code readability and may cause confusion for developers or users
interacting with revert reasons.

#### Recommendation
Rename the error to `NeedOwnerPrivileges` to improve code clarity and
maintainability.

### I-02: Duplicated validation and vote-application logic between single-vote and batch-vote paths
**Severity:** Informational  
**Status:** Open  
**Target:** D21.sol  
**Type:** Maintainability / DRY

#### Description
The contract implements voting rules and checks in multiple locations:
- `votePositive`/`voteNegative` rely on shared internal helpers (e.g., `_voteRequirements`, `_votePositive`, `_voteNegative`),
- while `voteBatch` re-implements similar checks and vote-application logic inline.

Although the current behavior appears consistent, duplicating core rules increases the risk that future changes will update one path but not the other, leading to inconsistent enforcement of voting constraints.

#### Impact
No immediate security impact was observed in the audited version. However, rule duplication increases maintenance cost and the likelihood of introducing logic inconsistencies in future updates.

#### Recommendation
Refactor shared voting checks and vote-application logic into internal helper functions that can be reused by both single-vote and batch-vote flows.

---

## 7. Testing

### 7.1 Unit Testing
Unit tests were written to validate the contract behavior against the
assignment specification and documented use cases.

The unit test suite focuses on deterministic verification of:
- Access control rules (e.g., owner-only functions),
- Subject and voter registration constraints,
- Voting limits and ordering constraints,
- Time-based voting restrictions,
- Prevention of invalid actions such as self-voting or double-voting.

Each unit test targets a specific requirement or edge case and asserts
both successful and reverting behavior where applicable.

### 7.2 Fuzz Testing
In addition to unit tests, stateful fuzz testing was performed using the
Wake fuzzing framework.

The fuzz tests execute randomized sequences of contract interactions,
including:
- Subject registration,
- Voter registration,
- Voting start,
- Positive, negative, and batch voting,
- Time manipulation.

The goal of fuzzing was to validate that critical system invariants hold
across arbitrary execution paths and state transitions, including
unexpected call orderings.

### 7.3 Invariants Tested
The following invariants were enforced during fuzz testing:

- Only the contract owner can start the voting period.
- Subjects cannot be registered after voting has started.
- Voting to self is not allowed.
- Votes cannot be cast outside the voting window.
- A voter cannot exceed the maximum number of positive or negative votes.
- Batch voting enforces the same rules as individual voting.

### 7.4 Results and Coverage
All implemented fuzz tests completed successfully without triggering
unexpected behavior or invariant violations. No additional security
issues were discovered beyond the findings documented in this report.

Several unit tests intentionally failed for the audited implementation.
These failures correspond directly to the identified findings and serve
as proof-of-concept demonstrations of specification violations rather
than defects in the test suite itself.

Specifically, the failing unit tests highlight:
- The ability for a single address to register multiple subjects,
- The ability for a subject to vote for itself under certain conditions.

All other unit tests passed, confirming correct behavior for the
remaining specification requirements, including access control,
time-based restrictions, and vote limit enforcement.
