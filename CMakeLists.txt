cmake_minimum_required(VERSION 3.20)
project(mcrt-intro LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++ version and std
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# OpenMP
find_package(OpenMP REQUIRED)

# externals
add_subdirectory("externals/")

# 1-classical-raytracer
add_executable(1-classical-raytracer "1-classical-raytracer/main.cpp")
target_include_directories(1-classical-raytracer PUBLIC "shared")
target_link_libraries(1-classical-raytracer PUBLIC OpenMP::OpenMP_CXX)