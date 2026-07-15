#pragma once

#include <optional>
#include <string>
#include <vector>

#include "model/Sample.hpp"
#include "repository/RepositoryResult.hpp"
#include "storage/JsonFileStore.hpp"

namespace dp {

// 시료 정보 수정 시 "지정된 필드만 변경"을 표현하기 위한 patch 구조체 (FR-8).
struct SampleUpdate {
    std::optional<std::string> name;
    std::optional<double> avgProductionTimeMin;
    std::optional<double> yieldRate;
    std::optional<int> stock;
};

class SampleRepository {
public:
    explicit SampleRepository(std::string dataFilePath) : store_(std::move(dataFilePath)) {}

    std::vector<Sample> findAll() const { return store_.load(); }

    std::optional<Sample> findById(const std::string& id) const;

    // id 또는 name에 keyword가 부분 일치하면 검색 결과에 포함한다.
    std::vector<Sample> search(const std::string& keyword) const;

    RepositoryResult create(const Sample& sample);

    RepositoryResult update(const std::string& id, const SampleUpdate& patch);

    RepositoryResult remove(const std::string& id);

private:
    JsonFileStore<Sample> store_;
};

}  // namespace dp
