cmake_minimum_required(VERSION 3.7.0)
project(Parameters)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_APPENDIX "-dev.1")
set(PROJECT_COPYRIGHT "Copyright (C) 2018 Christian Mayer <https://fox21.at>")

set(CMAKE_CXX_FLAGS "-Wall -Wshadow")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wno-unused-variable -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wunused-variable")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Set minimum C++ to 2014 standards.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default install path for Debian.
set(CMAKE_INSTALL_PREFIX /usr)

configure_file("${PROJECT_SOURCE_DIR}/src/config.hpp.in" "${PROJECT_SOURCE_DIR}/src/config.hpp" NEWLINE_STYLE LF)

add_executable(parameters
    src/app/parameters.cpp
    src/class/env_var.cpp src/class/env_var.hpp)
target_include_directories(parameters PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_compile_features(parameters PRIVATE cxx_noexcept)
get_target_property(parameters_std parameters CXX_STANDARD)

# Install
#install(TARGETS parameters)

# Term Color
find_package(termcolor QUIET)

# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED OFF)
find_package(Boost 1.62.0 COMPONENTS filesystem program_options)
if (Boost_FOUND)
    message(STATUS "Found Boost")
    message(STATUS "Boost INCL ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost LIBS ${Boost_LIBRARIES}")
    target_include_directories(parameters PRIVATE ${Boost_INCLUDE_DIRS})
endif ()

target_link_libraries(parameters ${Boost_LIBRARIES})

message(STATUS "CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX_STANDARD ${parameters_std}")
message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
