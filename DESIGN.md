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
