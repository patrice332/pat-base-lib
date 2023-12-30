#pragma once

#include <string>
#include <system_error>

#include "pat/runtime/libuv_errors.h"
#include "unifex/blocking.hpp"
#include "unifex/receiver_concepts.hpp"
#include "uv.h"

namespace pat::runtime::_getaddrinfo {

template <unifex::receiver_of<struct addrinfo *> Receiver>
class _op {
   public:
    _op(Receiver &&rec, uv_loop_t *loop, std::string node, std::string service,
        struct addrinfo hints)
        : rec_(std::move(rec)),
          loop_{loop},
          node_{std::move(node)},
          service_{std::move(service)},
          hints_{hints} {}

    _op(_op const &) = delete;
    _op &operator=(_op const &) = delete;
    _op(_op &&other) noexcept = default;
    _op &operator=(_op &&other) noexcept = default;
    ~_op() = default;

    void start() noexcept {
        getaddrinfo_op_.data = this;
        uv_getaddrinfo(
            loop_, &getaddrinfo_op_,
            [](uv_getaddrinfo_t *getaddrinfo_op, int status, struct addrinfo *res) {
                // trunk-ignore(clang-tidy/cppcoreguidelines-pro-type-reinterpret-cast)
                auto *operation = reinterpret_cast<_op<Receiver> *>(getaddrinfo_op->data);
                if (status < 0) {
                    std::move(operation->rec_)
                        .set_error(std::error_code(static_cast<int>(status), LibUVErrCategory));
                    return;
                }

                std::move(operation->rec_).set_value(res);
            },
            node_.c_str(), service_.c_str(), &hints_);
    }

   private:
    Receiver rec_;
    uv_loop_t *loop_;
    std::string node_;
    std::string service_;
    struct addrinfo hints_;
    uv_getaddrinfo_t getaddrinfo_op_{};
};

class _sender {
   public:
    _sender(uv_loop_t *loop, std::string node, std::string service, struct addrinfo hints);
    _sender(_sender const &) = delete;
    _sender &operator=(_sender const &) = delete;
    _sender(_sender &&other) noexcept;
    // trunk-ignore(clang-tidy/bugprone-exception-escape)
    _sender &operator=(_sender &&other) noexcept;
    ~_sender();

    auto connect(unifex::receiver_of<struct addrinfo *> auto &&rec) && {
        return _op{std::forward<decltype(rec)>(rec), loop_, std::move(node_), std::move(service_),
                   hints_};
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Variant<Tuple<struct addrinfo *>>;
    template <template <typename...> class Variant>
    using error_types = Variant<std::error_code>;
    static constexpr bool sends_done = false;
    static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;
    static constexpr bool is_always_scheduler_affine = false;

   private:
    uv_loop_t *loop_;
    std::string node_;
    std::string service_;
    struct addrinfo hints_;
};

}  // namespace pat::runtime::_getaddrinfo