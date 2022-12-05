# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required(VERSION 3.15)

include(FetchContent)

set(CMAKE_CXX_COMPILER clang++-14)
set(CMAKE_C_COMPILER clang-14)

# set(CMAKE_CXX_COMPILER g++)
# set(CMAKE_C_COMPILER gcc)

project(demo VERSION 0.00 LANGUAGES CXX C ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

FetchContent_Declare(
  trieste
  GIT_REPOSITORY https://github.com/microsoft/trieste
  GIT_TAG        ee45b2c78192a879ad5dbdea124c1e98589f3d09
)

FetchContent_GetProperties(trieste)
if(NOT trieste_POPULATED)
  set(TRIESTE_BUILD_SAMPLES OFF)
  FetchContent_Populate(trieste)
  add_subdirectory(${trieste_SOURCE_DIR} ${trieste_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(demo main.cpp)

target_link_libraries(demo Threads::Threads CLI11::CLI11 trieste::trieste)

enable_testing()
