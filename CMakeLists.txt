cmake_minimum_required(VERSION 3.8.0)

project(Parameters VERSION 1.0.0 LANGUAGES CXX)

set(PROJECT_VERSION_APPENDIX "-rc.3.dev")
set(PROJECT_VERSION_TAG "N/A" CACHE STRING "Git tag string.")
set(PROJECT_VERSION_COMMIT "N/A" CACHE STRING "Git commit string.")
set(PROJECT_COPYRIGHT "Copyright (C) 2018 Christian Mayer <https://fox21.at>")
set(PROJECT_HOMEPAGE_URL "https://github.com/TheFox/wallet-cpp")

set(CMAKE_CXX_FLAGS "-Wall -Wshadow")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wunused-variable -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBIAN "-O3 -Wunused-variable")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Set minimum C++ to 2014 standards.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/src/config.hpp"
    NEWLINE_STYLE LF)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src")

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
endif ()

set(TARGET_PARAMETERS_FILES
    src/app/parameters.cpp
    src/class/env_var.cpp src/class/env_var.hpp)

set(TARGET_PARAMETERS_COMPILE_FEATURES
    cxx_noexcept
    cxx_constexpr
    cxx_final)

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (CMAKE_BUILD_TYPE_LOWER STREQUAL "release"
    OR CMAKE_BUILD_TYPE_LOWER STREQUAL "debug"
    OR CMAKE_BUILD_TYPE_LOWER STREQUAL "debian")
    # Parameters Target
    add_executable(parameters ${TARGET_PARAMETERS_FILES})
    target_include_directories(parameters PRIVATE ${PROJECT_SOURCE_DIR}/src)
    target_include_directories(parameters PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(parameters ${Boost_LIBRARIES})
    target_compile_features(parameters PRIVATE ${TARGET_PARAMETERS_COMPILE_FEATURES})
    target_compile_definitions(parameters PRIVATE "-DNDEBUG")
    get_target_property(parameters_std parameters CXX_STANDARD)

    # Install Parameters
    install(TARGETS parameters RUNTIME DESTINATION bin)
endif ()

message(STATUS "CXX_STANDARD ${parameters_std}")

message(STATUS "CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_BUILD_TYPE_LOWER ${CMAKE_BUILD_TYPE_LOWER}")
message(STATUS "CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")
