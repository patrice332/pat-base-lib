#pragma once

#include <concepts>
#include <unifex/let_done.hpp>
#include <unifex/let_error.hpp>
#include <unifex/let_value.hpp>
#include <unifex/then.hpp>
#include <utility>

namespace pat::runtime::_promise {

template <typename Sender>
struct _sender {
    class type;
};

template <typename Sender>
using sender = typename _sender<Sender>::type;

template <typename Sender>
class _sender<Sender>::type {
   public:
    explicit type(Sender&& send) : send_{std::move(send)} {}

    auto connect(unifex::receiver auto&& rec) && {
        return unifex::connect(std::move(send_), std::forward<decltype(rec)>(rec));
    }

    template <template <typename...> class Variant, template <typename...> class Tuple>
    using value_types = Sender::template value_types<Variant, Tuple>;
    template <template <typename...> class Variant>
    using error_types = Sender::template error_types<Variant>;
    static constexpr bool sends_done = unifex::sender_traits<Sender>::sends_done;
    static constexpr unifex::blocking_kind blocking = unifex::sender_traits<Sender>::blocking;
    static constexpr bool is_always_scheduler_affine =
        unifex::sender_traits<Sender>::is_always_scheduler_affine;

    template <typename SuccessorFactory>
        requires std::invocable<decltype(unifex::let_value), Sender&&, SuccessorFactory&&>
    auto let(SuccessorFactory&& func) && {
        return sender<unifex::_let_v::sender<Sender, SuccessorFactory>>{
            unifex::let_value(std::move(send_), std::forward<SuccessorFactory>(func))};
    }

    template <typename DoneFactory>
        requires std::invocable<decltype(unifex::let_done), Sender&&, DoneFactory&&>
    auto let_done(DoneFactory&& func) && {
        return sender<unifex::_let_d::_sender<Sender, DoneFactory>>{
            unifex::let_done(std::move(send_), std::forward<DoneFactory>(func))};
    }

    template <typename ErrorFactory>
        requires std::invocable<decltype(unifex::let_error), Sender&&, ErrorFactory&&>
    auto let_error(ErrorFactory&& func) && {
        return sender<unifex::_let_e::_sender<Sender, ErrorFactory>>{
            unifex::let_error(std::move(send_), std::forward<ErrorFactory>(func))};
    }

    template <typename SuccessorFactory>
        requires std::invocable<decltype(unifex::then), Sender&&, SuccessorFactory&&>
    auto then(SuccessorFactory&& func) && {
        return sender<unifex::_then::sender<Sender, SuccessorFactory>>{
            unifex::then(std::move(send_), std::forward<SuccessorFactory>(func))};
    }

   private:
    Sender send_;
};

struct _fn {
    template <typename Sender>
        requires unifex::sender<Sender>
    unifex::sender auto operator()(Sender&& send) const {
        return sender<Sender>(std::forward<decltype(send)>(send));
    }
};

}  // namespace pat::runtime::_promise

namespace pat::runtime {
inline constexpr _promise::_fn promise{};
}