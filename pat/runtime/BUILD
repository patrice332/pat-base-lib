load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "promise",
    srcs = [
        "promise.cc",
    ],
    hdrs = [
        "promise.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@libunifex",
    ],
)

cc_library(
    name = "io_context",
    srcs = [
        "io_context.cc",
        "libuv_errors.cc",
        "stream_close.cc",
    ],
    hdrs = [
        "io_context.h",
        "libuv_errors.h",
        "stream_close.h",
    ],
    implementation_deps = [
        "@libuv",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":promise",
        "@libunifex",
    ],
)

cc_library(
    name = "runtime",
    srcs = [
        "close.cc",
        "context.cc",
        "file.cc",
        "getaddrinfo.cc",
        "open.cc",
        "read.cc",
        "stream.cc",
        "stream_read.cc",
        "stream_write.cc",
        "tcp_connect.cc",
        "tcp_socket.cc",
        "write.cc",
    ],
    hdrs = [
        "close.h",
        "context.h",
        "file.h",
        "file_fwd.h",
        "getaddrinfo.h",
        "open.h",
        "read.h",
        "stream.h",
        "stream_read.h",
        "stream_write.h",
        "tcp_connect.h",
        "tcp_socket.h",
        "write.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        ":promise",
        "//pat/io",
        "@libunifex",
        "@libuv",
    ],
)

cc_test(
    name = "tcp_socket_test",
    srcs = [
        "tcp_socket_test.cc",
    ],
    deps = [
        ":runtime",
        "//pat/io",
        "@googletest//:gtest_main",
    ],
)
