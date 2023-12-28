#include "pat/runtime/tcp_connect.h"

namespace pat::runtime::_tcp_connect {
_sender::_sender(uv_tcp_t *handle, const struct sockaddr *addr) : handle_{handle}, addr_{addr} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_tcp_connect