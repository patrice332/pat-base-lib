load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ip",
    srcs = [
        "ip.cc",
    ],
    hdrs = [
        "ip.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//pat/runtime:context",
        "//pat/runtime:getaddrinfo",
        "@libunifex",
    ],
)
