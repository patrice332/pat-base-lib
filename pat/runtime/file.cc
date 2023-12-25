#include "pat/runtime/file.h"

#include <unistd.h>

namespace pat::runtime {

File::File() = default;
File::File(File &&other) noexcept : fd_{other.fd_} { other.fd_ = -1; }

File &File::operator=(File &&other) noexcept {
    if (this != &other) {
        if (fd_ != -1) {
            close(fd_);
        }
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

File::~File() {
    if (fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

File NewFile(int file_descriptor, std::string /* name */) {
    File file;
    file.fd_ = file_descriptor;
    return file;
}

}  // namespace pat::runtime