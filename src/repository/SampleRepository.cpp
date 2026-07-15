#include "repository/SampleRepository.hpp"

#include <algorithm>

namespace dp {

namespace {

RepositoryResult validate(const std::string& id, const std::string& name, double avgProductionTimeMin,
                           double yieldRate, int stock) {
    if (id.empty()) {
        return RepositoryResult::fail("시료 ID는 비어있을 수 없습니다.");
    }
    if (name.empty()) {
        return RepositoryResult::fail("시료 이름은 비어있을 수 없습니다.");
    }
    if (avgProductionTimeMin <= 0.0) {
        return RepositoryResult::fail("평균 생산시간은 0보다 커야 합니다.");
    }
    if (yieldRate <= 0.0 || yieldRate > 1.0) {
        return RepositoryResult::fail("수율은 0 초과 1 이하 값이어야 합니다.");
    }
    if (stock < 0) {
        return RepositoryResult::fail("재고 수량은 0 이상이어야 합니다.");
    }
    return RepositoryResult::ok("");
}

// id로 항목을 찾는 반복자 조회가 findById/update/remove 3곳에서 반복되어 추출함 (Rule of Three).
std::vector<Sample>::iterator findIt(std::vector<Sample>& samples, const std::string& id) {
    return std::find_if(samples.begin(), samples.end(), [&](const Sample& s) { return s.id == id; });
}

}  // namespace

std::optional<Sample> SampleRepository::findById(const std::string& id) const {
    auto samples = store_.load();
    const auto it = findIt(samples, id);
    if (it == samples.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<Sample> SampleRepository::search(const std::string& keyword) const {
    const auto samples = store_.load();
    std::vector<Sample> result;
    for (const auto& s : samples) {
        if (s.id.find(keyword) != std::string::npos || s.name.find(keyword) != std::string::npos) {
            result.push_back(s);
        }
    }
    return result;
}

RepositoryResult SampleRepository::create(const Sample& sample) {
    auto validation = validate(sample.id, sample.name, sample.avgProductionTimeMin, sample.yieldRate, sample.stock);
    if (!validation.success) {
        return validation;
    }

    auto samples = store_.load();
    const bool duplicate =
        std::any_of(samples.begin(), samples.end(), [&](const Sample& s) { return s.id == sample.id; });
    if (duplicate) {
        return RepositoryResult::fail("이미 존재하는 시료 ID입니다: " + sample.id);
    }

    samples.push_back(sample);
    store_.save(samples);
    return RepositoryResult::ok("시료가 등록되었습니다: " + sample.id);
}

RepositoryResult SampleRepository::update(const std::string& id, const SampleUpdate& patch) {
    auto samples = store_.load();
    auto it = findIt(samples, id);
    if (it == samples.end()) {
        return RepositoryResult::fail("존재하지 않는 시료 ID입니다: " + id);
    }

    Sample updated = *it;
    if (patch.name) updated.name = *patch.name;
    if (patch.avgProductionTimeMin) updated.avgProductionTimeMin = *patch.avgProductionTimeMin;
    if (patch.yieldRate) updated.yieldRate = *patch.yieldRate;
    if (patch.stock) updated.stock = *patch.stock;

    auto validation =
        validate(updated.id, updated.name, updated.avgProductionTimeMin, updated.yieldRate, updated.stock);
    if (!validation.success) {
        return validation;
    }

    *it = updated;
    store_.save(samples);
    return RepositoryResult::ok("시료 정보가 수정되었습니다: " + id);
}

RepositoryResult SampleRepository::remove(const std::string& id) {
    auto samples = store_.load();
    auto it = findIt(samples, id);
    if (it == samples.end()) {
        return RepositoryResult::fail("존재하지 않는 시료 ID입니다: " + id);
    }

    samples.erase(it);
    store_.save(samples);
    return RepositoryResult::ok("시료가 삭제되었습니다: " + id);
}

}  // namespace dp
