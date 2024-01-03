#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h> /* To define `struct in_addr'.  */

#include <cstddef>
#include <exception>
#include <iostream>
#include <span>
#include <string_view>
#include <system_error>
#include <thread>
#include <unifex/any_sender_of.hpp>
#include <unifex/sender_concepts.hpp>
#include <vector>

#include "pat/io/io.h"
#include "pat/runtime/file.h"
#include "pat/runtime/file_fwd.h"
#include "pat/runtime/getaddrinfo.h"
#include "pat/runtime/io_context.h"
#include "pat/runtime/promise.h"
#include "pat/runtime/tcp_socket.h"
#include "unifex/just_done.hpp"
#include "unifex/just_error.hpp"
#include "unifex/let_value_with.hpp"
#include "unifex/repeat_effect_until.hpp"
#include "unifex/sync_wait.hpp"

int main() {
    pat::runtime::IOContext io_context;

    constexpr std::string_view msg = "This is a test\n";
    std::array<char, msg.size()> buf = {};

    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

    try {
        auto waitable =
            pat::runtime::File::Open(io_context, "/tmp/test.out",
                                     UV_FS_O_CREAT | UV_FS_O_APPEND | UV_FS_O_WRONLY,
                                     S_IRUSR | S_IWUSR)
                .let([&io_context, &msg](const pat::runtime::File& file) {
                    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                    return file.Write(io_context, msg)
                        .let([&io_context, &file](std::size_t wrote_bytes) {
                            std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                            std::cout << "Wrote " << wrote_bytes << std::endl;
                            std::cout << "FD is " << file.Descriptor() << std::endl;
                            return file.Close(io_context);
                        });
                })
                .let([&io_context]() {
                    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                    return pat::runtime::File::Open(io_context, "/tmp/test.out", UV_FS_O_RDONLY);
                })
                .let([&io_context, &buf](const pat::runtime::File& file) {
                    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                    return file.Read(io_context, buf)
                        .let([&io_context, &file, &buf](std::size_t read_bytes) {
                            std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                            std::cout << "Read " << read_bytes << std::endl;
                            std::cout << "FD is " << file.Descriptor() << std::endl;
                            std::cout << "Buf is " << std::string_view{buf.data(), read_bytes}
                                      << std::endl;
                            return file.Close(io_context);
                        });
                });
        std::cout << "Size of waitable: " << sizeof(waitable) << std::endl;
        unifex::sync_wait(std::move(waitable));

        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

        std::cout << "Network test" << std::endl;

        struct addrinfo hints {};

        std::memset(&hints, 0, sizeof hints);  // make sure the struct is empty
        hints.ai_family = AF_UNSPEC;           // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;       // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;           // fill in my IP for me

        auto glb_snd = unifex::let_value_with(
            []() { return pat::runtime::TCPSocket{}; },
            [&io_context, &hints](pat::runtime::TCPSocket& socket) {
                return io_context.get_scheduler()
                    .schedule()
                    .let([&io_context, &hints, &socket]() {
                        return pat::runtime::promise(
                                   pat::runtime::_getaddrinfo::_sender{io_context.GetLoop(),
                                                                       "localhost", "9000", hints})
                            .let([&io_context, &socket](addrinfo* addr) {
                                return socket.Connect(io_context, addr->ai_addr).then([addr]() {
                                    uv_freeaddrinfo(addr);
                                });
                            })
                            .let([&socket]() {
                                constexpr std::string_view kMsg =
                                    "GET / HTTP/1.1\r\nHost: localhost:9000\r\nAccept: */*\r\n\r\n";
                                return socket.Write(kMsg);
                            })
                            .let([&socket](auto&&) {
                                return unifex::let_value_with(
                                    []() { return std::vector<char>{}; },
                                    [&socket](std::vector<char>& read_buf) {
                                        read_buf.resize(65536);
                                        return socket.Read(read_buf)
                                            .then([&read_buf](std::size_t bytes_read) {
                                                std::cout
                                                    << "Read:\n"
                                                    << std::string_view{std::span{read_buf}.subspan(
                                                           0, bytes_read)}
                                                    << std::endl;
                                            })
                                            .let([&socket, &read_buf]() {
                                                return socket.Read(read_buf);
                                            })
                                            .then([&read_buf](std::size_t bytes_read) {
                                                std::cout
                                                    << "Read:\n"
                                                    << std::string_view{std::span{read_buf}.subspan(
                                                           0, bytes_read)}
                                                    << std::endl;
                                            });
                                    });
                            });
                    })
                    .let([&socket]() { return socket.Close(); })
                    .let_error([&socket](auto&& err) {
                        return socket.Close().then([&err]() { std::rethrow_exception(err); });
                    });
            });

        unifex::sync_wait(glb_snd);

    } catch (const std::error_code& ec) {
        std::cout << "Caught error_code: " << ec.message() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected throw in main" << std::endl;
    }

    return 0;
}