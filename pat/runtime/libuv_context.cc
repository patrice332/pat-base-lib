#include "pat/runtime/libuv_context.h"

#include "uv.h"

namespace pat::runtime {

IOContext::IOContext() = default;

IOContext::IOContext(IOContext&& other) noexcept = default;
IOContext& IOContext::operator=(IOContext&& other) noexcept = default;

IOContext::~IOContext() {
    if (loop_ != nullptr) {
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

}  // namespace pat::runtime