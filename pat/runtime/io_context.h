#pragma once

#include <uv.h>

#include <condition_variable>
#include <mutex>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <unifex/get_stop_token.hpp>
#include <unifex/receiver_concepts.hpp>

#include "pat/runtime/promise.h"

namespace pat::runtime::_io_context {

class context;

struct task_base {
    using execute_fn = void(task_base*) noexcept;

    explicit task_base(execute_fn* execute) noexcept : execute_(execute) {}

    void execute() noexcept { this->execute_(this); }

    task_base* next_ = nullptr;
    execute_fn* execute_;
};

template <typename Receiver>
struct _op {
    class type;
};

template <typename Receiver>
using operation = typename _op<std::remove_cvref_t<Receiver>>::type;

template <typename Receiver>
class _op<Receiver>::type final : task_base {
    using stop_token_type = unifex::stop_token_type_t<Receiver&>;

   public:
    template <typename Receiver2>
    explicit type(Receiver2&& receiver, context* loop)
        : task_base(&type::execute_impl), receiver_((Receiver2&&)receiver), loop_(loop) {}

    void start() noexcept;

   private:
    static void execute_impl(task_base* t) noexcept {
        auto& self = *static_cast<type*>(t);
        if constexpr (unifex::is_stop_never_possible_v<stop_token_type>) {
            unifex::set_value(std::move(self.receiver_));
        } else {
            if (get_stop_token(self.receiver_).stop_requested()) {
                unifex::set_done(std::move(self.receiver_));
            } else {
                unifex::set_value(std::move(self.receiver_));
            }
        }
    }

    UNIFEX_NO_UNIQUE_ADDRESS Receiver receiver_;
    context* const loop_;
};

class context {
    template <class Receiver>
    friend struct _op;

   public:
    class scheduler {
        class schedule_task {
           public:
            template <template <typename...> class Variant, template <typename...> class Tuple>
            using value_types = Variant<Tuple<>>;

            template <template <typename...> class Variant>
            using error_types = Variant<>;

            static constexpr bool sends_done = true;

            static constexpr unifex::blocking_kind blocking = unifex::blocking_kind::never;

            template <typename Receiver>
            operation<Receiver> connect(Receiver&& receiver) const {
                return operation<Receiver>{(Receiver&&)receiver, loop_};
            }

           private:
            friend scheduler;

            explicit schedule_task(context* loop) noexcept : loop_(loop) {}

            context* const loop_;
        };

        friend context;

        explicit scheduler(context* loop) noexcept : loop_(loop) {}

       public:
        unifex::sender auto schedule() const noexcept { return promise(schedule_task{loop_}); }

        friend bool operator==(scheduler a, scheduler b) noexcept { return a.loop_ == b.loop_; }
        friend bool operator!=(scheduler a, scheduler b) noexcept { return a.loop_ != b.loop_; }

       private:
        context* loop_;
    };

    scheduler get_scheduler() { return scheduler{this}; }

    void run();
    void stop();

    uv_loop_t* GetLoop();

    context() noexcept;
    context(context const&) = delete;
    context& operator=(context const&) = delete;
    context(context&&) = delete;
    context& operator=(context&&) = delete;
    ~context();

   private:
    void enqueue(task_base* task);
    void dequeueall();
    static void read_cb(uv_stream_s* req, long status, const uv_buf_t* /*unused*/);

    std::mutex mutex_;
    task_base* head_ = nullptr;
    task_base* tail_ = nullptr;
    bool stop_ = false;

    uv_loop_t uv_loop_{};
    std::array<uv_file, 2> pipes_{};
    uv_pipe_t read_pipe_{};
    char read_char_{};
    uv_buf_t read_buf_{.base = &read_char_, .len = sizeof(read_char_)};
    std::thread thread_;
    std::thread::id thread_id_{};
};

template <typename Receiver>
inline void _op<Receiver>::type::start() noexcept {
    loop_->enqueue(this);
}

}  // namespace pat::runtime::_io_context

namespace pat::runtime {
using IOContext = _io_context::context;
}