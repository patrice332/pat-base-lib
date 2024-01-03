#pragma once

#include <uv.h>

#include <system_error>
#include <unifex/blocking.hpp>
#include <unifex/receiver_concepts.hpp>

#include "pat/runtime/libuv_errors.h"

namespace pat::runtime::_close {

template <unifex::receiver_of<> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_loop_t *loop, int file_descriptor)
        : rec_(std::move(rec)), loop_{loop}, file_descriptor_{file_descriptor} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        fs_op_.data = this;
        uv_fs_close(loop_, &fs_op_, file_descriptor_, [](uv_fs_t *fs_op) {
            uv_fs_req_cleanup(fs_op);

            // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
            auto *operation = reinterpret_cast<_op<Receiver> *>(fs_op->data);
            if (fs_op->result < 0) {
                unifex::set_error(std::move(operation->rec_),
                                  std::make_exception_ptr(std::system_error(std::error_code(
                                      static_cast<int>(fs_op->result), LibUVErrCategory))));
                return;
            }

            std::move(operation->rec_).set_value();
        });
    }

   private:
    Receiver rec_{};
    uv_loop_t *loop_;
    int file_descriptor_;
    uv_fs_t fs_op_{};
};

class _sender {
   public:
    _sender(uv_loop_t *loop, int file_descriptor);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), loop_, file_descriptor_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_loop_t *loop_;
    int file_descriptor_;
};

}  // namespace pat::runtime::_close