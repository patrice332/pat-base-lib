#pragma once

#include <uv.h>

#include <cstring>
#include <system_error>
#include <unifex/blocking.hpp>
#include <unifex/receiver_concepts.hpp>

#include "pat/net/ip.h"
#include "pat/runtime/libuv_errors.h"

namespace pat::runtime::_tcp_connect {

template <unifex::receiver_of<> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_tcp_t *handle, net::Ip ip_addr, std::uint16_t port)
        : rec_(std::move(rec)), handle_{handle}, ip_{ip_addr}, port_{port} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        connect_op_.data = this;
        if (std::equal(net::internal::v4InV6Prefix.begin(), net::internal::v4InV6Prefix.end(),
                       ip_.begin())) {
            // IPv4
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port_);
            __builtin_memcpy(&addr.sin_addr.s_addr, ip_.data() + 12, sizeof(addr.sin_addr.s_addr));
            uv_tcp_connect(&connect_op_, handle_, reinterpret_cast<sockaddr *>(&addr),
                           &tcp_connect_cb);
            return;
        }
        // IPv6
        sockaddr_in6 addr{};
        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(port_);
        __builtin_memcpy(addr.sin6_addr.s6_addr, ip_.data(), sizeof(addr.sin6_addr.s6_addr));
        uv_tcp_connect(&connect_op_, handle_, reinterpret_cast<sockaddr *>(&addr), &tcp_connect_cb);
    }

   private:
    static void tcp_connect_cb(uv_connect_t *req, int status) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        auto *operation = reinterpret_cast<_op<Receiver> *>(req->data);
        if (status < 0) {
            unifex::set_error(std::move(operation->rec_),
                              std::make_exception_ptr(std::system_error(
                                  std::error_code(static_cast<int>(status), LibUVErrCategory))));
            return;
        }

        std::move(operation->rec_).set_value();
    }

    Receiver rec_{};
    uv_tcp_t *handle_{nullptr};
    net::Ip ip_;
    std::uint16_t port_;
    uv_connect_t connect_op_{};
};

class _sender {
   public:
    _sender(uv_tcp_t *handle, net::Ip ip_addr, std::uint16_t port);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), handle_, ip_, port_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_tcp_t *handle_;
    net::Ip ip_;
    std::uint16_t port_;
};

}  // namespace pat::runtime::_tcp_connect