#pragma once

#include <unifex/static_thread_pool.hpp>

#include "pat/runtime/io_context.h"

namespace pat::runtime {

IOContext& GetIOContext();
unifex::static_thread_pool& GetThreadPool();

}  // namespace pat::runtime