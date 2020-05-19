cmake_minimum_required(VERSION 3.10)
project(KernOS VERSION 0.1.0 LANGUAGES C CXX ASM)

# set toolchain
set(TOOLCHAIN_PREFIX   $ENV{HOME}/opt/cross/bin/i686-elf-)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}as)
set(CMAKE_LINKER       ${TOOLCHAIN_PREFIX}ld)
set(CMAKE_ASM_OBJECT_FORMAT elf) 

# linker configuration
set(LDSCRIPT ${CMAKE_CURRENT_LIST_DIR}/compile/linker.ld)
list(APPEND CMAKE_EXE_LINKER_FLAGS "-ffreestanding -Og -nostdlib \
  -T${LDSCRIPT}")

# architecture
set(PLATFORM arch/x86/)
option(32_BITS "Address size is 32 bits at configure time." OFF) # off indicates 64 bits

if(32_BITS)
  list(APPEND BUILD_DEF "ARCH_32=1;")
else()
  list(APPEND BUILD_DEF "ARCH_32=0;")
endif()

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

# include all src and header directories in include search path
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# collect source files by modules

## Module: boot
set(BootFile ${CMAKE_CURRENT_LIST_DIR}/boot/${PLATFORM}/boot.S)

## Module: kernel
set(KernelFiles
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/kernel.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/utilities.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/vga.cpp
  )
add_library(MOD_kernel STATIC ${KernelFiles})
target_compile_options(MOD_kernel PRIVATE -c -Og -Wall -Wextra -ggdb -fno-exceptions -fno-rtti -ffreestanding)
set_target_properties(MOD_kernel PROPERTIES COMPILE_DEFINITIONS ${BUILD_DEF})

# main executable
add_executable(kern.bin ${BootFile})
target_link_libraries(kern.bin MOD_kernel)

# copy compiler flags for ycm-completer
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  add_custom_command(
    TARGET kern.bin
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_LIST_DIR}/compile_commands.json
    )
endif()
