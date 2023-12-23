#pragma once

#include <exception>
#include <system_error>

#include "pat/runtime/file_fwd.h"
#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_open {

template <unifex::receiver_of<File> Receiver>
class _op {
   public:
    // trunk-ignore(clang-tidy/bugprone-easily-swappable-parameters)
    _op(Receiver &&rec, uv_loop_t *loop, std::string path, int flags, int mode)
        : rec_(std::move(rec)), loop_{loop}, path_{std::move(path)}, flags_{flags}, mode_{mode} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        fs_op_.data = this;
        uv_fs_open(loop_, &fs_op_, path_.c_str(), flags_, mode_, [](uv_fs_t *fs_op) {
            // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
            auto *operation = reinterpret_cast<_op<Receiver> *>(fs_op->data);
            if (fs_op->result < 0) {
                std::move(operation->rec_)
                    .set_error(std::error_code(static_cast<int>(errno), std::generic_category()));
                return;
            }

            auto file = NewFile(fs_op->result, std::move(operation->path_));
            std::move(operation->rec_).set_value(std::move(file));
        });
    }

   private:
    void start_impl(std::function<void(File)> set_value,
                    std::function<void(std::exception_ptr)> on_error,
                    std::function<void()> set_done);

    Receiver rec_;
    uv_loop_t *loop_;
    std::string path_;
    int flags_;
    int mode_;
    uv_fs_t fs_op_{};
};

class _sender {
   public:
    _sender(uv_loop_t *loop, std::string path, int flags, int mode);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<File> auto &&rec) {
        return _op{std::forward<decltype(rec)>(rec), loop_, std::move(path_), flags_, mode_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<File>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_loop_t *loop_;
    std::string path_;
    int flags_;
    int mode_;
};

}  // namespace pat::runtime::_open