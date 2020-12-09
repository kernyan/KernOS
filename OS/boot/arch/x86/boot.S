.set MULTIBOOT_MAGIC, 0x1badb002 #/* byte sequence to identify bootload, 1badboot :) */
.set MULTIBOOT_PAGE_ALIGN, 0x1   #/* align boot modules on i386 page (4KB) boundaries */
.set MULTIBOOT_MEM_INFO, 0x2     #/* pass memory info to OS */
.set MULTIBOOT_FLAGS, MULTIBOOT_PAGE_ALIGN #| MULTIBOOT_MEM_INFO
.set CHECKSUM, -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS) #/* to prove is multiboot */

.set KSTACK_SIZE, 4096*256     // 1 MB
.set KHEAP_SIZE, 4096*256      // 1 MB
.set KPAGETABLE_SIZE, 4096*3   // 12 KiB (1 Kernel Page Directory, and Kernel Page Frame 0, 1)

/* header for kernel */
.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long CHECKSUM

.section .stack, "aw", @nobits /* aw: allocatable/writable, nobits: space only exist on runtime */
stack_bottom:
.skip KSTACK_SIZE
stack_top:

.section .kheap_mem, "aw", @nobits /* aw: allocatable/writable, nobits: space only exist on runtime*/
.align 4096
kheap_start:
.skip KHEAP_SIZE

.section .page_tables
.align 4096
page_tables_start:
.skip KPAGETABLE_SIZE

.section .data
.extern kheap
.extern kpagetable

.section .text

.global _start
.type _start, @function

.extern kernel_main
.type kernel_main, @function

.extern multiboot_info_ptr
.type multiboot_info_ptr, @object

_start:
  cli /* clear interrupt flag */
  cld /* clear direction flag, for deciding convention in copying bits*/

  mov $stack_top, %esp /* set up stack pointer */
  and $-16, %esp       /* align stack pointer to 16 bits (%esp AND FFFFFFF0) */

  mov %ebx, multiboot_info_ptr

  movl $page_tables_start, kpagetable
  movl $kheap_start, kheap

  call kernel_main

  cli

loop:
  hlt
  jmp loop

.size _start, . - _start
  
