#pragma once

#include <uv.h>

#include <cstring>
#include <span>
#include <system_error>
#include <unifex/blocking.hpp>
#include <unifex/receiver_concepts.hpp>

#include "pat/runtime/libuv_errors.h"

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
        shutdown_req_.data = this;
        if (uv_is_writable((uv_stream_t *)stream_handle_) && stream_handle_->write_queue_size > 0) {
            uv_shutdown(&shutdown_req_, reinterpret_cast<uv_stream_t *>(stream_handle_),
                        [](uv_shutdown_s *stream, int status) {
                            auto *operation = reinterpret_cast<_op<Receiver> *>(stream->data);

                            if (status < 0) {
                                unifex::set_error(
                                    std::move(operation->rec_),
                                    std::make_exception_ptr(std::system_error(std::error_code(
                                        static_cast<int>(status), LibUVErrCategory))));
                            }

                            if (!uv_is_closing((uv_handle_t *)stream->handle)) {
                                uv_close(reinterpret_cast<uv_handle_t *>(operation->stream_handle_),
                                         [](uv_handle_t *handle) {
                                             // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                                             auto *operation =
                                                 reinterpret_cast<_op<Receiver> *>(handle->data);
                                             unifex::set_value(std::move(operation->rec_));
                                         });
                            } else {
                                unifex::set_value(std::move(operation->rec_));
                            }
                        });
        } else if (!uv_is_closing(reinterpret_cast<uv_handle_t *>(stream_handle_))) {
            uv_close(reinterpret_cast<uv_handle_t *>(stream_handle_), [](uv_handle_t *handle) {
                // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                auto *operation = reinterpret_cast<_op<Receiver> *>(handle->data);
                unifex::set_value(std::move(operation->rec_));
            });
        } else {
            unifex::set_value(std::move(rec_));
        }
    }

   private:
    Receiver rec_{};
    uv_stream_t *stream_handle_{nullptr};
    uv_shutdown_t shutdown_req_{};
};

class _sender {
   public:
    explicit _sender(uv_stream_t *stream_handle_);
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
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_stream_t *stream_handle_{nullptr};
};

}  // namespace pat::runtime::_stream_close