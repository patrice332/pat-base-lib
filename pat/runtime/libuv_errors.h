#pragma once

#include <uv.h>

#include <string>
#include <system_error>

namespace std {
template <>
struct is_error_code_enum<uv_errno_t> : true_type {};
}  // namespace std

namespace pat::runtime {

struct LibUVErrCategoryType : std::error_category {
    const char* name() const noexcept override;
    std::string message(int error_value) const override;
};

const LibUVErrCategoryType LibUVErrCategory{};

}  // namespace pat::runtime