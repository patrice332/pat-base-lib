#include "pat/runtime/context.h"

#include <atomic>
#include <memory>

namespace pat::runtime {

namespace {

std::once_flag io_context_init_flag;
std::unique_ptr<IOContext> io_context;

std::once_flag thread_pool_init_flag;
std::unique_ptr<unifex::static_thread_pool> thread_pool;

}  // namespace

IOContext& GetIOContext() {
    std::call_once(io_context_init_flag, []() { io_context = std::make_unique<IOContext>(); });
    return *io_context;
}

unifex::static_thread_pool& GetThreadPool() {
    std::call_once(thread_pool_init_flag,
                   []() { thread_pool = std::make_unique<unifex::static_thread_pool>(); });
    return *thread_pool;
}

}  // namespace pat::runtime