#include "pat/runtime/io_context.h"

#include <unistd.h>
#include <uv.h>

#include <cassert>
#include <exception>
#include <format>
#include <iostream>
#include <thread>
#include <unifex/sync_wait.hpp>

#include "pat/runtime/stream_close.h"

namespace pat::runtime::_io_context {

context::context() noexcept {
    uv_loop_init(&uv_loop_);
    uv_pipe(pipes_.data(), UV_NONBLOCK_PIPE, 0);
    uv_pipe_init(&uv_loop_, &read_pipe_, 0);
    uv_pipe_open(&read_pipe_, pipes_[0]);
    read_pipe_.data = this;
    uv_read_start(
        reinterpret_cast<uv_stream_t*>(&read_pipe_),
        [](uv_handle_s* handle, unsigned long, uv_buf_t* buf) {
            *buf = reinterpret_cast<context*>(handle->data)->read_buf_;
        },
        &context::read_cb);
    std::thread new_t{[this]() { run(); }};
    thread_.swap(new_t);
}

void context::read_cb(uv_stream_s* req, long status, const uv_buf_t* /*unused*/) {
    auto* self = reinterpret_cast<context*>(req->data);
    if (status <= 0) {
        return;
    }

    self->dequeueall();
}

context::~context() {
    stop();
    if (thread_.joinable()) {
        thread_.join();
    }
    int const rv = uv_loop_close(&uv_loop_);
    if (rv != 0) {
        std::cerr << "Loop close failed: " << uv_err_name(rv) << ": " << uv_strerror(rv)
                  << std::endl;
    }
}

void context::run() {
    thread_id_ = std::this_thread::get_id();
    while (!stop_) {
        uv_run(&uv_loop_, UV_RUN_DEFAULT);
    }
}

void context::dequeueall() {
    std::unique_lock lock{mutex_};
    if (head_ != nullptr) {
        auto* task = head_;
        head_ = task->next_;
        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        lock.unlock();
        task->execute();
        lock.lock();
    }
}

void context::stop() {
    unifex::sync_wait(get_scheduler().schedule().let([this]() {
        return promise(_stream_close::_sender{reinterpret_cast<uv_stream_t*>(&read_pipe_)})
            .then([this]() {
                stop_ = true;
                uv_stop(&uv_loop_);
            });
    }));
}

uv_loop_s* context::GetLoop() {
    assert(thread_id_ == std::this_thread::get_id());
    return &uv_loop_;
}

void context::enqueue(task_base* task) {
    std::unique_lock const lock{mutex_};
    if (head_ == nullptr) {
        head_ = task;
    } else {
        tail_->next_ = task;
    }
    tail_ = task;
    task->next_ = nullptr;
    char i = 0;
    (void)::write(pipes_[1], &i, sizeof(i));
}

}  // namespace pat::runtime::_io_context