cmake_minimum_required(VERSION 3.10)

# set the project name
project(ch8)

# SDL2
if (WIN32)
set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include")
set(SDL2_LIBRARIES "${SDL2_DIR}/lib/x64/SDL2.lib;${SDL2_DIR}/lib/x64/SDL2main.lib")
else()
find_package(SDL2 REQUIRED)
endif()

include_directories(${SDL2_INCLUDE_DIRS} include)

# add the executable
set(HEADERS "include/ch8_cpu.h" "include/display.h" "include/opcodes.h" "include/log.h" "include/input.h")
add_executable(ch8 ${HEADERS} "src/main.c" "src/ch8_cpu.c" "src/opcodes.c" "src/display.c" "src/log.c" "src/input.c")
target_link_libraries(ch8 ${SDL2_LIBRARIES})

# add test executable
add_executable(test_opcodes ${HEADERS} "test/vendor/unity.c" "test/test_opcodes.c" "src/opcodes.c" "test/log.c")

enable_testing()
add_test(opcodes test_opcodes)
