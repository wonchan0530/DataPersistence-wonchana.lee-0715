// PoC: nlohmann/json 라이브러리를 이용한 구조체 <-> JSON 파일 저장/로드 검증
// 본 프로젝트 소스와는 완전히 분리된 독립 실험 코드이다. (DESIGN.md Phase 0 참고)

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../third_party/nlohmann/json.hpp"

using json = nlohmann::json;

struct DummySample {
    std::string id;
    std::string name;
    double avgProductionTimeMin;
    double yieldRate;
    int stock;
};

// ADL 훅: DummySample <-> json 변환
void to_json(json& j, const DummySample& s) {
    j = json{
        {"id", s.id},
        {"name", s.name},
        {"avgProductionTimeMin", s.avgProductionTimeMin},
        {"yieldRate", s.yieldRate},
        {"stock", s.stock},
    };
}

void from_json(const json& j, DummySample& s) {
    j.at("id").get_to(s.id);
    j.at("name").get_to(s.name);
    j.at("avgProductionTimeMin").get_to(s.avgProductionTimeMin);
    j.at("yieldRate").get_to(s.yieldRate);
    j.at("stock").get_to(s.stock);
}

const char* kDataFile = "poc_samples.json";

std::vector<DummySample> loadSamples() {
    std::ifstream in(kDataFile);
    if (!in.is_open()) {
        std::cout << "[PoC] 데이터 파일이 없어 빈 목록으로 시작합니다.\n";
        return {};
    }

    try {
        json arr;
        in >> arr;
        return arr.get<std::vector<DummySample>>();
    } catch (const json::parse_error& e) {
        std::cout << "[PoC] JSON 파싱 실패, 빈 목록으로 시작합니다: " << e.what() << "\n";
        return {};
    }
}

void saveSamples(const std::vector<DummySample>& samples) {
    json arr = samples;
    std::ofstream out(kDataFile);
    out << arr.dump(2);
}

int main() {
    std::vector<DummySample> samples = loadSamples();

    std::cout << "[PoC] 로드된 시료 개수: " << samples.size() << "\n";
    for (const auto& s : samples) {
        std::cout << "  - " << s.id << " / " << s.name << " / stock=" << s.stock << "\n";
    }

    // 실행할 때마다 더미 시료 하나를 추가해 영속성(재실행 후 누적)을 눈으로 확인한다.
    DummySample newSample{
        "S-" + std::to_string(samples.size() + 1),
        "PoC 웨이퍼",
        0.5,
        0.9,
        100,
    };
    samples.push_back(newSample);
    saveSamples(samples);

    std::cout << "[PoC] 신규 시료 추가 후 저장 완료. 총 " << samples.size() << "개\n";
    std::cout << "[PoC] 다시 실행하면 누적된 개수를 확인할 수 있습니다.\n";
    return 0;
}
