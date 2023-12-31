#pragma once

#include <system_error>
#include <type_traits>

#include "pat/io/io.h"
#include "uv.h"

namespace pat::io {

template <typename R, class Enable = void>
    requires Reader<R>
class LimitedReader;

template <typename R>
    requires Reader<R>
class LimitedReader<R, typename std::enable_if<std::is_lvalue_reference_v<R>>::type> {
   public:
    explicit LimitedReader(std::remove_cvref<R>& r, std::size_t n) : r_{r}, n_{n} {}

    auto Read(std::span<char> buf) {
        if (n_ == 0) {
            if constexpr (AsyncReader<R>) {
                throw std::error_code(UV_EOF, std::generic_category());
            } else {
                return std::error_code(UV_EOF, std::generic_category());
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
class LimitedReader<R, typename std::enable_if<std::is_rvalue_reference_v<R>>::type> {
   public:
    explicit LimitedReader(std::remove_cvref<R>&& r, std::size_t n) : r_{std::move(r)}, n_{n} {}

    auto Read(std::span<char> buf) {
        if (n_ == 0) {
            if constexpr (AsyncReader<R>) {
                throw std::error_code(UV_EOF, std::generic_category());
            } else {
                return std::error_code(UV_EOF, std::generic_category());
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