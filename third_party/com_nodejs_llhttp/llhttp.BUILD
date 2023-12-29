load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "llhttp",
    srcs = [
        "src/api.c",
        "src/http.c",
        "src/llhttp.c",
    ],
    hdrs = [
        "include/llhttp.h",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
)
