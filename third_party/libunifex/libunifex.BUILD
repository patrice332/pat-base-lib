load("@rules_foreign_cc//foreign_cc:defs.bzl", "cmake")

filegroup(
    name = "srcs",
    srcs = glob([
        "*",
        "cmake/**/*",
        "include/**/*",
        "source/**/*",
    ]),
)

cmake(
    name = "libunifex",
    cache_entries = {
        "BUILD_TESTING": "OFF",
        "UNIFEX_BUILD_EXAMPLES": "OFF",
    },
    generate_args = ["-GNinja"],
    includes = ["include"],
    lib_source = ":srcs",
    visibility = ["//visibility:public"],
)
