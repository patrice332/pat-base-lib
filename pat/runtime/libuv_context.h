#pragma once

#include <memory>

struct uv_loop_s;

namespace pat::runtime {

class IOContext {
   public:
    static IOContext Create();
    IOContext(IOContext const& other) = delete;
    IOContext& operator=(IOContext const& other) = delete;
    IOContext(IOContext&& other) noexcept;
    IOContext& operator=(IOContext&& other) noexcept;
    ~IOContext();

    constexpr uv_loop_s* GetLoop() { return loop_.get(); }
    constexpr uv_loop_s const* GetLoop() const { return loop_.get(); }

   private:
    IOContext();
    std::unique_ptr<uv_loop_s> loop_{};
};

}  // namespace pat::runtime