load("//tools/install:install.bzl", "install", "install_src_files")

install(
    name = "install",
    data = [
        "ZL1.BUILD",
        "cyberfile.xml",
    ],
    #if you have build other module,you should add it in the deps.Such as "//cyber_demo/xxx:install"
    deps = [
        "//ZL1/control:install",
        "//ZL1/camera:install",
        "//ZL1/radar:install",
        "//ZL1/monitor:install",
        "//ZL1/collect:install",
        "//ZL1/perception:install",
        "//ZL1/planning:install",
    ],
)

install_src_files(
    name = "install_src",
    src_dir = ["."],
    dest = "ZL1/src",
    filter = "*",
    deps = [
        "//ZL1/control:install_src",
        "//ZL1/camera:install_src",
        "//ZL1/radar:install_src",
        "//ZL1/monitor:install_src",
        "//ZL1/collect:install_src",
        "//ZL1/perception:install_src",
        "//ZL1/planning:install_src",
    ]
)