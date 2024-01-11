#include "pat/runtime/tcp_connect.h"

namespace pat::runtime::_tcp_connect {
_sender::_sender(uv_tcp_t *handle, net::Ip ip_addr, std::uint16_t port)
    : handle_{handle}, ip_{ip_addr}, port_{port} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_tcp_connect