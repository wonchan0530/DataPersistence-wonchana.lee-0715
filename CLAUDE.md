# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository purpose

This repo (`DataPersistence`) is one of several standalone PoC (Proof of Concept) repositories required for a
larger assignment: the "반도체 시료 생산주문관리 시스템" (semiconductor sample production/order management
console system). Each PoC repo validates one technical concern in isolation before it gets reused in the final
`SampleOrderSystem` project.

This PoC's scope, per `docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf`:

> 데이터를 JSON 파일로 관리하는 CRUD(Create, Read, Update, Delete) 콘솔 애플리케이션을 개발한다.
> - PoC에서 사용된 코드 구조를 유지한 상태로, CRUD 구현
> - Create: 새로운 데이터를 입력 받아 JSON 파일에 저장
> - Read: 전체 목록 보기 및 특정 ID/키 값으로 검색 기능
> - Update: 기존 데이터를 선택하여 특정 필드 수정
> - Delete: 특정 데이터를 안전하게 삭제

"데이터 영속성" here specifically means: the application can be restarted and still see previously saved data,
because state lives in a JSON file on disk rather than only in memory.

There is currently no source code in this repository — only the `docs/` folder with the assignment slide decks.
Repository structure, language, and tooling have not been decided yet.

## Repository-wide context (from `docs/`)

- This repo is one of five sibling PoC/project repos in the same assignment series (`ConsoleMVC`, `DataPersistence`,
  `DataMonitor`, `DummyDataGenerator`, `SampleOrderSystem`). Code and conventions established here are meant to be
  reused later in `SampleOrderSystem` (an MVC console app), so prefer a structure that is easy to lift into an
  MVC Model layer later (e.g. a small repository/store class around the JSON file, not logic scattered through
  ad-hoc scripts).
- Per `docs/[CRA_AI] Day2_1_Agentic Engineering.pdf`, PoC-stage work is explicitly **not** expected to follow the
  full Agentic Engineering process (branch → requirements discussion → design doc → review → implement → review →
  merge). PoCs are meant to be built quickly ("Vibe Coding") in an isolated environment purely to validate the
  approach — the rigor (design docs, review, tests-as-gates) is expected later in the `SampleOrderSystem` project,
  not necessarily in this repo.
- Once the JSON read/write/CRUD pattern here is understood and settled, it should be treated as a trusted, reusable
  building block — i.e. "PoC 코드"— that later work can reference directly rather than re-deriving.

## Working in this repo

Since no implementation exists yet, there are no build/lint/test commands to record. When implementation starts:
- Confirm the target language/runtime with the user before scaffolding anything (not yet decided).
- Keep the JSON persistence logic isolated behind a small, dedicated module/class (load, save, and CRUD operations)
  so it can be dropped into the `SampleOrderSystem` Model layer later with minimal changes.
- Update this file with actual run/test/lint commands and architecture notes once code exists.
