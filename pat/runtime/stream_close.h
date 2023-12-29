#pragma once

#include <cstring>
#include <span>
#include <system_error>

#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_stream_close {

template <unifex::receiver_of<> Receiver>
class _op {
   public:
    // trunk-ignore(clang-tidy/bugprone-easily-swappable-parameters)
    _op(Receiver &&rec, uv_stream_t *stream_handle)
        : rec_{std::move(rec)}, stream_handle_{stream_handle} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        stream_handle_->data = this;
        uv_close(reinterpret_cast<uv_handle_t *>(stream_handle_), [](uv_handle_t *handle) {
            auto *operation = reinterpret_cast<_op<Receiver> *>(handle->data);
            unifex::set_value(std::move(operation->rec_));
        });
    }

   private:
    Receiver rec_;
    uv_stream_t *stream_handle_;
};

class _sender {
   public:
    _sender(uv_stream_t *stream_handle_);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), stream_handle_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::error_code>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_stream_t *stream_handle_;
};

}  // namespace pat::runtime::_stream_close