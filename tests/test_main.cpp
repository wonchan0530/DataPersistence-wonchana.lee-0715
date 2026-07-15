// 경량 자체 테스트 하네스. DESIGN.md Phase 6 참고.
// 외부 프레임워크(GoogleTest 등) 없이 CHECK 매크로 + 성공/실패 카운트만으로 구성한다.

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "model/Order.hpp"
#include "model/Sample.hpp"
#include "repository/OrderRepository.hpp"
#include "repository/SampleRepository.hpp"

namespace {

int g_checks = 0;
int g_failures = 0;

void reportCheck(bool condition, const std::string& expr, const char* file, int line) {
    ++g_checks;
    if (!condition) {
        ++g_failures;
        std::cerr << "[FAIL] " << file << ":" << line << "  CHECK(" << expr << ")\n";
    }
}

}  // namespace

#define CHECK(cond) reportCheck((cond), #cond, __FILE__, __LINE__)

using dp::Order;
using dp::OrderRepository;
using dp::OrderUpdate;
using dp::Sample;
using dp::SampleRepository;
using dp::SampleUpdate;

namespace {

const char* kTestDir = "test_data";

std::string testPath(const std::string& name) {
    return std::string(kTestDir) + "/" + name;
}

void resetFile(const std::string& path) {
    std::filesystem::remove(path);
}

void test_sample_create_and_duplicate() {
    const auto path = testPath("samples_create.json");
    resetFile(path);
    SampleRepository repo(path);

    auto r1 = repo.create(Sample{"S-1", "웨이퍼", 0.5, 0.9, 10});
    CHECK(r1.success);

    auto r2 = repo.create(Sample{"S-1", "다른이름", 0.5, 0.9, 10});
    CHECK(!r2.success);

    CHECK(repo.findAll().size() == 1);
}

void test_sample_not_found_operations() {
    const auto path = testPath("samples_missing.json");
    resetFile(path);
    SampleRepository repo(path);

    CHECK(!repo.findById("NOPE").has_value());

    SampleUpdate patch;
    patch.stock = 5;
    CHECK(!repo.update("NOPE", patch).success);
    CHECK(!repo.remove("NOPE").success);
}

void test_sample_partial_update() {
    const auto path = testPath("samples_update.json");
    resetFile(path);
    SampleRepository repo(path);
    repo.create(Sample{"S-1", "웨이퍼", 0.5, 0.9, 10});

    SampleUpdate patch;
    patch.stock = 99;
    CHECK(repo.update("S-1", patch).success);

    auto updated = repo.findById("S-1");
    CHECK(updated.has_value());
    CHECK(updated->stock == 99);
    CHECK(updated->name == "웨이퍼");  // 변경하지 않은 필드는 유지
}

void test_sample_validation_rejects_invalid_values() {
    const auto path = testPath("samples_invalid.json");
    resetFile(path);
    SampleRepository repo(path);

    CHECK(!repo.create(Sample{"", "웨이퍼", 0.5, 0.9, 10}).success);      // 빈 ID
    CHECK(!repo.create(Sample{"S-1", "웨이퍼", 0.0, 0.9, 10}).success);   // 생산시간 0 이하
    CHECK(!repo.create(Sample{"S-1", "웨이퍼", 0.5, 1.5, 10}).success);   // 수율 범위 초과
    CHECK(!repo.create(Sample{"S-1", "웨이퍼", 0.5, 0.9, -1}).success);   // 음수 재고
    CHECK(repo.findAll().empty());
}

void test_order_referential_integrity() {
    const auto samplePath = testPath("orders_samples.json");
    const auto orderPath = testPath("orders_orders.json");
    resetFile(samplePath);
    resetFile(orderPath);

    SampleRepository sampleRepo(samplePath);
    OrderRepository orderRepo(orderPath, sampleRepo);

    auto missingSampleResult = orderRepo.create(Order{"O-1", "S-NOPE", "고객A", 5, "RESERVED"});
    CHECK(!missingSampleResult.success);
    CHECK(orderRepo.findAll().empty());

    sampleRepo.create(Sample{"S-1", "웨이퍼", 0.5, 0.9, 10});
    CHECK(orderRepo.create(Order{"O-1", "S-1", "고객A", 5, "RESERVED"}).success);

    OrderUpdate patch;
    patch.status = "CONFIRMED";
    CHECK(orderRepo.update("O-1", patch).success);

    auto updated = orderRepo.findById("O-1");
    CHECK(updated.has_value());
    CHECK(updated->status == "CONFIRMED");
    CHECK(updated->quantity == 5);  // 변경하지 않은 필드는 유지
}

void test_persistence_across_instances() {
    const auto path = testPath("persistence_samples.json");
    resetFile(path);

    {
        SampleRepository repo(path);
        repo.create(Sample{"S-1", "웨이퍼", 0.5, 0.9, 10});
    }  // 첫 번째 "실행" 종료 (인스턴스 소멸 - 재시작을 시뮬레이션)

    {
        SampleRepository repo(path);  // 새 "실행"에서 같은 파일을 가리키는 새 인스턴스
        auto samples = repo.findAll();
        CHECK(samples.size() == 1);
        CHECK(samples[0].id == "S-1");
    }
}

void test_missing_file_returns_empty() {
    const auto path = testPath("does_not_exist.json");
    resetFile(path);
    SampleRepository repo(path);
    CHECK(repo.findAll().empty());  // NFR-3: 최초 실행(파일 없음) 시 빈 목록
}

void test_corrupted_file_returns_empty() {
    const auto path = testPath("corrupted_samples.json");
    std::filesystem::create_directories(kTestDir);
    {
        std::ofstream out(path);
        out << "{ this is not valid json ";
    }

    SampleRepository repo(path);
    CHECK(repo.findAll().empty());  // NFR-1: 파싱 실패 시 예외 없이 빈 목록으로 대체
}

}  // namespace

int main() {
    std::filesystem::create_directories(kTestDir);

    test_sample_create_and_duplicate();
    test_sample_not_found_operations();
    test_sample_partial_update();
    test_sample_validation_rejects_invalid_values();
    test_order_referential_integrity();
    test_persistence_across_instances();
    test_missing_file_returns_empty();
    test_corrupted_file_returns_empty();

    std::cout << "\n" << (g_checks - g_failures) << "/" << g_checks << " checks passed\n";
    if (g_failures > 0) {
        std::cout << g_failures << " FAILURE(S)\n";
        return 1;
    }
    std::cout << "ALL TESTS PASSED\n";
    return 0;
}
