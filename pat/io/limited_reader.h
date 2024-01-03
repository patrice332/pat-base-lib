#pragma once

#include <uv.h>

#include <system_error>
#include <type_traits>

#include "pat/io/io.h"

namespace pat::io {

template <typename R, class Enable = void>
    requires Reader<R>
class LimitedReader;

template <typename R>
    requires Reader<R>
class LimitedReader<R, typename std::enable_if_t<std::is_lvalue_reference_v<R>>> {
   public:
    explicit LimitedReader(std::remove_cvref<R>& rec, std::size_t n) : r_{rec}, n_{n} {}

    LimitedReader(const LimitedReader&) = delete;
    LimitedReader& operator=(const LimitedReader&) = delete;
    LimitedReader(LimitedReader&&) = delete;
    LimitedReader& operator=(LimitedReader&&) = delete;

    ~LimitedReader() = default;

    auto Read(std::span<char> buf) {
        if (n_ == 0) {
            if constexpr (AsyncReader<R>) {
                throw std::system_error(IoError::kEOF);
            } else {
                return std::unexpected(make_error_code(IoError::kEOF));
            }
        }
        if (buf.size() > n_) {
            buf = buf.subspan(0, n_);
        }
        n_ -= buf.size();
        return r_.Read(buf);
    }

   private:
    std::remove_cvref<R>& r_;
    std::size_t n_;
};

template <typename R>
    requires Reader<R>
class LimitedReader<R, typename std::enable_if_t<std::is_rvalue_reference_v<R>>> {
   public:
    explicit LimitedReader(std::remove_cvref<R>&& rec, std::size_t n) : r_{std::move(rec)}, n_{n} {}

    auto Read(std::span<char> buf) {
        if (n_ == 0) {
            if constexpr (AsyncReader<R>) {
                throw std::system_error(IoError::kEOF);
            } else {
                return std::unexpected(make_error_code(IoError::kEOF));
            }
        }
        if (buf.size() > n_) {
            buf = buf.subspan(0, n_);
        }
        n_ -= buf.size();
        return r_.Read(buf);
    }

   private:
    std::remove_cvref<R> r_;
    std::size_t n_;
};

}  // namespace pat::io