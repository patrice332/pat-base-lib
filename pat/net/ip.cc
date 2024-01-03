#include "pat/net/ip.h"

#include <netinet/in.h>

#include <cstdint>

namespace pat::net {

Ip MakeIp(in_addr addr) {
    const union {
        std::uint32_t i;
        std::array<std::uint8_t, 4> b;
    } foo{.i = addr.s_addr};
    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access)
    return IPv4(foo.b[0], foo.b[1], foo.b[2], foo.b[3]);
}

Ip MakeIp(in6_addr addr) {
    return {addr.s6_addr[0],  addr.s6_addr[1],  addr.s6_addr[2],  addr.s6_addr[3],
            addr.s6_addr[4],  addr.s6_addr[5],  addr.s6_addr[6],  addr.s6_addr[7],
            addr.s6_addr[8],  addr.s6_addr[9],  addr.s6_addr[10], addr.s6_addr[11],
            addr.s6_addr[12], addr.s6_addr[13], addr.s6_addr[14], addr.s6_addr[15]};
}

}  // namespace pat::net