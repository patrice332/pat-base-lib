#pragma once

#include <netinet/in.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <expected>
#include <ostream>
#include <unifex/sender_concepts.hpp>

#include "pat/runtime/context.h"
#include "pat/runtime/getaddrinfo.h"
#include "pat/runtime/promise.h"

namespace pat::net {

namespace internal {

constexpr std::array<std::uint8_t, 12> v4InV6Prefix = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};

}  // namespace internal

using Ip = std::array<std::uint8_t, 16>;

constexpr Ip IPv4(std::uint8_t byte0, std::uint8_t byte1, std::uint8_t byte2, std::uint8_t byte3) {
    return Ip{internal::v4InV6Prefix[0],
              internal::v4InV6Prefix[1],
              internal::v4InV6Prefix[2],
              internal::v4InV6Prefix[3],
              internal::v4InV6Prefix[4],
              internal::v4InV6Prefix[5],
              internal::v4InV6Prefix[6],
              internal::v4InV6Prefix[7],
              internal::v4InV6Prefix[8],
              internal::v4InV6Prefix[9],
              internal::v4InV6Prefix[10],
              internal::v4InV6Prefix[11],
              byte0,
              byte1,
              byte2,
              byte3};
}

constexpr Ip IPv4bcast = IPv4(255, 255, 255, 255);  // limited broadcast
constexpr Ip IPv4allsys = IPv4(224, 0, 0, 1);       // all systems
constexpr Ip IPv4allrouter = IPv4(224, 0, 0, 2);    // all routers
constexpr Ip IPv4zero = IPv4(0, 0, 0, 0);           // all zeros

constexpr bool operator==(const Ip& byte0, const Ip& byte1) {
    return std::equal(byte0.cbegin(), byte0.cend(), byte1.cbegin());
}

constexpr bool operator!=(const Ip& byte0, const Ip& byte1) { return !operator==(byte0, byte1); }

constexpr Ip MakeIp(in_addr addr) {
    const union {
        std::uint32_t i;
        std::array<std::uint8_t, 4> b;
    } foo{.i = addr.s_addr};
    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-union-access)
    return IPv4(foo.b[0], foo.b[1], foo.b[2], foo.b[3]);
}

constexpr Ip MakeIp(in6_addr addr) {
    return {addr.s6_addr[0],  addr.s6_addr[1],  addr.s6_addr[2],  addr.s6_addr[3],
            addr.s6_addr[4],  addr.s6_addr[5],  addr.s6_addr[6],  addr.s6_addr[7],
            addr.s6_addr[8],  addr.s6_addr[9],  addr.s6_addr[10], addr.s6_addr[11],
            addr.s6_addr[12], addr.s6_addr[13], addr.s6_addr[14], addr.s6_addr[15]};
}

static unifex::sender auto LookupIp(std::string host) {
    return runtime::GetIOContext()
        .get_scheduler()
        .schedule()
        .let([host_str = std::move(host)]() {
            struct addrinfo hints {};

            std::memset(&hints, 0, sizeof hints);  // make sure the struct is empty
            hints.ai_family = AF_UNSPEC;           // don't care IPv4 or IPv6
            hints.ai_socktype = 0;                 // Any Sockets
            hints.ai_flags = AI_PASSIVE;           // fill in my IP for me
            return runtime::_getaddrinfo::_sender{runtime::GetIOContext().GetLoop(),
                                                  std::move(host_str), std::string{}, hints};
        })
        .let([](struct addrinfo* addr_info) {
            return runtime::promise(unifex::then(
                unifex::schedule(runtime::GetThreadPool().get_scheduler()), [addr_info]() {
                    std::vector<Ip> rv;
                    for (auto* curr = addr_info; curr != nullptr; curr = curr->ai_next) {
                        if (curr->ai_family == AF_INET) {
                            rv.emplace_back(
                                MakeIp(reinterpret_cast<sockaddr_in*>(curr->ai_addr)->sin_addr));
                        } else if (curr->ai_family == AF_INET6) {
                            rv.emplace_back(
                                MakeIp(reinterpret_cast<sockaddr_in6*>(curr->ai_addr)->sin6_addr));
                        }
                    }
                    uv_freeaddrinfo(addr_info);
                    return rv;
                }));
        });
}

}  // namespace pat::net

namespace std {
ostream& operator<<(ostream&, const pat::net::Ip& ip);
}