#include "pat/runtime/io_context.h"

#include "uv.h"

namespace pat::runtime {

IOContext::IOContext() : thread_([this]() { this->run(); }) {}

IOContext::IOContext(IOContext&& other) noexcept = default;
IOContext& IOContext::operator=(IOContext&& other) noexcept = default;

IOContext::~IOContext() {
    if (loop_ != nullptr) {
        stop_ = true;
        thread_.join();
        uv_loop_close(loop_.get());
        loop_ = nullptr;
    }
}

IOContext IOContext::Create() {
    IOContext ioc;

    ioc.loop_ = std::make_unique<uv_loop_s>();
    uv_loop_init(ioc.loop_.get());
    return ioc;
}

void IOContext::run() {
    while (!this->stop_) {
        uv_run(loop_.get(), UV_RUN_DEFAULT);
    }
}

}  // namespace pat::runtime