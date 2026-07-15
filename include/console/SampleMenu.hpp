#pragma once

#include "repository/SampleRepository.hpp"

namespace dp::console {

// 시료 관리 메뉴(등록/조회/검색/수정/삭제) 콘솔 컨트롤러.
class SampleMenu {
public:
    explicit SampleMenu(SampleRepository& repository) : repository_(repository) {}

    void run();

private:
    void handleCreate();
    void handleListAll();
    void handleSearch();
    void handleUpdate();
    void handleDelete();

    SampleRepository& repository_;
};

}  // namespace dp::console
