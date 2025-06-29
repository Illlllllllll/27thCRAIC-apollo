load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/install:install.bzl", "install", "install_src_files")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "collect_lib",
    srcs = ["collect.cc"],
    hdrs = ["collect.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cyber",
        "//ZL1/proto:examples_cc_proto",
        "//third_libs/opencv3.2",
    ],
    alwayslink = True,
)

cc_binary(
    name = "lib_collect.so",
    linkshared = True,
    linkstatic = True,
    deps = [":collect_lib"],
)

filegroup(
    name = "conf",
    srcs = [
        ":collect.dag",
        ":collect.launch",
    ],
)

install(
    name = "collect_install",
    data = [
        ":conf",
    ],
    data_dest = "ZL1/collect/conf",
    library_dest = "ZL1/lib",
    targets = [
        ":lib_collect.so",
    ],
)

install(
    name = "install",
    deps = [
        ":collect_install",
    ]
)

install_src_files(
    name = "install_src",
    src_dir = ["."],
    dest = "ZL1/src/cyberatest",
    filter = "*",
)