# DataPersistence

JSON 파일로 데이터를 저장·조회하는 CRUD(Create, Read, Update, Delete) 콘솔 애플리케이션 PoC.
반도체 시료 생산주문관리 시스템의 핵심 데이터인 **시료(Sample)** 와 **주문(Order)** 을 다루며, 애플리케이션을
재시작해도 이전에 저장한 데이터가 그대로 유지된다(데이터 영속성).

- 요구사항: [`PRD.md`](PRD.md)
- 개발 계획(Phase 0~8): [`PLAN.md`](PLAN.md)
- Phase별 설계/구현/피드백 기록: [`DESIGN.md`](DESIGN.md)

## 요구사항

- C++17 컴파일러 (개발/검증 환경: Windows + Visual Studio 2022 Build Tools의 MSVC `cl.exe`)
- CMake 3.16 이상
- Ninja (Visual Studio 2022에 번들로 포함되어 있음)

## 빌드

가장 쉬운 방법은 Windows 시작 메뉴에서 **"Developer PowerShell for VS 2022"** (또는 Developer Command
Prompt)를 열어서 실행하는 것이다 — `cl.exe`/`cmake`/`ninja`가 이미 PATH에 잡혀 있다.

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

일반 `powershell.exe`/`cmd.exe`에서 곧바로 실행하는 경우 `vcvars64.bat`을 먼저 호출해 환경변수를 잡아야
한다. **주의**: `call`은 `cmd.exe` 전용 명령이라 일반 PowerShell에서 그대로 실행하면 `command not found`로
실패한다. PowerShell에서는 `cmd /c`로 감싸서 실행한다.

```powershell
# cmd.exe에서:
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake -S . -B build -G Ninja
cmake --build build

# 일반 PowerShell에서:
cmd /c '"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" && cmake -S . -B build -G Ninja && cmake --build build'
```

## 실행

```powershell
.\build\DataPersistenceApp.exe
```

최초 실행 시 `data/samples.json`, `data/orders.json`이 없으므로 빈 목록으로 시작하며, 시료/주문을 등록하면
해당 파일이 자동으로 생성된다. 프로그램을 다시 실행해도 이전에 등록한 데이터가 그대로 남아있다.

메뉴 구성:

```
[1] 시료 관리   [2] 주문 관리   [0] 종료

-- 시료 관리 --
[1] 시료 등록  [2] 시료 조회  [3] 시료 검색  [4] 시료 수정  [5] 시료 삭제  [0] 뒤로

-- 주문 관리 --
[1] 주문 등록  [2] 주문 조회  [3] 주문 검색  [4] 주문 수정  [5] 주문 삭제  [0] 뒤로
```

주문 등록 시 존재하지 않는 시료 ID를 입력하면 등록이 거부된다(참조 무결성).

## 테스트

```powershell
ctest --test-dir build --output-on-failure
```

Repository 계층의 CRUD 정상/예외 케이스, 시료 참조 무결성, 재시작 시뮬레이션을 통한 영속성 검증, 손상된
JSON 파일 방어 로직 등을 검증한다. 테스트는 실제 데이터(`data/`)가 아닌 별도의 `test_data/`를 사용한다.

## 폴더 구조

```
include/model/        Sample, Order 데이터 구조체 + JSON 변환
include/storage/       JsonFileStore: 범용 JSON 파일 로드/저장
include/repository/    SampleRepository, OrderRepository: 도메인 CRUD 규칙
include/console/       ConsoleIO, SampleMenu, OrderMenu: 콘솔 메뉴
src/main.cpp           실행 진입점 (의존성 조립)
tests/test_main.cpp    경량 자체 테스트 하네스
third_party/nlohmann/  json.hpp (벤더링된 JSON 라이브러리, v3.11.3)
poc/json_poc/          Phase 0 PoC 코드 (JSON 저장/로드 검증용, 본 앱과 무관)
docs/                  과제 원본 자료
```
