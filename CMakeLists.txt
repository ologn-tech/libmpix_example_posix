# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20)

project(posix_example)

set(LIBMPIX_DIR $ENV{LIBMPIX})
add_subdirectory(${LIBMPIX_DIR}/ports/posix libmpix)
add_executable(posix_example main.c)
target_link_libraries(posix_example PUBLIC libmpix)
