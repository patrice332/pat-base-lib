#pragma once

#include <cstdint>

#include "pat/net/ip.h"

namespace pat::net {

struct IpAddr {
    Ip ip_addr;
    std::uint16_t port;
};

}  // namespace pat::net