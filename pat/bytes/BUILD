load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

cc_library(
    name = "buffer",
    srcs = [
        "buffer.cc",
    ],
    hdrs = [
        "buffer.h",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "buffer_test",
    srcs = [
        "buffer_test.cc",
    ],
    deps = [
        ":buffer",
        "//pat/io",
        "@googletest//:gtest_main",
    ],
)
