load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/install:install.bzl", "install", "install_src_files")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "planning_lib",
    srcs = ["planning.cc"],
    hdrs = ["planning.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cyber",
        "//ZL1/proto:examples_cc_proto",
    ],
    alwayslink = True,
)

cc_binary(
    name = "lib_planning.so",
    linkshared = True,
    linkstatic = True,
    deps = [":planning_lib"],
)

filegroup(
    name = "conf",
    srcs = [
        ":planning.dag",
        ":planning.launch",
    ],
)

install(
    name = "planning_install",
    data = [
        ":conf",
    ],
    data_dest = "ZL1/planning/conf",
    library_dest = "ZL1/lib",
    targets = [
        ":lib_planning.so",
    ],
)

install(
    name = "install",
    deps = [
        ":planning_install",
    ]
)

install_src_files(
    name = "install_src",
    src_dir = ["."],
    dest = "ZL1/src/cyberatest",
    filter = "*",
)