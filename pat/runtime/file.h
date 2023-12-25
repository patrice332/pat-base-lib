#pragma once

#include <string>

#include "pat/runtime/close.h"
#include "pat/runtime/file_fwd.h"
#include "pat/runtime/io_context.h"
#include "pat/runtime/open.h"
#include "pat/runtime/promise.h"
#include "pat/runtime/read.h"
#include "pat/runtime/write.h"
#include "unifex/sender_concepts.hpp"
#include "unifex/task.hpp"

struct uv_fs_s;

namespace pat::runtime {

class File {
   public:
    static unifex::sender auto Open(IOContext &context, std::string path, int flags, int mode = 0) {
        return promise(_open::_sender{context.GetLoop(), std::move(path), flags, mode});
    }

    File(File const &) = delete;
    File &operator=(File const &) = delete;
    File(File &&other) noexcept;
    File &operator=(File &&other) noexcept;

    ~File();

    unifex::sender auto Write(IOContext &context, std::span<const char> msg) const {
        return promise(_write::_sender(context.GetLoop(), fd_, msg));
    }

    unifex::sender auto Read(IOContext &context, std::span<char> msg) const {
        return promise(_read::_sender(context.GetLoop(), fd_, msg));
    }

    unifex::sender auto Close(IOContext &context) const {
        return promise(_close::_sender(context.GetLoop(), fd_));
    }

    constexpr int Descriptor() const { return fd_; }

   private:
    File();
    friend File NewFile(int file_descriptor, std::string name);
    int fd_{-1};
};

}  // namespace pat::runtime