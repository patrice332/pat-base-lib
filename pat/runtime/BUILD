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
        "file.cc",
        "open.cc",
    ],
    hdrs = [
        "file.h",
        "file_fwd.h",
        "open.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":io_context",
        "@com_facebook_libunifex//:libunifex",
        "@com_github_libuv//:libuv",
    ],
)
