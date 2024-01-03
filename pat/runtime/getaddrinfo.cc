#include "pat/runtime/getaddrinfo.h"

namespace pat::runtime::_getaddrinfo {
_sender::_sender(uv_loop_t *loop, std::string node, std::string service, struct addrinfo hints)
    : loop_{loop}, node_{std::move(node)}, service_{std::move(service)}, hints_{hints} {}
_sender::_sender(_sender &&other) noexcept = default;
_sender &_sender::operator=(_sender &&other) noexcept = default;
_sender::~_sender() = default;

}  // namespace pat::runtime::_getaddrinfo