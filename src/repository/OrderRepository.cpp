#include "repository/OrderRepository.hpp"

#include <algorithm>

namespace dp {

namespace {

RepositoryResult validateCommon(const std::string& orderId, const std::string& customerName, int quantity) {
    if (orderId.empty()) {
        return RepositoryResult::fail("주문번호는 비어있을 수 없습니다.");
    }
    if (customerName.empty()) {
        return RepositoryResult::fail("고객명은 비어있을 수 없습니다.");
    }
    if (quantity <= 0) {
        return RepositoryResult::fail("주문 수량은 0보다 커야 합니다.");
    }
    return RepositoryResult::ok("");
}

}  // namespace

std::optional<Order> OrderRepository::findById(const std::string& orderId) const {
    const auto orders = store_.load();
    const auto it =
        std::find_if(orders.begin(), orders.end(), [&](const Order& o) { return o.orderId == orderId; });
    if (it == orders.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<Order> OrderRepository::search(const std::string& keyword) const {
    const auto orders = store_.load();
    std::vector<Order> result;
    for (const auto& o : orders) {
        if (o.orderId.find(keyword) != std::string::npos || o.customerName.find(keyword) != std::string::npos ||
            o.sampleId.find(keyword) != std::string::npos) {
            result.push_back(o);
        }
    }
    return result;
}

RepositoryResult OrderRepository::create(const Order& order) {
    auto validation = validateCommon(order.orderId, order.customerName, order.quantity);
    if (!validation.success) {
        return validation;
    }

    if (!sampleRepository_.findById(order.sampleId)) {
        return RepositoryResult::fail("존재하지 않는 시료 ID입니다: " + order.sampleId);
    }

    auto orders = store_.load();
    const bool duplicate =
        std::any_of(orders.begin(), orders.end(), [&](const Order& o) { return o.orderId == order.orderId; });
    if (duplicate) {
        return RepositoryResult::fail("이미 존재하는 주문번호입니다: " + order.orderId);
    }

    orders.push_back(order);
    store_.save(orders);
    return RepositoryResult::ok("주문이 접수되었습니다: " + order.orderId);
}

RepositoryResult OrderRepository::update(const std::string& orderId, const OrderUpdate& patch) {
    auto orders = store_.load();
    auto it = std::find_if(orders.begin(), orders.end(), [&](const Order& o) { return o.orderId == orderId; });
    if (it == orders.end()) {
        return RepositoryResult::fail("존재하지 않는 주문번호입니다: " + orderId);
    }

    Order updated = *it;
    if (patch.customerName) updated.customerName = *patch.customerName;
    if (patch.quantity) updated.quantity = *patch.quantity;
    if (patch.status) updated.status = *patch.status;

    auto validation = validateCommon(updated.orderId, updated.customerName, updated.quantity);
    if (!validation.success) {
        return validation;
    }

    *it = updated;
    store_.save(orders);
    return RepositoryResult::ok("주문 정보가 수정되었습니다: " + orderId);
}

RepositoryResult OrderRepository::remove(const std::string& orderId) {
    auto orders = store_.load();
    auto it = std::find_if(orders.begin(), orders.end(), [&](const Order& o) { return o.orderId == orderId; });
    if (it == orders.end()) {
        return RepositoryResult::fail("존재하지 않는 주문번호입니다: " + orderId);
    }

    orders.erase(it);
    store_.save(orders);
    return RepositoryResult::ok("주문이 삭제되었습니다: " + orderId);
}

}  // namespace dp
