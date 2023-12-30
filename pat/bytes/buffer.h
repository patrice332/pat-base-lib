#pragma once

#include <span>
#include <string>
#include <string_view>

namespace pat::bytes {

struct Buffer {
    Buffer();
    explicit Buffer(std::string&& str);
    explicit Buffer(std::string_view str);

    std::size_t Read(std::span<char> buf);
    std::size_t Write(std::span<const char> buf);

    constexpr std::size_t Size() const {
        return in_view_ ? buf_view_.size() - pos_ : buf_.size() - pos_;
    }

    constexpr bool Empty() const { return Size() == 0; }
    constexpr std::size_t Available() const { return in_view_ ? 0 : buf_.capacity() - buf_.size(); }
    constexpr std::size_t Capacity() const { return in_view_ ? buf_view_.size() : buf_.capacity(); }

   private:
    std::string buf_{};
    std::string_view buf_view_{};
    std::size_t pos_{0};
    bool in_view_{false};
};

}  // namespace pat::bytes