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

# compile flags
set(COMPILE_FLAGS "-std=c++20;-c;-O0;-Wall;-Wextra;-ggdb;-fno-exceptions;-fno-rtti;-ffreestanding;")

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

# build type
if (USE_DEBUG)
  set(CMAKE_BUILD_TYPE "Debug")
  list(APPEND BUILD_DEF "DEBUG=1")
else ()
  set(CMAKE_BUILD_TYPE "Release")
  list(APPEND BUILD_DEF "DEBUG=0")
endif()

# include all src and header directories in include search path
include_directories(${CMAKE_CURRENT_LIST_DIR}/..)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/KLib/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/test)
include_directories(${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM})

# collect source files by modules

## Module: boot
set(BootFile ${CMAKE_CURRENT_LIST_DIR}/boot/${PLATFORM}/boot.S)

## Module: kernel
set(KernelFiles
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/kernel.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/utilities.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/vga.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/cpu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/interrupt.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/gdt.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/global.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/kprintf.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/memoryallocator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/virtualmemory.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/pit.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/pic.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/process.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/pci.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/sata.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/ahci.cpp
  ${CMAKE_CURRENT_LIST_DIR}/kernel/${PLATFORM}/ide.cpp
  )
add_library(MOD_kernel STATIC ${KernelFiles})
target_compile_options(MOD_kernel PRIVATE ${COMPILE_FLAGS})
set_target_properties(MOD_kernel PROPERTIES COMPILE_DEFINITIONS "${BUILD_DEF}")

## Module: test
set(TestFiles ${CMAKE_CURRENT_LIST_DIR}/test/test.cpp)
add_library(MOD_test STATIC ${TestFiles})
target_compile_options(MOD_test PRIVATE ${COMPILE_FLAGS})
set_target_properties(MOD_test PROPERTIES COMPILE_DEFINITIONS "${BUILD_DEF}")

# main executable
add_executable(kern.bin ${BootFile})
target_link_libraries(kern.bin MOD_kernel MOD_test gcc)

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
