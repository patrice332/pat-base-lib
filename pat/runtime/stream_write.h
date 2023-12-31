#pragma once

#include <cstring>
#include <span>
#include <system_error>

#include "pat/runtime/libuv_errors.h"
#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_stream_write {

template <unifex::receiver_of<std::size_t> Receiver>
class _op {
   public:
    // trunk-ignore(clang-tidy/bugprone-easily-swappable-parameters)
    _op(Receiver &&rec, uv_stream_t *stream_handle, std::span<const char> msg)
        : rec_{std::move(rec)},
          stream_handle_{stream_handle},
          // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-const-cast)
          msg_{uv_buf_t{.base = const_cast<char *>(msg.data()), .len = msg.size()}} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        write_op_.data = this;
        uv_write(&write_op_, stream_handle_, msg_.data(), msg_.size(),
                 [](uv_write_t *fs_op, int status) {
                     // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                     auto *operation = reinterpret_cast<_op<Receiver> *>(fs_op->data);
                     if (status < 0) {
                         unifex::set_error(
                             std::move(operation->rec_),
                             std::make_exception_ptr(std::system_error(
                                 std::error_code(static_cast<int>(status), LibUVErrCategory))));
                         return;
                     }

                     std::move(operation->rec_).set_value(static_cast<std::size_t>(status));
                 });
    }

   private:
    Receiver rec_{};
    uv_stream_t *stream_handle_{nullptr};
    std::array<uv_buf_t, 1> msg_{};
    uv_write_t write_op_{};
};

class _sender {
   public:
    _sender(uv_stream_t *stream_handle_, std::span<const char> msg);
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
    std::span<const char> msg_{};
};

}  // namespace pat::runtime::_stream_write