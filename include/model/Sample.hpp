#pragma once

#include <string>

#include "nlohmann/json.hpp"

namespace dp {

// 시료(Sample) 데이터 모델.
// 필드 정의 근거: docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf 12페이지 "시료 관리".
struct Sample {
    std::string id;
    std::string name;
    double avgProductionTimeMin = 0.0;
    double yieldRate = 0.0;
    int stock = 0;
};

inline void to_json(nlohmann::json& j, const Sample& s) {
    j = nlohmann::json{
        {"id", s.id},
        {"name", s.name},
        {"avgProductionTimeMin", s.avgProductionTimeMin},
        {"yieldRate", s.yieldRate},
        {"stock", s.stock},
    };
}

inline void from_json(const nlohmann::json& j, Sample& s) {
    j.at("id").get_to(s.id);
    j.at("name").get_to(s.name);
    j.at("avgProductionTimeMin").get_to(s.avgProductionTimeMin);
    j.at("yieldRate").get_to(s.yieldRate);
    j.at("stock").get_to(s.stock);
}

}  // namespace dp
