#pragma once

#include <optional>
#include <string>
#include <vector>

#include "model/Order.hpp"
#include "repository/RepositoryResult.hpp"
#include "repository/SampleRepository.hpp"
#include "storage/JsonFileStore.hpp"

namespace dp {

// 주문 정보 수정 시 "지정된 필드만 변경"을 표현하기 위한 patch 구조체 (FR-13).
struct OrderUpdate {
    std::optional<std::string> customerName;
    std::optional<int> quantity;
    std::optional<std::string> status;
};

class OrderRepository {
public:
    OrderRepository(std::string dataFilePath, const SampleRepository& sampleRepository)
        : store_(std::move(dataFilePath)), sampleRepository_(sampleRepository) {}

    std::vector<Order> findAll() const { return store_.load(); }

    std::optional<Order> findById(const std::string& orderId) const;

    // 주문번호, 고객명, 시료 ID 중 하나라도 keyword를 포함하면 매치 (FR-12).
    std::vector<Order> search(const std::string& keyword) const;

    RepositoryResult create(const Order& order);

    RepositoryResult update(const std::string& orderId, const OrderUpdate& patch);

    RepositoryResult remove(const std::string& orderId);

private:
    JsonFileStore<Order> store_;
    const SampleRepository& sampleRepository_;
};

}  // namespace dp
