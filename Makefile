PWD :=$(shell echo -e `pwd`)
OUT :=$(PWD)/out

# Compile tools and flags
CC   := gcc
DD   := dd
AS   := as
LD   := ld
NASM := nasm
OBJDUMP := objdump
OBJCOPY := objcopy
CFLAGS  := -Werror -fstack-protector -mcmodel=large -fno-builtin -m64 -Wl,-Map=output.map
ASFLAGS := --64
LDFLAGS := -b elf64-x86-64
OBJCPYFLAGS := -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary
MAKE    := make

export OUT CC DD AS LD CFLAGS LDFLAGS MAKE

INCLUDE := -I$(PWD)/include

# Compile the source file
BOOT_DIR := $(PWD)/arch/x86/boot
KERN_DIR := $(PWD)/arch/x86/kernel
BOOT_SRC := $(BOOT_DIR)/boot.asm
LOADER_SRC := $(BOOT_DIR)/loader.asm
KERN_HEAD_SRC := $(KERN_DIR)/head.S
KERN_MAIN_SRC := $(KERN_DIR)/main.c

# Virtual machine for debuging OS
BOCHS := bochs

.PHONY: all
all: prepare kernel.bin boot
	$(call start_bochs)

# create directory for output
.PHONY: prepare
prepare: clean
	$(shell [ -d $(OUT) ] || mkdir $(OUT))

define start_bochs
	@read -p "Start BOCHS for this image:(Enter to default is yes, 'n' to No): " start_gui; \
	if [  "$$start_gui"x = "n"x ]; then \
		echo "Existing with no bochs!" ; exit 0; \
	fi ; \
	$(BOCHS) -q
endef

.PHONY: kernel.bin head.o main.o
kernel.bin: head.o main.o
	@$(LD) $(LDFLAGS) -o $(OUT)/kernel.o $(OUT)/head.o $(OUT)/main.o -T $(BOOT_DIR)/kernel.lds
	@$(OBJCOPY) $(OBJCPYFLAGS) $(OUT)/kernel.o $(OUT)/$@

main.o: $(KERN_MAIN_SRC)
	@$(CC) $(CFLAGS) -c $< -o $(OUT)/$@

head.o: $(KERN_HEAD_SRC)
	@$(CC) -E $< > $(OUT)/head.s
	@$(AS) $(ASFLAGS) -o $(OUT)/$@ $(OUT)/head.s

.PHONY: boot
boot: boot.bin loader.bin
	@# Burn boot.bin into boot.img
	@echo -e "Generating boot.img..."; \
	cp $(PWD)/boot.img $(OUT)/boot.img; \
	$(DD) if=$(OUT)/boot.bin of=$(OUT)/boot.img bs=512 count=1 conv=notrunc || exit $?

	@# Copy loader.bin into the boot.image
	@sudo mount $(OUT)/boot.img /media/ -t vfat -o loop || exit $?; \
	sudo cp $(OUT)/loader.bin /media/ || exit $?; \
	sudo cp $(OUT)/kernel.bin /media/ || exit $?; \
	sync; sudo umount /media || exit $?

.PHONY: boot.bin
boot.bin:
	@echo -e "crate boot.bin..."; \
	$(NASM) $(BOOT_SRC) -o $(OUT)/$@ || exit $?

.PHONY: loader.bin
loader.bin:
	@echo -e "crate loader.bin..."; \
	$(NASM) $(LOADER_SRC) -o $(OUT)/$@ || exit $?

.PHONY: tools
tools:
	$(MAKE) -C $(PWD)/scripts/

.PHONY: clean
clean:
	@-rm -rf out/
