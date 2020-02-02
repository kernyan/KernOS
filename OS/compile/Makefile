BOOTIMAGE := kern.bin

PLATFORM = x86
TOOLCHAIN_PREFIX := i686-elf-

TOPDIR := ..

CXXFLAGS += -c -Og -Wall -Wall -Wextra -ggdb
CXXFLAGS += -fno-exceptions -fno-rtti -ffreestanding

LDFLAGS := -ffreestanding -Og -nostdlib

INCLUDE := -I$(TOPDIR)/include -I$(TOPDIR)/kernel/arch/$(PLATFORM) -I$(TOPDIR)

CXXFLAGS += $(INCLUDE)

LDSCRIPT := linker.ld

LDFLAGS += -T$(LDSCRIPT)

LDLIBS := -lgcc

CC := $(TOOLCHAIN_PREFIX)g++
LD := $(TOOLCHAIN_PREFIX)ld
AS := $(TOOLCHAIN_PREFIX)as

ifeq ($(VERBOSE),)
.SILENT:
endif

SYSCOMPS := boot/arch/$(PLATFORM) \
            kernel/arch/$(PLATFORM)

COMPS := $(SYSCOMPS)

COMP_SRC := 
include $(COMPS:%=$(TOPDIR)/%/Makerules)

COMP_SRC_S := $(filter %S,$(COMP_SRC))
COMP_SRC_CPP := $(filter %cpp,$(COMP_SRC))
	
COMP_OBJ_S := $(patsubst %S,%o, $(COMP_SRC_S))
COMP_OBJ_CPP :=	$(patsubst %cpp,%o,$(COMP_SRC_CPP))

$(BOOTIMAGE): $(COMP_OBJ_S) $(COMP_OBJ_CPP)
	@echo -e "\tLinking" $@ "with " $^
	$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBD)

$(COMP_OBJ_CPP): %.o : %.cpp
	$(CC) $(CXXFLAGS) -o $@ $^

$(COMP_OBJ_S): %.o : %.S
	$(AS) -o $@ $<

clean:
	@echo -e "\tCleaning all objects"
	rm -f *.o $(COMP_OBJ_CPP)
	rm -f *.o $(COMP_OBJ_S)
	rm -f kern.bin

.DEFAULT_GOAL := $(BOOTIMAGE)
