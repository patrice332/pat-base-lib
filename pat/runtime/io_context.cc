#include "pat/runtime/io_context.h"

#include <exception>

#include "uv.h"

namespace pat::runtime {

IOContext::IOContext(std::unique_ptr<uv_loop_s> loop)
    : loop_{std::move(loop)}, thread_([this]() { this->run(); }) {}

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
    auto loop = std::make_unique<uv_loop_s>();
    uv_loop_init(loop.get());
    return IOContext{std::move(loop)};
}

void IOContext::run() {
    while (!this->stop_) {
        if (loop_ == nullptr) {
            std::terminate();
        }
        uv_run(loop_.get(), UV_RUN_DEFAULT);
    }
}

}  // namespace pat::runtime