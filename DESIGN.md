# DESIGN.md

Phase별 구현 전 설계를 기록하는 문서. `PLAN.md`의 Phase 순서를 따라가며, 각 Phase는
**설계 → 구현 → 피드백(검토/수정)** 순으로 진행하고 그 결과를 아래에 이어서 기록한다.

---

## Phase 0 — PoC: JSON 파싱/저장 라이브러리 검증

### 설계

- **목표**: C++에서 구조체를 JSON으로 저장하고 다시 읽어오는 최소 사이클을 검증한다.
- **라이브러리 선정**: [`nlohmann/json`](https://github.com/nlohmann/json) (header-only, MIT License)
  - 선정 이유: 별도 빌드/링크 없이 헤더 하나만 포함하면 사용 가능, STL 컨테이너 변환이 자연스러움,
    `to_json`/`from_json` ADL 훅으로 사용자 구조체 직렬화가 간단함
  - 벤더링 위치: `third_party/nlohmann/json.hpp` (이후 본 프로젝트에서도 동일 파일을 재사용)
- **검증 범위** (본 프로젝트 코드와 완전히 분리된 `poc/json_poc/` 디렉토리에서 진행)
  1. 사용자 구조체 ↔ `nlohmann::json` 객체 변환 (`to_json`/`from_json`)
  2. 구조체 배열을 JSON 배열로 파일에 저장 (`std::ofstream`)
  3. 파일을 다시 읽어 구조체 배열로 복원 (`std::ifstream`)
  4. 파일이 없는 최초 실행 상황 처리 (빈 배열로 시작)
  5. 파싱 실패(손상된 JSON) 상황에서 예외를 잡아 처리
- **완료 기준**: PoC 실행 파일을 두 번 연속 실행했을 때, 첫 실행에서 저장한 더미 데이터를 두 번째
  실행에서 그대로 불러오는 것을 콘솔 출력으로 확인한다.
- **Agentic Engineering 적용 여부**: 미적용 (PoC 단계는 Vibe Coding으로 빠르게 검증)

### 피드백

- `to_json`/`from_json`을 ADL(인자 종속 검색)로 오버로드하면 `nlohmann::json`이 사용자 구조체를
  컨테이너(`std::vector<T>`)째로 직렬화/역직렬화할 수 있음을 확인 — 본 프로젝트의 Repository 계층에서
  그대로 재사용하기로 결정 (Phase 1에 반영).
- 파일이 없을 때 `std::ifstream`이 실패 상태가 되는 것을 `is_open()`으로 감지해 빈 배열로 초기화하는
  방식이 가장 단순하고 명확함 — `JsonFileStore`의 표준 동작으로 채택.
- 손상된 JSON 파싱 시 `nlohmann::json::parse`가 `json::parse_error` 예외를 던짐을 확인 — 이후
  Repository/Store 계층에서 반드시 `try/catch`로 감싸고 사용자에게 안내 메시지를 출력하도록 설계에 반영.
- MSVC(`cl.exe`, `/std:c++17`)로 컴파일 시 한글 주석이 포함된 UTF-8 소스가 기본 코드페이지(949)로
  잘못 해석되어 구문 오류가 발생함을 확인 — **`/utf-8` 컴파일 옵션을 반드시 추가**해야 함. 이후
  `CMakeLists.txt`에 MSVC 대상일 때 `/utf-8`을 기본 컴파일 옵션으로 추가하기로 결정 (Phase 2 반영).
- 실행 파일을 두 번 연속 실행한 결과, 1차 실행에서 저장한 더미 시료 1건을 2차 실행이 그대로 불러온 뒤
  누적 저장(2건)하는 것을 확인 — JSON 파일 기반 영속성이 정상 동작함을 검증.
- 저장된 JSON 파일 내 한글 문자열도 UTF-8로 정상 보존됨을 확인 (PowerShell 콘솔에 `Get-Content`로 출력할
  때는 콘솔 코드페이지 차이로 깨져 보이지만, 파일 자체의 바이트는 올바른 UTF-8임을 직접 파일을 읽어
  재확인함 — 실제 데이터 손상이 아님).

**결론**: PoC 검증 완료. `third_party/nlohmann/json.hpp`와 여기서 확인한 저장/로드 패턴, 그리고
`/utf-8` 컴파일 옵션 필요성을 본 프로젝트의 `JsonFileStore` 설계 및 `CMakeLists.txt`에 반영한다.

---

## Phase 1 — 요구사항 확정 및 데이터 모델 설계

### 설계

`PRD.md` 4장(데이터 모델 요구사항)을 C++ 구조체로 확정한다.

**`Sample` (시료)** — `include/model/Sample.hpp`

| 필드 | 타입 | 검증 규칙 |
|---|---|---|
| id | `std::string` | 비어있지 않음, 저장소 내 유일 |
| name | `std::string` | 비어있지 않음 |
| avgProductionTimeMin | `double` | 0보다 큼 |
| yieldRate | `double` | 0 초과 1 이하 |
| stock | `int` | 0 이상 |

**`Order` (주문)** — `include/model/Order.hpp`

| 필드 | 타입 | 검증 규칙 |
|---|---|---|
| orderId | `std::string` | 비어있지 않음, 저장소 내 유일 |
| sampleId | `std::string` | 등록된 `Sample.id`를 참조해야 함 |
| customerName | `std::string` | 비어있지 않음 |
| quantity | `int` | 0보다 큼 |
| status | `std::string` | 생성 시 `"RESERVED"`로 고정 초기화 |

두 구조체 모두 Phase 0에서 검증한 `to_json`/`from_json` ADL 패턴을 그대로 사용한다.

**모듈 구조** (재사용성을 고려해 계층 분리, `SampleOrderSystem`의 MVC Model 계층으로 이식 가능하도록 설계)

```
include/
  model/        Sample.hpp, Order.hpp            (순수 데이터 구조 + JSON 변환)
  storage/       JsonFileStore.hpp                (범용 JSON 파일 로드/저장 템플릿)
  repository/    SampleRepository.hpp/.cpp        (시료 CRUD + 유효성 검증)
                 OrderRepository.hpp/.cpp         (주문 CRUD + 시료 참조 무결성 검증)
  console/       ConsoleIO.hpp                    (입력 파싱 공통 유틸)
                 SampleMenu.hpp/.cpp              (시료 관리 메뉴)
                 OrderMenu.hpp/.cpp               (주문 관리 메뉴)
src/
  main.cpp       최상위 메뉴 및 의존성 조립(Composition Root)
data/
  samples.json, orders.json                      (런타임 데이터 파일, 최초 실행 시 자동 생성)
```

- `JsonFileStore<T>`는 파일 경로만 주입받아 `std::vector<T>` 단위로 load/save를 수행하는 순수 저장소 계층이다.
  파일이 없으면 빈 벡터, 파싱 실패 시 예외를 잡아 빈 벡터로 대체하고 경고를 반환한다 (Phase 0 검증 내용 반영).
- `SampleRepository`/`OrderRepository`는 `JsonFileStore<T>`를 감싸서 "중복 ID 금지", "존재하지 않는 ID
  조회/수정/삭제 시 실패 반환", "부분 필드 수정" 등 도메인 규칙을 구현한다. Repository는 File I/O 세부사항을
  몰라도 되고, Storage는 도메인 규칙을 몰라도 되도록 책임을 분리한다(SRP).
- `Console` 계층은 Repository의 반환값(성공/실패, 데이터)을 받아 사용자에게 안내 문구를 출력하는 역할만
  담당한다.

### 완료 기준

- 위 구조체/모듈 경계가 PRD 5장의 기능 요구사항(FR-5~FR-14)을 모두 표현할 수 있는지 확인
- Phase 2에서 이 구조 그대로 컴파일 가능한 스켈레톤을 만들 수 있는 수준까지 구체화

### 피드백

- `include/model/Sample.hpp`, `include/model/Order.hpp`를 설계대로 구현한 뒤, 임시 스모크 테스트로
  두 구조체가 한글 필드 값을 포함해 정상적으로 JSON 직렬화되는지 컴파일·실행하여 확인함 (검증 후 임시
  파일은 삭제, 결과만 본 문서에 기록).
- `Order.status`의 기본값을 구조체 멤버 초기화(`= "RESERVED"`)로 지정해, Repository 계층에서 별도로
  기본값을 채우지 않아도 되도록 설계를 단순화함.
- FR-5~FR-14 전 항목이 `Sample`/`Order` 필드와 Repository 계층의 책임(유일성 검증, 참조 무결성 검증,
  부분 수정)으로 표현 가능함을 확인 — Phase 2 스켈레톤으로 진행.

**결론**: 데이터 모델 및 모듈 구조 확정. Phase 2에서 CMake 스켈레톤에 그대로 반영한다.

---

## Phase 2 — 프로젝트 스켈레톤 구성

### 설계

- **빌드 시스템**: CMake (`CMakeLists.txt`) + Ninja 제너레이터, MSVC(`cl.exe`) 컴파일러
  - `CMAKE_CXX_STANDARD 17`
  - MSVC 대상일 때 `/utf-8`(Phase 0에서 확인한 한글 소스 인코딩 문제 대응), `/W4`(경고 강화) 적용
- **디렉터리 구조**: Phase 1에서 설계한 구조를 그대로 생성
  - `include/storage/JsonFileStore.hpp`: `Sample`/`Order` 어느 쪽에도 종속되지 않는 범용 JSON 파일
    저장소 템플릿. `load()`는 파일 부재/파싱 실패 시 빈 벡터로 대체(NFR-1, NFR-3), `save()`는 상위
    디렉터리가 없으면 `std::filesystem::create_directories`로 생성
  - `src/main.cpp`: Composition Root. `JsonFileStore<Sample>`/`JsonFileStore<Order>`를 `data/samples.json`,
    `data/orders.json` 경로로 생성하고, 최상위 메뉴(시료 관리/주문 관리/종료)만 뼈대로 구성
- **완료 기준**: `cmake --build`로 빌드 성공, 실행 시 메뉴가 출력되고 1/2/0 입력에 따라 분기함
  (CRUD 로직 자체는 Phase 3/4에서 구현)

### 피드백

- CMake+Ninja+MSVC 조합으로 빌드가 한 번에 성공함을 확인 (`cmake -S . -B build -G Ninja` →
  `cmake --build build`).
- 실행 파일에 `1`, `2`, `0`을 순서대로 입력했을 때 각각 "시료 관리(Phase 3 예정)", "주문 관리(Phase 4
  예정)", 정상 종료로 분기하는 것을 Bash 파이프 입력(`printf '1\n2\n0\n' | ...`)으로 확인함.
  - PowerShell에서 개행 문자열(`` `n ``)을 파이프로 넘겼을 때는 객체 스트림 처리 방식 차이로 입력이
    한 줄씩 밀리는 현상이 있었으나, 이는 테스트 방법(파이프 입력 방식)의 차이일 뿐 애플리케이션
    로직의 결함이 아님을 Bash 파이프 재현으로 교차 확인함. 이후 수동 검증은 Bash의 `printf | exe`
    방식을 기준으로 한다.
- 빌드 산출물(`build/`, `.vs/`, `*.obj`, `*.exe` 등)은 `.gitignore`에 이미 반영되어 있어 커밋 대상에서
  제외됨을 확인.

**결론**: 빌드 가능한 스켈레톤 확보. Phase 3부터 시료 CRUD를 이 뼈대 위에 구현한다.

---
