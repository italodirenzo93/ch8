cmake_minimum_required(VERSION 3.10)

# set the project name
project(ch8)

# SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# add the executable
add_executable(ch8 main.c ch8_cpu.c ch8_gpu.c util.c display.c)
target_link_libraries(ch8 ${SDL2_LIBRARIES})
