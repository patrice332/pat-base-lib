#include "pat/runtime/read.h"

namespace pat::runtime::_read {
_sender::_sender(uv_loop_t *loop, int file_descriptor, std::span<char> msg)
    : loop_{loop}, file_descriptor_{file_descriptor}, msg_{msg} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_read