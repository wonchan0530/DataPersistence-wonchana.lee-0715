#include <iostream>
#include <string>

#include "console/SampleMenu.hpp"
#include "model/Order.hpp"
#include "model/Sample.hpp"
#include "repository/SampleRepository.hpp"
#include "storage/JsonFileStore.hpp"

namespace {
constexpr const char* kSampleDataFile = "data/samples.json";
constexpr const char* kOrderDataFile = "data/orders.json";
}  // namespace

int main() {
    dp::SampleRepository sampleRepository(kSampleDataFile);
    dp::JsonFileStore<dp::Order> orderStore(kOrderDataFile);

    const auto samples = sampleRepository.findAll();
    const auto orders = orderStore.load();

    std::cout << "==============================================\n";
    std::cout << " 반도체 시료 데이터 영속성 CRUD 콘솔 (PoC 기반)\n";
    std::cout << "==============================================\n";
    std::cout << "등록된 시료 " << samples.size() << "건, 주문 " << orders.size() << "건 로드됨\n\n";

    bool running = true;
    while (running) {
        std::cout << "[1] 시료 관리   [2] 주문 관리   [0] 종료\n";
        std::cout << "선택 > ";

        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (input == "1") {
            dp::console::SampleMenu(sampleRepository).run();
        } else if (input == "2") {
            std::cout << "(주문 관리 메뉴는 Phase 4에서 구현 예정)\n\n";
        } else if (input == "0") {
            running = false;
        } else {
            std::cout << "잘못된 입력입니다. 다시 선택해주세요.\n\n";
        }
    }

    std::cout << "종료합니다.\n";
    return 0;
}
