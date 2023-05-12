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
        "sdl/sdl_animation.cpp",
        "sdl/sdl_keyboard_state.cpp",
        "sdl/sdl_renderer.cpp",
        "sdl/sdl_system.cpp",
        "sdl/sdl_window.cpp",
    ],
    hdrs = [
        "sdl/sdl_animation.hpp",
        "sdl/sdl_keyboard_state.hpp",
        "sdl/sdl_renderer.hpp",
        "sdl/sdl_system.hpp",
        "sdl/sdl_window.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@sdl2",
    ],
)
