#include "pat/runtime/tcp_socket.h"

#include "pat/io/io.h"
#include "pat/runtime/stream.h"

namespace pat::runtime {

TCPSocket::TCPSocket(IOContext& context) : Stream<TCPSocket>(context) {}
TCPSocket::TCPSocket(TCPSocket&& other) noexcept = default;
TCPSocket& TCPSocket::operator=(TCPSocket&& other) noexcept = default;
TCPSocket::~TCPSocket() = default;

TCPSocket TCPSocket::Create(IOContext& context) {
    TCPSocket return_value{context};
    uv_tcp_init(return_value.Loop(), &return_value.handle_);
    return return_value;
}

uv_stream_t& TCPSocket::StreamHandle() { return *reinterpret_cast<uv_stream_t*>(&handle_); }
uv_stream_t const& TCPSocket::StreamHandle() const {
    return *reinterpret_cast<uv_stream_t const*>(&handle_);
}

static_assert(io::Reader<TCPSocket>, "File is not an io::Reader");
static_assert(io::Writer<TCPSocket>, "File is not an io::Writer");
static_assert(io::Closer<TCPSocket>, "File is not an io::Closer");

}  // namespace pat::runtime