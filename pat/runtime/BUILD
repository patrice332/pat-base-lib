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
        "@com_github_libuv//:libuv",
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
        "write.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        "@com_facebook_libunifex//:libunifex",
        "@com_github_libuv//:libuv",
    ],
)
