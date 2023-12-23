#pragma once

#include <string>

#include "pat/runtime/file_fwd.h"
#include "pat/runtime/io_context.h"
#include "pat/runtime/open.h"
#include "unifex/sender_concepts.hpp"

struct uv_fs_s;

namespace pat::runtime {

class File {
   public:
    static unifex::sender auto Open(IOContext &context, std::string path, int flags, int mode) {
        return _open::_sender{context.GetLoop(), std::move(path), flags, mode};
    }

    File(File const &) = delete;
    File &operator=(File const &) = delete;
    File(File &&other) noexcept;
    File &operator=(File &&other) noexcept;

    ~File();

   private:
    File();
    friend File NewFile(int file_descriptor, std::string name);
    int fd_{-1};
};

}  // namespace pat::runtime