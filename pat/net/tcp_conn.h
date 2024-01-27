#pragma once

#include <expected>
#include <unifex/sender_concepts.hpp>

#include "pat/net/ipaddr.h"
#include "pat/runtime/tcp_socket.h"

namespace pat::net {

class TcpConn;

unifex::sender auto DialTcp(IpAddr const& laddr, IpAddr const& raddr);

class TcpConn {
   public:
   private:
    runtime::TCPSocket socket_;
};

}  // namespace pat::net