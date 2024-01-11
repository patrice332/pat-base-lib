#pragma once

#include <uv.h>

#include <iostream>
#include <unifex/sender_concepts.hpp>

#include "pat/net/ip.h"
#include "pat/runtime/promise.h"
#include "pat/runtime/stream.h"
#include "pat/runtime/tcp_connect.h"

namespace pat::runtime {

class TCPSocket final : public Stream {
   public:
    TCPSocket();

    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
    TCPSocket(TCPSocket&& other) noexcept;
    TCPSocket& operator=(TCPSocket&& other) noexcept = delete;
    ~TCPSocket() final;

    unifex::sender auto Connect(IOContext& context, net::Ip ip_addr, std::uint16_t port) {
        handle_.data = this;
        return context.get_scheduler().schedule().let(
            [&context, handle = &handle_, ip_addr, port]() mutable {
                if (std::equal(net::internal::v4InV6Prefix.begin(),
                               net::internal::v4InV6Prefix.end(), ip_addr.begin())) {
                    uv_tcp_init_ex(context.GetLoop(), handle, AF_INET);
                } else {
                    uv_tcp_init_ex(context.GetLoop(), handle, AF_INET6);
                }
                reinterpret_cast<TCPSocket*>(handle->data)->init_ = true;
                return promise(_tcp_connect::_sender(handle, ip_addr, port));
            });
    }

   private:
    uv_stream_t& StreamHandle() final;
    uv_stream_t const& StreamHandle() const final;

    uv_tcp_t handle_{};
    bool init_{false};
};

}  // namespace pat::runtime