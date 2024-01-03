#include "pat/runtime/tcp_socket.h"

#include <gtest/gtest.h>

#include "pat/io/io.h"

namespace pat::runtime {

namespace {

TEST(TCPSocket, asserts) {
    static_assert(io::Reader<TCPSocket>, "File is not an io::Reader");
    static_assert(io::Writer<TCPSocket>, "File is not an io::Writer");
    static_assert(io::Closer<TCPSocket>, "File is not an io::Closer");
}

}  // namespace

}  // namespace pat::runtime