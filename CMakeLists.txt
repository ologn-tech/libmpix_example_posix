# SPDX-License-Identifier: Apache-2.0

# Example CMake project for using libmpix on a POSIX system such as Linux, BSD, Mac OSX...

cmake_minimum_required(VERSION 3.20)

project(posix_example)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory($ENV{LIBMPIX}/ports/posix libmpix)

add_executable(posix_example main.c)

target_link_libraries(posix_example PUBLIC libmpix)
