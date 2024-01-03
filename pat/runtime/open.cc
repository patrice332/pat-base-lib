#include "pat/runtime/open.h"

namespace pat::runtime::_open {
// trunk-ignore(clang-tidy/bugprone-easily-swappable-parameters)
_sender::_sender(uv_loop_t *loop, std::string path, int flags, int mode)
    : loop_{loop}, path_{std::move(path)}, flags_{flags}, mode_{mode} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept {
    if (this != &other) {
        loop_ = other.loop_;
        other.loop_ = nullptr;
        path_ = std::move(other.path_);
        flags_ = other.flags_;
        mode_ = other.mode_;
    }
    return *this;
}
_sender::~_sender() = default;

}  // namespace pat::runtime::_open