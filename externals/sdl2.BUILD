load("@rules_cc//cc:defs.bzl", "cc_import", "cc_library")

cc_import(
    name = "sdl2_dll",
    shared_library = "lib/x64/SDL2.dll",
)

cc_library(
    name = "sdl2_hdrs",
    hdrs = glob([
        "include/**/*.h",
    ]),
    include_prefix = "SDL2",
    strip_include_prefix = "include",
)

cc_library(
    name = "sdl2_lib",
    srcs = [
        "lib/x64/SDL2.lib",
        "lib/x64/SDL2main.lib",
    ],
    linkopts = [
        "-DEFAULTLIB:shell32.lib",
    ],
)

cc_library(
    name = "sdl2",
    visibility = ["//visibility:public"],
    deps = [
        ":sdl2_dll",
        ":sdl2_hdrs",
        ":sdl2_lib",
    ],
)
