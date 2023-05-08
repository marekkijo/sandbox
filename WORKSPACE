load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "sdl2",
    build_file = "//externals:sdl2.BUILD",
    patch_args = ["-p1"],
    patches = ["//externals:SDL2-devel-2.26.5-VC.patch"],
    sha256 = "446cf6277ff0dd4211e6dc19c1b9015210a72758f53f5034c7e4d6b60e540ecf",
    strip_prefix = "SDL2-2.26.5",
    url = "https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-devel-2.26.5-VC.zip",
)
