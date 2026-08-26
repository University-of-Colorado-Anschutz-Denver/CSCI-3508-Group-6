# System Design

## 1. Design Overview

**Project:** [Enter project name]

**Design goals:** [Summarize the qualities this design prioritizes and why.]

**Constraints:** [List technical, schedule, platform, security, or course constraints.]

Store editable diagram sources and readable exported images in `design-assets/`. Use relative links so diagrams display from a fresh repository copy.

## 2. Architecture

### Architecture Style

[Name the architecture style or pattern, such as layered, client-server, MVC, event-driven, or another appropriate choice. Explain why it fits the requirements.]

### Architecture Diagram

> [Insert a labeled architecture or component diagram here. Show major components and connections.]

**Diagram explanation:** [Explain each component, its responsibility, and how data or control moves through the system.]

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
