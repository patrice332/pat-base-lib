#pragma once

#include <uv.h>

#include <iostream>
#include <unifex/sender_concepts.hpp>

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

    unifex::sender auto Connect(IOContext& context, const struct sockaddr* addr) {
        handle_.data = this;
        return context.get_scheduler().schedule().let(
            [&context, handle = &handle_, addr]() mutable {
                uv_tcp_init_ex(context.GetLoop(), handle, addr->sa_family);
                reinterpret_cast<TCPSocket*>(handle->data)->init_ = true;
                return promise(_tcp_connect::_sender(handle, addr));
            });
    }

   private:
    uv_stream_t& StreamHandle() final;
    uv_stream_t const& StreamHandle() const final;

    uv_tcp_t handle_{};
    bool init_{false};
};

}  // namespace pat::runtime