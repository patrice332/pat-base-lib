#pragma once

#include "pat/runtime/io_context.h"
#include "pat/runtime/promise.h"
#include "pat/runtime/stream_close.h"
#include "pat/runtime/stream_read.h"
#include "pat/runtime/stream_write.h"
#include "unifex/sender_concepts.hpp"
#include "uv.h"

namespace pat::runtime {

template <typename Derived>
class Stream {
   public:
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream(Stream&& other) noexcept = default;
    Stream& operator=(Stream&& other) noexcept = default;
    virtual ~Stream() {
        auto* handle =
            reinterpret_cast<uv_handle_t*>(&reinterpret_cast<Derived*>(this)->StreamHandle());
        if (!static_cast<bool>(uv_is_closing(handle))) {
            std::terminate();
        }
    }

    unifex::sender auto Write(std::span<const char> msg) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        return pat::runtime::promise(
            _stream_write::_sender(&reinterpret_cast<Derived*>(this)->StreamHandle(), msg));
    }

    unifex::sender auto Read(std::span<char> msg) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        return pat::runtime::promise(
            _stream_read::_sender(&reinterpret_cast<Derived*>(this)->StreamHandle(), msg));
    }

    unifex::sender auto Close() {
        return pat::runtime::promise(
            _stream_close::_sender(&reinterpret_cast<Derived*>(this)->StreamHandle()));
    }

   protected:
    constexpr explicit Stream(IOContext& context) : loop_{context.GetLoop()} {}

    constexpr uv_loop_t* Loop() { return loop_; }
    constexpr uv_loop_t const* Loop() const { return loop_; }

   private:
    uv_loop_t* loop_;
};

}  // namespace pat::runtime