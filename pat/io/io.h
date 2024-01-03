#pragma once

#include <concepts>
#include <expected>
#include <span>
#include <system_error>
#include <type_traits>
#include <unifex/sender_concepts.hpp>

namespace pat::io {

enum class IoError {
    kSuccess,
    kEOF,
    kClosedPipe,
    kNoProgress,
    kShortBuffer,
    kShortWrite,
    kUnexpectedEOF,
};
std::error_code make_error_code(IoError error_code);

template <typename From, typename To>
concept _is_sender_convertible_to =
    unifex::sender<From> &&
    unifex::convertible_to<unifex::sender_single_value_return_type_t<From>, To>;

template <typename T>
concept AsyncReader = requires(T self, std::span<char> read_buf) {
    { self.Read(read_buf) } -> _is_sender_convertible_to<std::size_t>;
};

template <typename T>
concept AsyncWriter = requires(T self, std::span<const char> write_buf) {
    { self.Write(write_buf) } -> _is_sender_convertible_to<std::size_t>;
};

template <typename T>
concept AsyncCloser = requires(T self) {
    { self.Close() } -> _is_sender_convertible_to<void>;
};

template <typename T>
concept ImmReader = requires(T self, std::span<char> read_buf) {
    { self.Read(read_buf) } -> std::convertible_to<std::expected<std::size_t, std::error_code>>;
};

template <typename T>
concept ImmWriter = requires(T self, std::span<const char> write_buf) {
    { self.Write(write_buf) } -> std::convertible_to<std::expected<std::size_t, std::error_code>>;
};

template <typename T>
concept ImmCloser = requires(T self) {
    { self.Close() } -> std::convertible_to<void>;
};

template <typename T>
concept Reader = AsyncReader<T> || ImmReader<T>;

template <typename T>
concept Writer = AsyncWriter<T> || ImmWriter<T>;

template <typename T>
concept Closer = AsyncCloser<T> || ImmCloser<T>;

template <typename T>
concept ReaderCloser = Reader<T> && Closer<T>;

template <typename T>
concept WriterCloser = Writer<T> && Closer<T>;

template <typename T>
concept ReaderWriterCloser = Reader<T> && Writer<T> && Closer<T>;

}  // namespace pat::io

namespace std {
template <>
struct is_error_code_enum<pat::io::IoError> : true_type {};
}  // namespace std