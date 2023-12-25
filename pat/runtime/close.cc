#include "pat/runtime/close.h"

namespace pat::runtime::_close {
_sender::_sender(uv_loop_t *loop, int file_descriptor)
    : loop_{loop}, file_descriptor_{file_descriptor} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_close