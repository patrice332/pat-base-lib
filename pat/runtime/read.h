#pragma once

#include <cstring>
#include <span>
#include <system_error>

#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_read {

template <unifex::receiver_of<std::size_t> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_loop_t *loop, int file_descriptor, std::span<char> msg)
        : rec_(std::move(rec)),
          loop_{loop},
          file_descriptor_{file_descriptor},
          msg_{uv_buf_t{.base = msg.data(), .len = msg.size()}} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        fs_op_.data = this;
        uv_fs_read(
            loop_, &fs_op_, file_descriptor_, msg_.data(), msg_.size(), -1, [](uv_fs_t *fs_op) {
                // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                uv_fs_req_cleanup(fs_op);

                auto *operation = reinterpret_cast<_op<Receiver> *>(fs_op->data);
                if (fs_op->result < 0) {
                    std::move(operation->rec_)
                        .set_error(
                            std::error_code(static_cast<int>(errno), std::generic_category()));
                    return;
                }

                std::move(operation->rec_).set_value(static_cast<std::size_t>(fs_op->result));
            });
    }

   private:
    Receiver rec_;
    uv_loop_t *loop_;
    int file_descriptor_;
    std::array<uv_buf_t, 1> msg_;
    uv_fs_t fs_op_{};
};

class _sender {
   public:
    _sender(uv_loop_t *loop, int file_descriptor, std::span<char> msg);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<std::size_t> auto &&rec) {
        return _op{std::forward<decltype(rec)>(rec), loop_, file_descriptor_, msg_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<std::size_t>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::error_code>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_loop_t *loop_;
    int file_descriptor_;
    std::span<char> msg_;
};

}  // namespace pat::runtime::_read