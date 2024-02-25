cmake_minimum_required(VERSION 3.10)  # Minimum version of CMake required

# Set your project name and version
project(prompter VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)  # Using C++17 standard
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable target with source files
add_executable(prompter main.cpp)
