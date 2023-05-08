load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "math",
    srcs = ["math/math.cpp"],
    hdrs = ["math/math.hpp"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "sdl",
    srcs = [
        "sdl/sdl.cpp",
        "sdl/sdl_keyboard_state.cpp",
    ],
    hdrs = [
        "sdl/sdl.hpp",
        "sdl/sdl_keyboard_state.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@sdl2",
    ],
)
