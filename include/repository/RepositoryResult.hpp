#pragma once

#include <string>

namespace dp {

// Repository 계층의 Create/Update/Delete 결과.
// Console 계층은 message를 그대로 출력하기만 하면 되도록 하여 도메인 규칙과 View를 분리한다.
struct RepositoryResult {
    bool success;
    std::string message;

    static RepositoryResult ok(std::string message) {
        return RepositoryResult{true, std::move(message)};
    }

    static RepositoryResult fail(std::string message) {
        return RepositoryResult{false, std::move(message)};
    }
};

}  // namespace dp
