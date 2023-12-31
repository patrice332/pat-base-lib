#pragma once

#include "pat/runtime/io_context.h"
#include "pat/runtime/promise.h"
#include "pat/runtime/stream_close.h"
#include "pat/runtime/stream_read.h"
#include "pat/runtime/stream_write.h"
#include "unifex/sender_concepts.hpp"
#include "uv.h"

namespace pat::runtime {

class Stream {
   public:
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
    Stream(Stream&& other) noexcept;
    Stream& operator=(Stream&& other) noexcept;
    virtual ~Stream();

    unifex::sender auto Write(std::span<const char> msg) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        return pat::runtime::promise(_stream_write::_sender(&StreamHandle(), msg));
    }

    unifex::sender auto Read(std::span<char> msg) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        return pat::runtime::promise(_stream_read::_sender(&StreamHandle(), msg));
    }

    unifex::sender auto Close() {
        return pat::runtime::promise(_stream_close::_sender(&StreamHandle()));
    }

   protected:
    constexpr explicit Stream(IOContext& context) : loop_{context.GetLoop()} {}

    constexpr uv_loop_t* Loop() { return loop_; }
    constexpr uv_loop_t const* Loop() const { return loop_; }

    virtual uv_stream_t& StreamHandle() = 0;
    virtual uv_stream_t const& StreamHandle() const = 0;

   private:
    uv_loop_t* loop_;
};

}  // namespace pat::runtime