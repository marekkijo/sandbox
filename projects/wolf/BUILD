load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "wolf",
    srcs = [
        "main.cpp",
        "map_renderer.cpp",
        "map_renderer.hpp",
        "player_state.cpp",
        "player_state.hpp",
        "raw_map.cpp",
        "raw_map.hpp",
        "renderer.cpp",
        "renderer.hpp",
        "vector_map.cpp",
        "vector_map.hpp",
        "wolf_renderer.cpp",
        "wolf_renderer.hpp",
    ],
    data = glob(["data/**"]),
    deps = [
        "//tools:math",
        "//tools:sdl",
        "@glm",
        "@sdl2",
    ],
)
