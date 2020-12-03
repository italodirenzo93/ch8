cmake_minimum_required(VERSION 3.10)

# set the project name
project(ch8 CXX)
set(CMAKE_CXX_STANDARD_REQUIRED 11)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

if (MSVC)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# SDL2
if (WIN32)
set(SDL2_INCLUDE_DIRS "${SDL2_DIR}/include")
set(SDL2_LIBRARIES "${SDL2_DIR}/lib/x64/SDL2.lib;${SDL2_DIR}/lib/x64/SDL2main.lib")
else()
find_package(SDL2 REQUIRED)
endif()

include_directories(${SDL2_INCLUDE_DIRS} include)

# add the executable
set(HEADERS "include/Cpu.hpp"  "include/Log.hpp")
add_executable(ch8 ${HEADERS} "src/Main.cpp"  "src/Cpu.cpp" "src/Log.cpp" )
target_link_libraries(ch8 ${SDL2_LIBRARIES})

# add test executable

