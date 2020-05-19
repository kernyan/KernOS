cmake_minimum_required(VERSION 3.10)
project(KernOS VERSION 0.1.0 LANGUAGES C CXX ASM)

# test directory indirection
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/../../OS)

# architecture
set(PLATFORM arch/x86/)

# options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(USE_DEBUG "Compile debug if enable." ON)
option(BUILD_TESTS "Compile tests if enable." ON)
option(REBUILD_GTEST "Update GTest Repo from github if enable." OFF)

# build type
if (USE_DEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
else ()
  set(CMAKE_BUILD_TYPE "Release")
endif()

# download and unpack googletest at configure time
if (REBUILD_GTEST)
  configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND}
    -g "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
    )

  if (result)
    message(FATAL_ERROR "CMake step for gtest failed: ${result}")
  endif()

  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
    )

  if (result)
    message(FATAL_ERROR "Build step for gtest failed: ${result}")
  endif()
endif() # REBUILD_GTEST

# add googletest directly to project
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL
  )

# include modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/CMake)
include(TestFunctions)

# include all src and header directories in include search path
include_directories(${TEST_DIR})
include_directories(${TEST_DIR}/include)
include_directories(${TEST_DIR}/kernel/${PLATFORM})

# collect source files by modules
## Module: kernel
set(KernelFiles 
  ${TEST_DIR}/kernel/${PLATFORM}/utilities.cpp
  ${TEST_DIR}/kernel/${PLATFORM}/vga.cpp
  )
add_library(MOD_kernel STATIC ${KernelFiles})
target_compile_options(MOD_kernel PRIVATE -c -Og -Wall -Wextra -ggdb -fno-exceptions -fno-rtti -ffreestanding)

# ctest
enable_testing()

# build tests
if (BUILD_TESTS)
  add_utest(UtilityTest "${CMAKE_CURRENT_LIST_DIR}/utility/UtilityTest.cpp" ${MOD_kernel})
endif()

# main executable
set(SRC dummy.cpp)
add_executable(dummy ${SRC})

# copy compiler flags for ycm-completer
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_command(
    TARGET dummy
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    )
endif()
