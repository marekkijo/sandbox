add_library(my_lib STATIC my_test.cpp my_test.hpp)

target_include_directories(my_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
