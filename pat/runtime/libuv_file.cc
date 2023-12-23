#include "pat/runtime/libuv_file.h"

#include <unistd.h>

namespace pat::runtime {

File::File() = default;
File::File(File &&other) noexcept = default;
File &File::operator=(File &&other) noexcept = default;
File::~File() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

}  // namespace pat::runtime