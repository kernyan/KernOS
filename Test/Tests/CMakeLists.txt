cmake_minimum_required(VERSION 3.10)
project(KernOS VERSION 0.1.0 LANGUAGES C CXX ASM)

# test directory indirection
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/../../OS)

# compile flags
list(APPEND CMAKE_CXX_FLAGS "-m32")
set(COMPILE_FLAGS "-O0;-Wall;-Wextra;-ggdb;-m32;")

# architecture
set(PLATFORM arch/x86/)
option(32_BITS "Address size is 32 bits at configure time." ON) # off indicates 64 bits

if(32_BITS)
  list(APPEND BUILD_DEF "ARCH_32=1")
else()
  list(APPEND BUILD_DEF "ARCH_32=0")
endif()

# options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(USE_DEBUG "Compile debug if enable." ON)
option(BUILD_TESTS "Compile tests if enable." ON)
option(REBUILD_GTEST "Update GTest Repo from github if enable." ON)

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
   # ${TEST_DIR}/kernel/${PLATFORM}/kernel.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/utilities.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/vga.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/cpu.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/interrupt.cpp
   # ${TEST_DIR}/kernel/${PLATFORM}/gdt.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/global.cpp
    ${TEST_DIR}/kernel/${PLATFORM}/kprintf.cpp
    )
add_library(MOD_kernel STATIC ${KernelFiles})
target_compile_options(MOD_kernel PRIVATE ${COMPILE_FLAGS})
set_target_properties(MOD_kernel PROPERTIES COMPILE_DEFINITIONS "${BUILD_DEF}")

# ctest
enable_testing()

# build tests
if (BUILD_TESTS)
  add_utest(UtilityTest "${CMAKE_CURRENT_LIST_DIR}/utility_test.cpp" ${MOD_kernel})
  add_utest(kmallocTest "${CMAKE_CURRENT_LIST_DIR}/kmalloc_test.cpp" ${MOD_kernel})
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
