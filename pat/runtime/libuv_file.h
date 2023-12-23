#pragma once

#include <stdexcept>
#include <string>

#include "pat/runtime/libuv_context.h"
#include "unifex/create.hpp"
#include "uv.h"

struct uv_fs_s;

namespace pat::runtime {

class File {
   public:
    static unifex::sender auto Open(IOContext &context, std::string name, int flags, int mode) {
        return unifex::create<File>([loop = context.GetLoop(), name = std::move(name), flags,
                                     mode](unifex::receiver auto &rec) {
            auto *fs_op_ptr = new uv_fs_s();
            fs_op_ptr->data = &rec;
            uv_fs_open(loop, fs_op_ptr, name.c_str(), flags, mode, [](uv_fs_s *fs_op) {
                unifex::receiver auto &rec_in = unifex::void_cast<decltype(rec)>(fs_op->data);
                if (fs_op->result < 0) {
                    rec_in.set_error(std::runtime_error(strerror(errno)));
                }

                File file;
                file.fd_ = static_cast<int>(fs_op->result);
                rec_in.set_value(file);
            });
        });
    }
    File(File const &) = delete;
    File &operator=(File const &) = delete;
    File(File &&other) noexcept;
    File &operator=(File &&other) noexcept;

    ~File();

   private:
    File();
    int fd_{-1};
};

}  // namespace pat::runtime