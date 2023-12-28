#pragma once

#include "pat/runtime/promise.h"
#include "pat/runtime/stream.h"
#include "pat/runtime/tcp_connect.h"
#include "unifex/sender_concepts.hpp"

namespace pat::runtime {

class TCPSocket final : public Stream<TCPSocket> {
   public:
    static TCPSocket Create(IOContext& context);
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
    TCPSocket(TCPSocket&& other) noexcept;
    TCPSocket& operator=(TCPSocket&& other) noexcept;
    ~TCPSocket() final;

    unifex::sender auto Connect(const struct sockaddr* addr) {
        return promise(_tcp_connect::_sender(&handle_, addr));
    }

   private:
    explicit TCPSocket(IOContext& context);

    uv_stream_t& StreamHandle();
    uv_stream_t const& StreamHandle() const;

    uv_tcp_t handle_{};
};

}  // namespace pat::runtime