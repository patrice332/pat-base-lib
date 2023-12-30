load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "io_context",
    srcs = [
        "io_context.cc",
    ],
    hdrs = [
        "io_context.h",
    ],
    implementation_deps = [
        "@libuv",
    ],
)

cc_library(
    name = "runtime",
    srcs = [
        "close.cc",
        "file.cc",
        "getaddrinfo.cc",
        "open.cc",
        "promise.cc",
        "read.cc",
        "stream.cc",
        "stream_close.cc",
        "stream_read.cc",
        "stream_write.cc",
        "tcp_connect.cc",
        "tcp_socket.cc",
        "write.cc",
    ],
    hdrs = [
        "close.h",
        "file.h",
        "file_fwd.h",
        "getaddrinfo.h",
        "open.h",
        "promise.h",
        "read.h",
        "stream.h",
        "stream_close.h",
        "stream_read.h",
        "stream_write.h",
        "tcp_connect.h",
        "tcp_socket.h",
        "write.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        "//pat/io",
        "@libunifex",
        "@libuv",
    ],
)
