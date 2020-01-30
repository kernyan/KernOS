.section .stack, "aw", @nobits /* aw: allocatable/writable, nobits: space onlu exist on runtime */
stack_bottom:
.skip 32768
stack_top:

.section .text

.global start
.type start, @function

.extern init
.type init, @function

start:
  cli /* clear interrupt flag */
  cld /* clear direction flag, for deciding convention in copying bits*/

  mov $stack_top, %esp /* set up stack pointer */
  and $-16, %esp       /* align stack pointer to 16 bits (%esp AND FFFFFFF0) */
  
  call init
