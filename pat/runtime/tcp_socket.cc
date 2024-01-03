#include "pat/runtime/tcp_socket.h"

#include "pat/runtime/stream.h"

namespace pat::runtime {

namespace {
constexpr std::size_t UV_HANDLE_CLOSED = 0x00000002;
}

TCPSocket::TCPSocket() = default;

TCPSocket::TCPSocket(TCPSocket&& other) noexcept {
    std::memcpy(&handle_, &other.handle_, sizeof(handle_));
    std::memset(&other.handle_, 0, sizeof(other.handle_));
    other.handle_.flags = UV_HANDLE_CLOSED;
}

/*
TCPSocket& TCPSocket::operator=(TCPSocket&& other) noexcept {
    if (this != &other) {
        if (!static_cast<bool>(uv_is_closing(reinterpret_cast<uv_handle_t*>(&StreamHandle())))) {
            std::terminate();
        }
        handle_ = other.handle_;
        std::memset(&other.handle_, 0, sizeof(other.handle_));
        other.handle_.flags = UV_HANDLE_CLOSED;
    }
    return *this;
}
*/
TCPSocket::~TCPSocket() {
    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
    if (!static_cast<bool>(uv_is_closing(reinterpret_cast<uv_handle_t*>(&StreamHandle())))) {
        std::terminate();
    }
}

// trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
uv_stream_t& TCPSocket::StreamHandle() { return *reinterpret_cast<uv_stream_t*>(&handle_); }
uv_stream_t const& TCPSocket::StreamHandle() const {
    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
    return *reinterpret_cast<uv_stream_t const*>(&handle_);
}

}  // namespace pat::runtime