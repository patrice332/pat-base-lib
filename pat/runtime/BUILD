load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "runtime",
    srcs = [
        "libuv_context.cc",
        "libuv_file.cc",
    ],
    hdrs = [
        "libuv_context.h",
        "libuv_file.h",
    ],
    implementation_deps = [
        "@com_github_libuv//:libuv",
    ],
    deps = [
        "@com_facebook_libunifex//:libunifex",
    ],
)
