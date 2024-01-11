#include "pat/net/ip.h"

#include <span>

namespace pat::net {
namespace {

void PrintIp(std::ostream& os, const Ip& ip) {
    if (std::equal(internal::v4InV6Prefix.begin(), internal::v4InV6Prefix.end(), ip.begin())) {
        std::array<char, INET_ADDRSTRLEN> ipstr;
        struct in_addr ipv4_addr;
        union {
            std::uint32_t i;
            std::array<std::uint8_t, 4> b;
        } foo{.i = 0};
        std::copy(ip.data() + 12, ip.data() + ip.size(), foo.b.begin());
        ipv4_addr.s_addr = foo.i;
        inet_ntop(AF_INET, &ipv4_addr, ipstr.data(), ipstr.size());
        os << ipstr.data();
        return;
    }
    std::array<char, INET6_ADDRSTRLEN> ipstr;
    struct in6_addr ipv6_addr;
    std::copy(ip.begin(), ip.end(), ipv6_addr.s6_addr);
    inet_ntop(AF_INET6, &ipv6_addr, ipstr.data(), ipstr.size());
    os << ipstr.data();
}

}  // namespace
}  // namespace pat::net

namespace std {
ostream& operator<<(ostream& os, const pat::net::Ip& ip) {
    pat::net::PrintIp(os, ip);
    return os;
}
}  // namespace std