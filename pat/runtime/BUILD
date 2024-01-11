load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "promise",
    srcs = [
        "promise.cc",
        "promise_pimpl.cc",
    ],
    hdrs = [
        "promise.h",
        "promise_pimpl.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@libunifex",
    ],
)

cc_library(
    name = "libuv_errors",
    srcs = [
        "libuv_errors.cc",
    ],
    hdrs = [
        "libuv_errors.h",
    ],
    deps = [
        "@libuv",
    ],
)

cc_library(
    name = "io_context",
    srcs = [
        "io_context.cc",
    ],
    hdrs = [
        "io_context.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":libuv_errors",
        ":promise",
        ":stream_close",
        "@libunifex",
    ],
)

cc_library(
    name = "tcp_connect",
    srcs = [
        "tcp_connect.cc",
    ],
    hdrs = [
        "tcp_connect.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        "//pat/net:ip",
        "@libuv",
    ],
)

cc_library(
    name = "stream",
    srcs = [
        "stream.cc",
        "stream_read.cc",
        "stream_write.cc",
    ],
    hdrs = [
        "stream.h",
        "stream_read.h",
        "stream_write.h",
    ],
    deps = [
        ":io_context",
        ":stream_close",
        "//pat/io",
    ],
)

cc_library(
    name = "stream_close",
    srcs = [
        "stream_close.cc",
    ],
    hdrs = [
        "stream_close.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":libuv_errors",
        "@libunifex",
        "@libuv",
    ],
)

cc_library(
    name = "context",
    srcs = [
        "context.cc",
    ],
    hdrs = [
        "context.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        "@libunifex",
    ],
)

cc_library(
    name = "getaddrinfo",
    srcs = [
        "getaddrinfo.cc",
    ],
    hdrs = [
        "getaddrinfo.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":libuv_errors",
        "@libunifex",
        "@libuv",
    ],
)

cc_library(
    name = "tcp_socket",
    srcs = [
        "tcp_socket.cc",
    ],
    hdrs = [
        "tcp_socket.h",
    ],
    deps = [
        ":promise",
        ":stream",
        ":tcp_connect",
        "//pat/net:ip",
        "@libunifex",
        "@libuv",
    ],
)

cc_library(
    name = "runtime",
    srcs = [
        "close.cc",
        "file.cc",
        "open.cc",
        "read.cc",
        "write.cc",
    ],
    hdrs = [
        "close.h",
        "file.h",
        "file_fwd.h",
        "open.h",
        "read.h",
        "write.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":context",
        ":getaddrinfo",
        ":io_context",
        ":promise",
        ":stream",
        ":tcp_connect",
        ":tcp_socket",
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
