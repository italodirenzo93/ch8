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

include_directories(${SDL2_INCLUDE_DIRS})

# add the executable
set(HEADERS "src/ch8_cpu.h" "src/ch8_display.h" "src/ch8_opcodes.h" "src/ch8_log.h" "src/ch8_keyboard.h" "src/ch8_audio.h" "src/ch8_util.h" "src/ch8_def.h")
add_executable(ch8 ${HEADERS} "src/main.c" "src/ch8_cpu.c" "src/ch8_opcodes.c" "src/ch8_display.c" "src/ch8_log.c" "src/ch8_keyboard.c" "src/ch8_audio.c")
target_link_libraries(ch8 ${SDL2_LIBRARIES})

# copy test ROM file
file(COPY "assets" DESTINATION "${CMAKE_BINARY_DIR}")

# add test executable
add_executable(test_opcodes "test/vendor/unity.c" "test/test_opcodes.c" "test/test_stubs.c" "src/ch8_opcodes.c" "src/ch8_cpu.c")

enable_testing()
add_test(opcodes test_opcodes)
