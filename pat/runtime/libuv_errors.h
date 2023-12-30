#pragma once

#include <string>
#include <system_error>

#include "uv.h"

namespace std {
template <>
struct is_error_code_enum<uv_errno_t> : true_type {};
}  // namespace std

namespace pat::runtime {

struct LibUVErrCategoryType : std::error_category {
    const char* name() const noexcept override;
    std::string message(int ev) const override;
};

const LibUVErrCategoryType LibUVErrCategory{};

}  // namespace pat::runtime