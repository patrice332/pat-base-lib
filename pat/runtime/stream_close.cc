#include "pat/runtime/stream_close.h"

namespace pat::runtime::_stream_close {
_sender::_sender(uv_stream_t *stream_handle) : stream_handle_{stream_handle} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_stream_close