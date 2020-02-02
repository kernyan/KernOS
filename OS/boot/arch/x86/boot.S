.set MULTIBOOT_MAGIC, 0x1badb002 #/* byte sequence to identify bootload, 1badboot :) */
.set MULTIBOOT_PAGE_ALIGN, 0x1   #/* align boot modules on i386 page (4KB) boundaries */
.set MULTIBOOT_MEM_INFO, 0x2     #/* pass memory info to OS */
.set MULTIBOOT_FLAGS, MULTIBOOT_PAGE_ALIGN #| MULTIBOOT_MEM_INFO
.set CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) #/* to prove is multiboot */

/* header for kernel */
.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long CHECKSUM

.section .stack, "aw", @nobits /* aw: allocatable/writable, nobits: space onlu exist on runtime */
stack_bottom:
.skip 32768 # 32 KiB
stack_top:

.section .text

.global _start
.type _start, @function

.extern kernel_main
.type kernel_main, @function

_start:
  cli /* clear interrupt flag */
  cld /* clear direction flag, for deciding convention in copying bits*/

  mov $stack_top, %esp /* set up stack pointer */
  and $-16, %esp       /* align stack pointer to 16 bits (%esp AND FFFFFFF0) */

  call kernel_main

  cli

loop:
  hlt
  jmp loop

.size _start, . - _start
  
