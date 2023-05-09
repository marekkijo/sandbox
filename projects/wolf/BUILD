load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "wolf",
    srcs = [
        "main.cpp",
        "player_state.cpp",
        "player_state.hpp",
        "raw_map.cpp",
        "raw_map.hpp",
        "vector_map.cpp",
        "vector_map.hpp",
    ],
    data = glob(["data/**"]),
    deps = [
        "//tools:math",
        "//tools:sdl",
        "@sdl2",
    ],
)
