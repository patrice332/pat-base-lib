#include <fcntl.h>

#include <exception>
#include <iostream>
#include <thread>
#include <unifex/any_sender_of.hpp>

#include "pat/runtime/file.h"
#include "pat/runtime/file_fwd.h"
#include "pat/runtime/getaddrinfo.h"
#include "pat/runtime/io_context.h"
#include "unifex/sync_wait.hpp"

int main() {
    auto io_context = pat::runtime::IOContext::Create();

    constexpr std::string_view msg = "This is a test\n";
    std::array<char, msg.size()> buf{};

    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

    try {
        unifex::sync_wait(
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
                }));

        std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

        struct addrinfo hints {};

        std::memset(&hints, 0, sizeof hints);  // make sure the struct is empty
        hints.ai_family = AF_UNSPEC;           // don't care IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;       // TCP stream sockets
        hints.ai_flags = AI_PASSIVE;           // fill in my IP for me

        pat::runtime::_getaddrinfo::_sender get_add_info_snd{io_context.GetLoop(),
                                                             "www.example.net", "3490", hints};

        auto res_gai = unifex::sync_wait(std::move(get_add_info_snd));
        if (res_gai) {
            auto* res = res_gai.value();
            for (auto* pointer = res; pointer != nullptr; pointer = pointer->ai_next) {
                void* addr = nullptr;
                std::string ipver;
                std::array<char, INET6_ADDRSTRLEN> ipstr{};

                // get the pointer to the address itself,
                // different fields in IPv4 and IPv6:
                if (pointer->ai_family == AF_INET) {  // IPv4
                    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                    auto* ipv4 = reinterpret_cast<struct sockaddr_in*>(pointer->ai_addr);
                    addr = &(ipv4->sin_addr);
                    ipver = "IPv4";
                } else {  // IPv6
                    // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                    auto* ipv6 = reinterpret_cast<struct sockaddr_in6*>(pointer->ai_addr);
                    addr = &(ipv6->sin6_addr);
                    ipver = "IPv6";
                }

                // convert the IP to a string and print it:
                inet_ntop(pointer->ai_family, addr, ipstr.data(), ipstr.size());
                printf("  %s: %s\n", ipver.c_str(), ipstr.data());
            }
        }
    } catch (const std::error_code& ec) {
        std::cout << "Caught error_code: " << ec.message() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected throw in main" << std::endl;
    }

    return 0;
}