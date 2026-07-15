#pragma once

#include <string>

#include "nlohmann/json.hpp"

namespace dp {

// 주문(Order) 데이터 모델.
// 필드 정의 근거: docs/[CRA_AI] Day3_개인과제_반도체시료관리.pdf 14페이지 "시료 주문".
struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity = 0;
    std::string status = "RESERVED";
};

inline void to_json(nlohmann::json& j, const Order& o) {
    j = nlohmann::json{
        {"orderId", o.orderId},
        {"sampleId", o.sampleId},
        {"customerName", o.customerName},
        {"quantity", o.quantity},
        {"status", o.status},
    };
}

inline void from_json(const nlohmann::json& j, Order& o) {
    j.at("orderId").get_to(o.orderId);
    j.at("sampleId").get_to(o.sampleId);
    j.at("customerName").get_to(o.customerName);
    j.at("quantity").get_to(o.quantity);
    j.at("status").get_to(o.status);
}

}  // namespace dp
