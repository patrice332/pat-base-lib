#pragma once

#include "pat/runtime/io_context.h"
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
    virtual ~Stream() = default;

    unifex::sender auto Write(std::span<const char> msg) {
        // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
        return _stream_write::_sender(&reinterpret_cast<Derived*>(this)->StreamHandle(), msg);
    }

   protected:
    constexpr explicit Stream(IOContext& context) : loop_{context.GetLoop()} {}

    constexpr uv_loop_t* Loop() { return loop_; }
    constexpr uv_loop_t const* Loop() const { return loop_; }

   private:
    uv_loop_t* loop_;
};

}  // namespace pat::runtime