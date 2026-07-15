# PLAN.md

## 개요

`PRD.md`에서 정의한 JSON 기반 CRUD 콘솔 애플리케이션을, `docs/[CRA_AI] Day2_1_Agentic Engineering.pdf`의
"PoC → Application 개발" 흐름(29, 32페이지)에 따라 단계적으로 구현하기 위한 계획이다.

- **언어/런타임**: C++ (빌드 시스템: CMake)
- **JSON 처리**: PoC(Phase 0)에서 라이브러리를 직접 검증한 뒤 채택 (1순위 후보: `nlohmann/json`, 헤더 온리로
  의존성 관리가 간단하고 STL 컨테이너와의 변환이 쉬움)
- **원칙**: PoC 단계(Phase 0)는 Vibe Coding으로 빠르게 검증하고, Application 개발 단계(Phase 1 이후)부터는
  Agentic Engineering 절차(요구사항 정리 → 설계 → 검토 → 구현 → 코드 리뷰 → merge)를 적용한다.

## Phase 0 — PoC: JSON 파싱/저장 라이브러리 검증

**목표**: C++에서 JSON 파일을 읽고 쓰는 방법을 검증하고, 익숙한 코드로 만든다.

- 별도의 독립 디렉토리(예: `poc/json-poc/`)에서 진행, 본 프로젝트 구조와 분리
- `nlohmann/json` (혹은 대안 라이브러리) 도입, 다음 동작을 최소 예제로 검증
  - 구조체 ↔ JSON 객체 직렬화/역직렬화
  - JSON 배열 파일 전체 읽기/쓰기 (`std::ifstream`/`std::ofstream`)
  - 파일이 없을 때의 초기화 처리, 파싱 실패 시 예외 처리
- Agentic Engineering 적용 없이 빠르게 구현 (문서화·리뷰 절차 생략)

**산출물**: 이해된 PoC 코드 스니펫 (커밋 히스토리 또는 `poc/` 디렉토리에 남김)
**완료 기준**: 구조체 → JSON 파일 저장 → 재실행 후 로드까지 한 사이클을 코드로 직접 이해하고 설명할 수 있음

## Phase 1 — 요구사항 확정 및 설계 문서 작성

**목표**: PoC에서 검증한 구조를 실제 애플리케이션 구조로 옮기기 위한 설계를 확정한다.

- `PRD.md`의 데이터 모델(시료/주문)을 C++ 구조체/클래스로 매핑
- 파일 구조 결정: `data/samples.json`, `data/orders.json` (파일 분리 전략)
- 모듈 구조 결정 (재사용을 고려한 계층 분리)
  - `Model`: `Sample`, `Order` 데이터 구조
  - `Storage`/`Repository`: JSON 파일 입출력 + CRUD 로직 (`SampleRepository`, `OrderRepository`)
  - `Console`/`View`: 메뉴 출력 및 사용자 입력 처리
  - `Controller`: 사용자 입력 → Repository 호출 연결
- 인간 검토(설계 및 요구사항 검토) 후 다음 Phase 진행

**산출물**: 설계 메모(본 PLAN.md 내 구조 반영, 필요 시 별도 `DESIGN.md`)
**완료 기준**: 디렉토리/클래스 구조와 파일 포맷(JSON 스키마)이 확정됨

## Phase 2 — 프로젝트 스켈레톤 구성

**목표**: 빌드 가능한 최소 골격을 만든다.

- CMake 프로젝트 초기화 (`CMakeLists.txt`), 서드파티 라이브러리 연동(vcpkg/FetchContent 등)
- 디렉토리 구조 생성: `src/`, `include/`, `data/`, `tests/`
- `Sample`, `Order` 구조체 정의 및 JSON 변환 함수(`to_json`/`from_json`) 작성
- 빈 상태에서 실행 시 데이터 파일 자동 초기화 로직 작성 (NFR-3)

**산출물**: 컴파일되는 스켈레톤 프로젝트 (기능 없이 빌드/실행만 확인)
**완료 기준**: `cmake --build`로 빌드 성공, 실행 시 빈 메뉴라도 뜸

## Phase 3 — 시료(Sample) CRUD 구현

Agentic Engineering 기능 추가 절차 적용: Branch 생성 → (요구사항은 PRD 5.2절로 확정됨) → 구현 → 코드 리뷰 → merge

