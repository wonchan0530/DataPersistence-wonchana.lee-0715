cmake_minimum_required(VERSION 3.16)
project(DataPersistenceApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DataPersistenceApp
    src/main.cpp
    src/repository/SampleRepository.cpp
    src/console/SampleMenu.cpp
)

target_include_directories(DataPersistenceApp PRIVATE
    include
    third_party
)

if (MSVC)
    # Phase 0 PoC에서 확인: 한글 주석/문자열이 포함된 소스는 /utf-8 옵션 없이는
    # 기본 코드페이지로 잘못 해석되어 컴파일 오류가 발생한다.
    target_compile_options(DataPersistenceApp PRIVATE /utf-8 /W4)
endif()
