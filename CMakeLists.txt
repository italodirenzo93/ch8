cmake_minimum_required(VERSION 3.10)

# set the project name
project(ch8)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} include)

# add the executable
add_executable(ch8 src/main.c src/ch8_cpu.c src/opcodes.c src/display_sdl.c)
target_link_libraries(ch8 ${SDL2_LIBRARIES})

# add test executable
add_executable(test_opcodes test/vendor/unity.c test/test_opcodes.c src/opcodes.c)
