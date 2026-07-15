cmake_minimum_required(VERSION 3.16)
project(DataPersistenceApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DataPersistenceApp
    src/main.cpp
    src/repository/SampleRepository.cpp
    src/repository/OrderRepository.cpp
    src/console/SampleMenu.cpp
    src/console/OrderMenu.cpp
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

enable_testing()

add_executable(DataPersistenceTests
    tests/test_main.cpp
    src/repository/SampleRepository.cpp
    src/repository/OrderRepository.cpp
)

target_include_directories(DataPersistenceTests PRIVATE
    include
    third_party
)

if (MSVC)
    target_compile_options(DataPersistenceTests PRIVATE /utf-8 /W4)
endif()

add_test(NAME DataPersistenceTests
    COMMAND DataPersistenceTests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
