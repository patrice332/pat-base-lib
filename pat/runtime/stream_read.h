#pragma once

#include <cstring>
#include <span>
#include <system_error>

#include "pat/io/io.h"
#include "pat/runtime/libuv_errors.h"
#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_stream_read {

template <unifex::receiver_of<std::size_t> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_stream_t *stream_handle, std::span<char> msg)
        : rec_{std::move(rec)}, stream_handle_{stream_handle}, msg_{msg} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        stream_handle_->data = this;
        uv_read_start(
            stream_handle_,
            [](uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
                // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                auto *operation = reinterpret_cast<_op<Receiver> *>(handle->data);
                buf->base = operation->msg_.data();
                buf->len = std::min(suggested_size, operation->msg_.size());
            },
            [](uv_stream_t *stream, ssize_t nread, const uv_buf_t * /*buf*/) {
                uv_read_stop(stream);
                // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                auto *operation = reinterpret_cast<_op<Receiver> *>(stream->data);
                if (nread < 0) {
                    if (nread == UV_EOF) {
                        unifex::set_error(std::move(operation->rec_),
                                          std::make_exception_ptr(std::system_error(
                                              make_error_code(io::IoError::kEOF))));
                        return;
                    }
                    unifex::set_error(std::move(operation->rec_),
                                      std::make_exception_ptr(std::system_error(std::error_code(
                                          static_cast<int>(nread), LibUVErrCategory))));
                    return;
                }
                std::move(operation->rec_).set_value(static_cast<std::size_t>(nread));
            });
    }

   private:
    Receiver rec_{};
    uv_stream_t *stream_handle_{nullptr};
    std::span<char> msg_{};
};

class _sender {
   public:
    _sender(uv_stream_t *stream_handle_, std::span<char> msg);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<std::size_t> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), stream_handle_, msg_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<std::size_t>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_stream_t *stream_handle_{nullptr};
    std::span<char> msg_{};
};

}  // namespace pat::runtime::_stream_read