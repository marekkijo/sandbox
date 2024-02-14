include_directories(${CMAKE_SOURCE_DIR})

add_library(wolf_commonlib STATIC
        common_lib/map_renderer.cpp
        common_lib/map_renderer.hpp
        common_lib/player_state.cpp
        common_lib/player_state.hpp
        common_lib/raw_map.cpp
        common_lib/raw_map.hpp
        common_lib/raw_map_from_ascii.cpp
        common_lib/raw_map_from_ascii.hpp
        common_lib/raw_map_from_wolf.cpp
        common_lib/raw_map_from_wolf.hpp
        common_lib/renderer.cpp
        common_lib/renderer.hpp
        common_lib/vector_map.cpp
        common_lib/vector_map.hpp
        common_lib/wolf_map_info.hpp
        common_lib/wolf_renderer.cpp
        common_lib/wolf_renderer.hpp
)

target_link_libraries(wolf_commonlib tools_math)
target_link_libraries(wolf_commonlib tools_sdl)
target_link_libraries(wolf_commonlib SDL2::SDL2)
target_link_libraries(wolf_commonlib glm::glm)

add_executable(wolf_singlethread
        singlethread/main.cpp
        singlethread/wolf_renderer_singlethread.cpp
        singlethread/wolf_renderer_singlethread.hpp
)

add_custom_command(TARGET wolf_singlethread PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/projects/wolf/data $<TARGET_FILE_DIR:wolf_singlethread>/data)

target_link_libraries(wolf_singlethread wolf_commonlib)
target_link_libraries(wolf_singlethread tools_math)
target_link_libraries(wolf_singlethread tools_sdl)
target_link_libraries(wolf_singlethread SDL2::SDL2)
target_link_libraries(wolf_singlethread SDL2::SDL2main)
target_link_libraries(wolf_singlethread glm::glm)
target_link_libraries(wolf_singlethread Boost::program_options)

add_executable(wolf_multithread
        multithread/main.cpp
        multithread/wolf_renderer_multithread.cpp
        multithread/wolf_renderer_multithread.hpp
)

add_custom_command(TARGET wolf_multithread PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/projects/wolf/data $<TARGET_FILE_DIR:wolf_multithread>/data)

target_link_libraries(wolf_multithread wolf_commonlib)
target_link_libraries(wolf_multithread tools_math)
target_link_libraries(wolf_multithread tools_sdl)
target_link_libraries(wolf_multithread SDL2::SDL2)
target_link_libraries(wolf_multithread SDL2::SDL2main)
target_link_libraries(wolf_multithread glm::glm)
target_link_libraries(wolf_multithread Boost::program_options)