- `SampleRepository` 구현: JSON 파일 load/save, Create/Read(전체·검색)/Update/Delete
- 콘솔 메뉴 연동: 시료 등록/조회/검색/수정/삭제
- 예외 케이스 처리: 중복 ID 등록 거부, 존재하지 않는 ID 조회/수정/삭제 시 안내 메시지 (FR-4, FR-5~FR-9)

**완료 기준**: 시료 CRUD 전체 시나리오가 콘솔에서 동작하고, 재시작 후에도 등록한 시료가 남아있음

## Phase 4 — 주문(Order) CRUD 구현

Phase 3과 동일한 절차 적용.

- `OrderRepository` 구현: JSON 파일 load/save, Create/Read(전체·검색)/Update/Delete
- 콘솔 메뉴 연동: 주문 등록(예약)/조회/검색/수정/삭제
- 시료 ID 참조 무결성 검증: 존재하지 않는 시료 ID로는 주문 생성 불가 (FR-10)

**완료 기준**: 주문 CRUD 전체 시나리오가 콘솔에서 동작하고, 시료 데이터와 독립적으로 영속화됨

## Phase 5 — 통합 및 콘솔 UI 정리

**목표**: 개별 기능을 하나의 애플리케이션으로 통합한다.

- 메인 메뉴에서 시료 관리 / 주문 관리로 진입하는 상위 메뉴 구성
- 입력값 검증 공통화 (숫자 입력, 범위 검증 등 중복 로직 정리)
- 사용자 피드백 메시지 일관성 정리 (성공/실패 메시지 포맷 통일)

**완료 기준**: 하나의 실행 파일에서 시료·주문 CRUD를 모두 메뉴로 접근 가능

## Phase 6 — 테스트 및 영속성 검증

**목표**: 데이터 영속성이라는 핵심 요구사항을 명시적으로 검증한다.

- 유닛 테스트 프레임워크 도입 (GoogleTest 또는 Catch2)
- Repository 계층 단위 테스트: Create/Read/Update/Delete 각각에 대한 정상/예외 케이스
- 영속성 시나리오 테스트: 데이터 저장 → 프로세스 재시작(또는 Repository 재생성) → 동일 데이터 로드 확인
- 손상된/누락된 JSON 파일에 대한 복구 동작 테스트 (NFR-1)

**완료 기준**: 테스트 스위트가 CI 없이 로컬에서 전부 통과, 재현 가능한 회귀 방지 체계 확보

## Phase 7 — 코드 리뷰 및 리팩토링

**목표**: AI가 생성한 코드를 사람이 검토하고 책임지는 절차를 명시적으로 남긴다.

- Rule of Three에 따라 중복 코드 리팩토링 여부 판단
- 클래스별 단일 책임 원칙(SRP) 준수 여부 점검 (Repository/Model/View 책임 분리 유지)
- 코드 리뷰 완료 후, Commit Message에 "리뷰 완료 및 책임" 의미의 표시(리뷰 도장) 남기기

**완료 기준**: 주요 모듈에 대해 사람의 리뷰 기록이 커밋 히스토리에 남음

## Phase 8 — 문서화 마무리 및 제출 준비

- `CLAUDE.md` 갱신: 실제 빌드/실행/테스트 명령어, 최종 아키텍처 반영
- `README.md` 작성(빌드 방법, 실행 방법, 폴더 구조 요약)
- 최종 커밋 정리 및 Public Repository로 제출 준비 (`DataPersistence-본인영문이름-사번` 네이밍)

**완료 기준**: 처음 접하는 사람이 문서만 보고 빌드·실행·CRUD 시나리오를 재현할 수 있음

## Phase 요약

| Phase | 내용 | 산출물 |
|---|---|---|
| 0 | JSON 라이브러리 PoC | 이해된 PoC 코드 |
| 1 | 요구사항/설계 확정 | 모듈 구조, JSON 스키마 |
| 2 | 프로젝트 스켈레톤 | 빌드되는 뼈대 |
| 3 | 시료 CRUD | 동작하는 시료 관리 기능 |
| 4 | 주문 CRUD | 동작하는 주문 관리 기능 |
| 5 | 통합/UI 정리 | 통합 콘솔 애플리케이션 |
| 6 | 테스트/영속성 검증 | 테스트 스위트 |
| 7 | 코드 리뷰/리팩토링 | 리뷰 기록이 남은 커밋 |
| 8 | 문서화/제출 준비 | 최종 README/CLAUDE.md, 제출본 |
