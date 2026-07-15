#pragma once

#include <iostream>
#include <optional>
#include <string>

#include "repository/RepositoryResult.hpp"

namespace dp::console {

// Create/Update/Delete 결과 메시지를 출력하는 공통 헬퍼.
// (Sample/Order 각 메뉴에서 3회 이상 반복되어 Rule of Three에 따라 추출함. DESIGN.md Phase 5 참고)
inline void printResult(const RepositoryResult& result) {
    std::cout << result.message << "\n";
}

// 한 줄 문자열 입력. 스트림이 끊기면(EOF) 빈 문자열을 반환한다.
// 호출부에서 EOF 자체를 감지해야 하는 경우(메뉴 루프 등)는 readLine 이후 std::cin의 상태를 직접 확인한다.
inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }
    return line;
}

namespace detail {

// line 전체가 숫자로 소비되었을 때만 값을 반환한다 (예: "12abc"는 실패로 처리).
// readInt/readDouble/readOptionalInt/readOptionalDouble 4곳에서 반복되던 try/catch 파싱 로직을 추출함
// (Rule of Three: 4회 반복 확인). DESIGN.md 참고.
template <typename T, typename ParseFn>
std::optional<T> parseFull(const std::string& line, ParseFn parse) {
    try {
        size_t pos = 0;
        T value = parse(line, &pos);
        if (pos != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

inline std::optional<int> parseInt(const std::string& line) {
    return parseFull<int>(line, [](const std::string& s, size_t* pos) { return std::stoi(s, pos); });
}

inline std::optional<double> parseDouble(const std::string& line) {
    return parseFull<double>(line, [](const std::string& s, size_t* pos) { return std::stod(s, pos); });
}

}  // namespace detail

// 정수 입력. 파싱 실패 시 nullopt.
inline std::optional<int> readInt(const std::string& prompt) { return detail::parseInt(readLine(prompt)); }

// 실수 입력. 파싱 실패 시 nullopt.
inline std::optional<double> readDouble(const std::string& prompt) { return detail::parseDouble(readLine(prompt)); }

// 수정 메뉴 전용: 입력이 비어있으면 "변경 없음"(nullopt)으로 해석한다.
inline std::optional<std::string> readOptionalLine(const std::string& prompt) {
    const std::string line = readLine(prompt);
    if (line.empty()) {
        return std::nullopt;
    }
    return line;
}

inline std::optional<double> readOptionalDouble(const std::string& prompt) {
    const std::string line = readLine(prompt);
    if (line.empty()) {
        return std::nullopt;
    }
    return detail::parseDouble(line);
}

inline std::optional<int> readOptionalInt(const std::string& prompt) {
    const std::string line = readLine(prompt);
    if (line.empty()) {
        return std::nullopt;
    }
    return detail::parseInt(line);
}

}  // namespace dp::console
