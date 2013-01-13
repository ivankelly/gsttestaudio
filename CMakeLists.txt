cmake_minimum_required(VERSION 2.6)
project(testaudio_gstreamer)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_C_FLAGS "-O0 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb")
set(CMAKE_CXX_FLAGS "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O0 -ggdb")

enable_testing()

find_package(PkgConfig REQUIRED)

pkg_check_modules (GST gstreamer-1.0)
pkg_check_modules (GSTAUDIO gstreamer-audio-1.0)

include_directories (${GST_INCLUDE_DIRS} ${GSTAUDIO_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS} ${GSTAUDIO_LIBRARY_DIRS})

add_subdirectory(src)
