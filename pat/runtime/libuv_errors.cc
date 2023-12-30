#include "pat/runtime/libuv_errors.h"

#include <format>

namespace pat::runtime {

const char* LibUVErrCategoryType::name() const noexcept { return "libuv"; }
std::string LibUVErrCategoryType::message(int ev) const {
    return std::format("{}: {}", name(), uv_strerror(ev));
}

}  // namespace pat::runtime