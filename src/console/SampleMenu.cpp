#include "console/SampleMenu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "console/ConsoleIO.hpp"

namespace dp::console {

namespace {

void printSampleRow(const Sample& s) {
    std::cout << std::left << std::setw(10) << s.id << std::setw(20) << s.name << "평균생산시간="
              << s.avgProductionTimeMin << "min  수율=" << s.yieldRate << "  재고=" << s.stock << "\n";
}

// std::to_string(double)은 "0.500000"처럼 후행 0이 많아 프롬프트에 그대로 쓰기엔 지저분하다.
// printSampleRow와 동일한 기본 스트림 포맷(예: "0.5")으로 맞춰서 표시한다.
std::string toDisplay(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

}  // namespace

void SampleMenu::run() {
    bool inSampleMenu = true;
    while (inSampleMenu) {
        std::cout << "\n-- 시료 관리 --\n";
        std::cout << "[1] 시료 등록  [2] 시료 조회  [3] 시료 검색  [4] 시료 수정  [5] 시료 삭제  [0] 뒤로\n";
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
            inSampleMenu = false;
        } else {
            std::cout << "잘못된 입력입니다.\n";
        }
    }
}

void SampleMenu::handleCreate() {
    Sample sample;
    sample.id = readLine("시료 ID > ");
    sample.name = readLine("이름 > ");

    const auto avgTime = readDouble("평균 생산시간(min/ea) > ");
    const auto yield = readDouble("수율(0~1) > ");
    const auto stock = readInt("초기 재고 > ");

    if (!avgTime || !yield || !stock) {
        std::cout << "숫자 입력이 올바르지 않아 등록을 취소합니다.\n";
        return;
    }

    sample.avgProductionTimeMin = *avgTime;
    sample.yieldRate = *yield;
    sample.stock = *stock;

    const auto result = repository_.create(sample);
    printResult(result);
}

void SampleMenu::handleListAll() {
    const auto samples = repository_.findAll();
    std::cout << "등록된 시료 (총 " << samples.size() << "종)\n";
    for (const auto& s : samples) {
        printSampleRow(s);
    }
}

void SampleMenu::handleSearch() {
    const std::string keyword = readLine("검색어(ID/이름) > ");
    const auto results = repository_.search(keyword);
    std::cout << "검색 결과 " << results.size() << "건\n";
    for (const auto& s : results) {
        printSampleRow(s);
    }
}

void SampleMenu::handleUpdate() {
    const std::string id = readLine("수정할 시료 ID > ");
    const auto existing = repository_.findById(id);
    if (!existing) {
        std::cout << "존재하지 않는 시료 ID입니다: " << id << "\n";
        return;
    }

    std::cout << "현재 값: ";
    printSampleRow(*existing);
    std::cout << "변경하지 않을 항목은 입력 없이 Enter를 누르세요.\n";

    SampleUpdate patch;
    patch.name = readOptionalLine("이름 [" + existing->name + "] > ");
    patch.avgProductionTimeMin =
        readOptionalDouble("평균 생산시간 [" + toDisplay(existing->avgProductionTimeMin) + "] > ");
    patch.yieldRate = readOptionalDouble("수율 [" + toDisplay(existing->yieldRate) + "] > ");
    patch.stock = readOptionalInt("재고 [" + std::to_string(existing->stock) + "] > ");

    const auto result = repository_.update(id, patch);
    printResult(result);
}

void SampleMenu::handleDelete() {
    const std::string id = readLine("삭제할 시료 ID > ");
    const auto result = repository_.remove(id);
    printResult(result);
}

}  // namespace dp::console
