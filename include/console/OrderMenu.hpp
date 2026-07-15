#pragma once

#include "repository/OrderRepository.hpp"

namespace dp::console {

// 주문 관리 메뉴(등록/조회/검색/수정/삭제) 콘솔 컨트롤러.
class OrderMenu {
public:
    explicit OrderMenu(OrderRepository& repository) : repository_(repository) {}

    void run();

private:
    void handleCreate();
    void handleListAll();
    void handleSearch();
    void handleUpdate();
    void handleDelete();

    OrderRepository& repository_;
};

}  // namespace dp::console
