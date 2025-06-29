load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")
load("//tools/install:install.bzl", "install", "install_src_files")
load("//tools:cpplint.bzl", "cpplint")
package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "camera",
    srcs = ["camera.cc"],
    deps = [
        "//ZL1/proto:examples_cc_proto",
        "//third_libs/opencv3.2",
    ], 
)

install(
    name = "install",
    runtime_dest = "ZL1/bin",
    targets = [
        ":camera"
    ],
)

install_src_files(
    name = "install_src",
    src_dir = ["."],
    dest = "ZL1/src/cyberatest",
    filter = "*",
)