#include "pat/runtime/libuv_errors.h"

#include <format>

namespace pat::runtime {

const char* LibUVErrCategoryType::name() const noexcept { return "libuv"; }
std::string LibUVErrCategoryType::message(int error_value) const {
    return std::format("{}: {}", name(), uv_strerror(error_value));
}

}  // namespace pat::runtime