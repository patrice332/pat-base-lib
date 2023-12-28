#pragma once

#include "pat/runtime/io_context.h"
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

   protected:
    constexpr explicit Stream(IOContext& context) : loop_{context.GetLoop()} {}

    constexpr uv_loop_t* Loop() { return loop_; }
    constexpr uv_loop_t const* Loop() const { return loop_; }

   private:
    uv_loop_t* loop_;
};

}  // namespace pat::runtime