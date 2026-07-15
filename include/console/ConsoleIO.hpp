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
inline std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return "";
    }
    return line;
}

// 정수 입력. 파싱 실패 시 nullopt.
inline std::optional<int> readInt(const std::string& prompt) {
    const std::string line = readLine(prompt);
    try {
        size_t pos = 0;
        int value = std::stoi(line, &pos);
        if (pos != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

// 실수 입력. 파싱 실패 시 nullopt.
inline std::optional<double> readDouble(const std::string& prompt) {
    const std::string line = readLine(prompt);
    try {
        size_t pos = 0;
        double value = std::stod(line, &pos);
        if (pos != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

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
    try {
        size_t pos = 0;
        double value = std::stod(line, &pos);
        if (pos != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

inline std::optional<int> readOptionalInt(const std::string& prompt) {
    const std::string line = readLine(prompt);
    if (line.empty()) {
        return std::nullopt;
    }
    try {
        size_t pos = 0;
        int value = std::stoi(line, &pos);
        if (pos != line.size()) {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

}  // namespace dp::console
