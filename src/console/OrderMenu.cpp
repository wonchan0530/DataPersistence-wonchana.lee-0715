#include "console/OrderMenu.hpp"

#include <iomanip>
#include <iostream>

#include "console/ConsoleIO.hpp"

namespace dp::console {

namespace {

void printOrderRow(const Order& o) {
    std::cout << std::left << std::setw(14) << o.orderId << std::setw(10) << o.sampleId << std::setw(16)
              << o.customerName << "수량=" << o.quantity << "  상태=" << o.status << "\n";
}

}  // namespace

void OrderMenu::run() {
    bool inOrderMenu = true;
    while (inOrderMenu) {
        std::cout << "\n-- 주문 관리 --\n";
        std::cout << "[1] 주문 등록  [2] 주문 조회  [3] 주문 검색  [4] 주문 수정  [5] 주문 삭제  [0] 뒤로\n";
        const std::string choice = readLine("선택 > ");

        if (!std::cin) {
            // 입력 스트림이 종료됨(EOF) - "잘못된 입력"으로 무한 반복하지 않고 상위 메뉴로 복귀한다.
            break;
        }

        if (choice == "1") {
            handleCreate();
        } else if (choice == "2") {
            handleListAll();
        } else if (choice == "3") {
            handleSearch();
        } else if (choice == "4") {
            handleUpdate();
        } else if (choice == "5") {
            handleDelete();
        } else if (choice == "0") {
            inOrderMenu = false;
        } else {
            std::cout << "잘못된 입력입니다.\n";
        }
    }
}

void OrderMenu::handleCreate() {
    Order order;
    order.orderId = readLine("주문번호 > ");
    order.sampleId = readLine("시료 ID > ");
    order.customerName = readLine("고객명 > ");

    const auto quantity = readInt("주문 수량 > ");
    if (!quantity) {
        std::cout << "숫자 입력이 올바르지 않아 등록을 취소합니다.\n";
        return;
    }
    order.quantity = *quantity;
    // order.status는 Order 구조체 기본값(RESERVED)을 그대로 사용한다.

    const auto result = repository_.create(order);
    printResult(result);
}

void OrderMenu::handleListAll() {
    const auto orders = repository_.findAll();
    std::cout << "등록된 주문 (총 " << orders.size() << "건)\n";
    for (const auto& o : orders) {
        printOrderRow(o);
    }
}

void OrderMenu::handleSearch() {
    const std::string keyword = readLine("검색어(주문번호/고객명/시료ID) > ");
    const auto results = repository_.search(keyword);
    std::cout << "검색 결과 " << results.size() << "건\n";
    for (const auto& o : results) {
        printOrderRow(o);
    }
}

void OrderMenu::handleUpdate() {
    const std::string orderId = readLine("수정할 주문번호 > ");
    const auto existing = repository_.findById(orderId);
    if (!existing) {
        std::cout << "존재하지 않는 주문번호입니다: " << orderId << "\n";
        return;
    }

    std::cout << "현재 값: ";
    printOrderRow(*existing);
    std::cout << "변경하지 않을 항목은 입력 없이 Enter를 누르세요.\n";

    OrderUpdate patch;
    patch.customerName = readOptionalLine("고객명 [" + existing->customerName + "] > ");
    patch.quantity = readOptionalInt("수량 [" + std::to_string(existing->quantity) + "] > ");
    patch.status = readOptionalLine("상태 [" + existing->status + "] > ");

    const auto result = repository_.update(orderId, patch);
    printResult(result);
}

void OrderMenu::handleDelete() {
    const std::string orderId = readLine("삭제할 주문번호 > ");
    const auto result = repository_.remove(orderId);
    printResult(result);
}

}  // namespace dp::console
