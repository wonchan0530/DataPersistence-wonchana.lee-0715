#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace dp {

// 범용 JSON 파일 저장소.
// T는 nlohmann::json과 to_json/from_json으로 변환 가능해야 한다 (ADL 훅, Phase 0 PoC에서 검증한 패턴).
//
// - load(): 파일이 없으면 빈 벡터를 반환한다 (최초 실행 시 자동 초기화, PRD NFR-3).
// - load(): 파싱에 실패하면 경고를 출력하고 빈 벡터로 대체한다 (PRD NFR-1).
// - save(): 파일이 위치할 디렉터리가 없으면 생성한다.
template <typename T>
class JsonFileStore {
public:
    explicit JsonFileStore(std::string filePath) : filePath_(std::move(filePath)) {}

    std::vector<T> load() const {
        std::ifstream in(filePath_);
        if (!in.is_open()) {
            return {};
        }

        try {
            nlohmann::json arr;
            in >> arr;
            return arr.get<std::vector<T>>();
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "[경고] " << filePath_ << " 파일을 읽는 중 오류가 발생하여 빈 목록으로 대체합니다: "
                      << e.what() << "\n";
            return {};
        }
    }

    void save(const std::vector<T>& items) const {
        std::filesystem::path path(filePath_);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }

        nlohmann::json arr = items;
        std::ofstream out(filePath_);
        out << arr.dump(2);
    }

private:
    std::string filePath_;
};

}  // namespace dp
