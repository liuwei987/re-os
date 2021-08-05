PWD :=$(shell pwd)
OUT :=out

# Compile tools and flags
CC   := gcc
DD   := dd
AS   := as
AR   := ar
LD   := ld
NASM := nasm
OBJDUMP := objdump
OBJCOPY := objcopy
#TODO: fix the number function check stack failed
CFLAGS  := -Werror -fno-stack-protector -mcmodel=large -fno-builtin -m64 -Wl,-Map=output.map
ASFLAGS := --64
ARFLAGS := crs
LDFLAGS := -b elf64-x86-64 #-t --trace-symbol=
OBJCPYFLAGS := -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary
MAKE    := make

export OUT CC DD AS LD CFLAGS LDFLAGS MAKE

# Compile the source file
MODULES := modules
BOOT_DIR := arch/x86/boot
KERN_DIR := arch/x86/kernel
BOOT_SRC := $(BOOT_DIR)/boot.asm
LOADER_SRC := $(BOOT_DIR)/loader.asm

MOD_LIB := $(OUT)/$(MODULES)/modules.a

LIB_OBJS := $(patsubst %.c, $(OUT)/%.o, $(wildcard $(MODULES)/lib/*.c))
LIB_OBJS += $(patsubst %.c, $(OUT)/%.o, $(wildcard $(MODULES)/printk/*.c))
OBJS := $(patsubst %.S, $(OUT)/%.o, $(wildcard $(KERN_DIR)/*.S))
OBJS += $(patsubst %.c, $(OUT)/%.o, $(wildcard $(KERN_DIR)/*.c))
OBJS += $(patsubst %.c, $(OUT)/%.o, $(wildcard $(KERN_DIR)/cpu/*.c))

INCLUDE := -I./$(MODULES)/lib -I./$(MODULES)/printk
INCLUDE += -I./$(KERN_DIR)/ -I./$(KERN_DIR)/cpu/


# Virtual machine for debuging OS
BOCHS := bochs

.PHONY: all
all: prepare kernel.bin boot
	$(call start_bochs)

.PHONY:
start:
	$(call start_bochs)

# create directory for output
.PHONY: prepare
prepare: clean
	$(shell [ -e $(OUT) ] || mkdir -p $(OUT))
	$(shell [ -e $(OUT)/$(MODULES) ] || mkdir -p $(OUT)/$(MODULES))
	$(shell [ -e $(OUT)/$(BOOT_DIR) ] || mkdir -p $(OUT)/$(BOOT_DIR))
	$(shell [ -e $(OUT)/$(KERN_DIR) ] || mkdir -p $(OUT)/$(KERN_DIR))

define start_bochs
	@read -p "Start BOCHS for this image:(Enter to default is yes, 'n' to No): " start_gui; \
	if [  "$$start_gui"x = "n"x ]; then \
		echo "Existing with no bochs!" ; exit 0; \
	fi ; \
	$(BOCHS) -q
endef

.PHONY: kernel.bin
kernel.bin: $(OBJS) $(MOD_LIB)
	$(LD) $(LDFLAGS) -static -o $(OUT)/$(KERN_DIR)/kernel.o $(OBJS) -L$(OUT)/$(MODULES)/ $(MOD_LIB) -T $(KERN_DIR)/kernel.lds
	@$(OBJCOPY) $(OBJCPYFLAGS) $(OUT)/$(KERN_DIR)/kernel.o $(OUT)/$(KERN_DIR)/$@

$(OUT)/%.o: %.c
	[ -e $(dir $@) ] || mkdir -p $(dir $@);
	$(CC) -c $(CFLAGS) $(INCLUDE) $< -o $@

$(OUT)/%.o: %.S
	#[ -e $(dir $@) ] || mkdir -p $(dir $@)
	@$(CC) -E $< > $(OUT)/$(patsubst %.S,%.s,$<)
	@$(AS) $(ASFLAGS) -o $@ $(OUT)/$(patsubst %.S,%.s,$<)

$(MOD_LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

.PHONY: boot
boot: boot.bin loader.bin
	@# Burn boot.bin into boot.img
	@echo -e "Generating boot.img..."; \
	cp $(PWD)/boot.img $(OUT)/$(BOOT_DIR)/boot.img; \
	$(DD) if=$(OUT)/$(BOOT_DIR)/boot.bin of=$(OUT)/$(BOOT_DIR)/boot.img bs=512 count=1 conv=notrunc || exit $?

	@# Copy loader.bin into the boot.image
	@sudo mount $(OUT)/$(BOOT_DIR)/boot.img /media/ -t vfat -o loop || exit $?; \
	sudo cp $(OUT)/$(BOOT_DIR)/loader.bin /media/ || exit $?; \
	sudo cp $(OUT)/$(KERN_DIR)/kernel.bin /media/ || exit $?; \
	sync; sudo umount /media || exit $?

.PHONY: boot.bin
boot.bin:
	@echo -e "crate boot.bin..."; \
	$(NASM) $(BOOT_SRC) -o $(OUT)/$(BOOT_DIR)/$@ || exit $?

.PHONY: loader.bin
loader.bin:
	@echo -e "crate loader.bin..."; \
	$(NASM) $(LOADER_SRC) -o $(OUT)/$(BOOT_DIR)/$@ || exit $?

.PHONY: tools
tools:
	$(MAKE) -C $(PWD)/scripts/

.PHONY: clean
clean:
	@-rm -rf out/
