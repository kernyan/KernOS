/*! @brief linker script to specify kernel binary's section locations
 *  @see <a href="https://wiki.osdev.org/Bare_Bones#Linking_the_Kernel">Osdev's Linking the kernel</a>
 */

/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)
 
/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	/* Convention of kernel by the bootloader is 1MB, 
   * while high virtual memory convention for Linux i386 is 3GB, i.e. 0xC000'0000 
   * thus we want to load our section starting at 1MB + 3GB  
   * however we need to enable paging (virtual memory) before jumping to _start
   * for now, we have to be content with loading at 1MB
   */
	. = 0x00100000;
 
	/* First put the multiboot header, as it is required to be put very early
	   early in the image or the bootloader won't recognize the file format.
	   Next we'll put the .text section. */
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.page_tables)
		*(.kheap_mem)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
        /* label start, end of global constructors */
        start_ctors = .;
        *(.ctors)
        end_ctors = .;

        *(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
 
	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
