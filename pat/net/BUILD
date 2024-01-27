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

cc_library(
    name = "ipaddr",
    srcs = [
        "ipaddr.cc",
    ],
    hdrs = [
        "ipaddr.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":ip",
    ],
)

cc_library(
    name = "tcp_conn",
    srcs = [
        "tcp_conn.cc",
    ],
    hdrs = [
        "tcp_conn.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":ipaddr",
        "//pat/runtime:tcp_socket",
        "@libunifex",
    ],
)
