load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/install:install.bzl", "install", "install_src_files")
load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "perception_lib",
    srcs = ["perception.cc"],
    hdrs = ["perception.h"],
    visibility = ["//visibility:private"],
    deps = [
        "//cyber",
        "//ZL1/proto:examples_cc_proto",
        "//third_libs/gflags",
        "//third_libs/mkldnn",
        "//third_libs/tensorrt7.2",
        "//third_libs/mklml",
        "//third_libs/paddle",
        "//third_libs/paddle_deploy",
        "//third_libs/yaml-cpp",
    ],
    linkopts = ["-ldl","-lrt","-lgomp","-lz","-lm","-lpthread"],
    copts = ["-Wno-sign-compare"],
    alwayslink = True,
)

cc_binary(
    name = "lib_perception.so",
    linkshared = True,
    linkstatic = True,
    deps = [":perception_lib"],
)

filegroup(
    name = "conf",
    srcs = [
        ":perception.dag",
        ":perception.launch",
    ],
)

install(
    name = "perception_install",
    data = [
        ":conf",
    ],
    data_dest = "ZL1/perception/conf",
    library_dest = "ZL1/lib",
    targets = [
        ":lib_perception.so",
    ],
)

install(
    name = "install",
    deps = [
        ":perception_install",
    ]
)

install_src_files(
    name = "install_src",
    src_dir = ["."],
    dest = "ZL1/src/cyberatest",
    filter = "*",
)