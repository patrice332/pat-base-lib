#pragma once

#include <memory>
#include <thread>

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
    void run();
    explicit IOContext(std::unique_ptr<uv_loop_s> loop);
    std::unique_ptr<uv_loop_s> loop_{};
    bool stop_{false};
    std::thread thread_{};
};

}  // namespace pat::runtime