#include <fcntl.h>

#include <exception>
#include <iostream>
#include <thread>
#include <unifex/any_sender_of.hpp>

#include "pat/runtime/file.h"
#include "pat/runtime/file_fwd.h"
#include "pat/runtime/io_context.h"
#include "unifex/just.hpp"
#include "unifex/let_value.hpp"
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
    } catch (const std::error_code& ec) {
        std::cout << "Caught error_code: " << ec.message() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unexpected throw in main" << std::endl;
    }

    std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;

    return 0;
}