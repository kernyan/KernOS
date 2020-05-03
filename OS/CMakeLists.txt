cmake_minimum_required(VERSION 3.10)
project(KernOS VERSION 0.1.0 LANGUAGES C CXX)

# set toolchain
set(TOOLCHAIN_PREFIX i686-elf-)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}as)
set(CMAKE_LINKER       ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_ASM_OBJECT_FORMAT elf) 

# architecture
set(ARCH_CONFIG arch/x86/)

# options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(USE_DEBUG "Compile debug if enable." ON)
option(BUILD_TESTS "Compile tests if enable." ON)

# build type
if (USE_DEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
else ()
  set(CMAKE_BUILD_TYPE "Release")
endif()

# downlaod and unpack googletest at configure time

message(STATUS "CMake_command: ${CMAKE_COMMAND}")
message(STATUS "CMake_generator: ${CMAKE_GENERATOR}")

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND}
  -g "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
  )

if (result)
  message(FATAL_ERROR "CMake step for gtest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -- build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download
  )

if (result)
  message(FATAL_ERROR "Build step for gtest failed: ${result}")
endif()

# add googletest directly to project
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL
  )

# include modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(TestFunctions)

# include all src and header directories in include search path
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)

# collect source files by modules
## boot
set(BootFile ${CMAKE_CURRENT_LIST_DIR}/boot/${ARCH_CONFIG}/boot.S)

## kernel
add_library(MOD_kernel PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${ARCH_CONFIG}/kernel.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${ARCH_CONFIG}/utilities.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${ARCH_CONFIG}/vga.cpp
  )

# ctest
enable_testing()

# build tests
if (BUILD_TESTS)
  add_utest(VGATest "${CMAKE_CURRENT_LIST_DIR}/test/display/VGATest.cpp" MOD_kernel)
endif()

# main executable
add_executable(kern.bin ${BootFile})
target_link_libraries(kern.bin ${MOD_boot})

# copy compiler flags for ycm-completer
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_command(
    TARGET main
    POST_BUILD
    COMMAND ${cmake_command} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    )
endif()
