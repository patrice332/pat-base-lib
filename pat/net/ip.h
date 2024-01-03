#pragma once

#include <netinet/in.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <unifex/sender_concepts.hpp>

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

Ip MakeIp(in_addr addr);
Ip MakeIp(in6_addr addr);

}  // namespace pat::net