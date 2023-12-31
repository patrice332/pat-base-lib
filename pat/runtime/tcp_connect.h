#pragma once

#include <system_error>

#include "pat/runtime/libuv_errors.h"
#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_tcp_connect {

template <unifex::receiver_of<> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_tcp_t *handle, const struct sockaddr *addr)
        : rec_(std::move(rec)), handle_{handle}, addr_{addr} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        connect_op_.data = this;
        uv_tcp_connect(&connect_op_, handle_, addr_, [](uv_connect_t *req, int status) {
            // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
            auto *operation = reinterpret_cast<_op<Receiver> *>(req->data);
            if (status < 0) {
                unifex::set_error(std::move(operation->rec_),
                                  std::make_exception_ptr(std::system_error(std::error_code(
                                      static_cast<int>(status), LibUVErrCategory))));
                return;
            }

            std::move(operation->rec_).set_value();
        });
    }

   private:
    Receiver rec_{};
    uv_tcp_t *handle_{nullptr};
    struct sockaddr const *addr_{nullptr};
    uv_connect_t connect_op_{};
};

class _sender {
   public:
    _sender(uv_tcp_t *handle, const struct sockaddr *addr);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), handle_, addr_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::exception_ptr>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_tcp_t *handle_;
    struct sockaddr const *addr_;
};

}  // namespace pat::runtime::_tcp_connect