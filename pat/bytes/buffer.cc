#include "pat/bytes/buffer.h"

#include <cstring>

namespace pat::bytes {

namespace testing {
__attribute((weak)) std::size_t* alloc_counter = nullptr;
}

Buffer::Buffer() = default;
Buffer::Buffer(std::string&& str) : buf_{std::move(str)} {}
Buffer::Buffer(std::string_view str) : buf_view_{str}, in_view_{true} {}

std::expected<std::size_t, std::error_code> Buffer::Read(std::span<char> buf) {
    std::size_t read_bytes = std::min(Size(), buf.size());
    if (in_view_) {
        std::copy(&buf_view_[pos_], &buf_view_[pos_ + read_bytes], buf.data());
    } else {
        std::copy(&buf_[pos_], &buf_[pos_ + read_bytes], buf.data());
    }
    pos_ += read_bytes;
    return read_bytes;
}

std::expected<std::size_t, std::error_code> Buffer::Write(std::span<const char> buf) {
    if (buf.empty()) {
        return 0;
    }

    // We are going to modify
    if (in_view_) {
        if (testing::alloc_counter != nullptr) {
            *testing::alloc_counter = *testing::alloc_counter + 1;
        }
        buf_.reserve(buf_view_.size() + buf.size());
        buf_.assign(buf_view_);
        in_view_ = false;
    }

    // If you can't write at the end, copy
    if (Available() < buf.size()) {
        if (Capacity() - Size() < buf.size()) {
            // Reallocation
            if (testing::alloc_counter != nullptr) {
                *testing::alloc_counter = *testing::alloc_counter + 1;
            }
            std::string new_buf;
            new_buf.reserve(Size() + buf.size());
            new_buf.assign(&buf_[pos_], Size());
            buf_ = std::move(new_buf);
        } else {
            std::memmove(buf_.data(), &buf_[pos_], Size());
            buf_.resize(Size());
        }
        pos_ = 0;
    }

    buf_.append(buf.begin(), buf.end());
    return buf.size();
}

}  // namespace pat::bytes