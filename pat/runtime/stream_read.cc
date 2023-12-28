#include "pat/runtime/stream_read.h"

namespace pat::runtime::_stream_read {
_sender::_sender(uv_stream_t *stream_handle, std::span<char> msg)
    : stream_handle_{stream_handle}, msg_{msg} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_stream_read