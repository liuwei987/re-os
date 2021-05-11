PWD=$(shell echo -e `pwd`)
OUT=$(PWD)/out

# Compile tools and flags
CC=gcc
DD=dd
AS=as
LD=ld
NASM=nasm
OBJDUMP=objdump
OBJCOPY=objcopy
CFLAGS= -Werror -Wstack-protector -fstack-protector

INCLUDE = -I$(PWD)/include

# Compile the source file
BOOT_SRC=$(PWD)/arch/x86/boot/boot.asm
LOADER_SRC=$(PWD)/arch/x86/boot/loader.asm

# Virtual machine for debuging OS
BOCHS=bochs

.PHONY: all
all: prepare boot
	$(call start_bochs)

# create directory for output
.PHONY: prepare
prepare: clean
	$(shell [ -d $(OUT) ] || mkdir $(OUT))

define start_bochs
	@read -p "Start BOCHS:(Enter to default to [y]): " start_gui; \
	if [ "$$start_gui"x = ""x ] || [  "$$start_gui"x = "n"x ]; then \
		echo "Existing with no bochs!"; exit; \
	fi
	$(BOCHS) -q
endef

.PHONY: boot
boot: boot.bin loader.bin
	@# Burn boot.bin into boot.img
	@echo -e "Generating boot.img..."; \
	cp $(PWD)/boot.img $(OUT)/boot.img; \
	$(DD) if=$(OUT)/boot.bin of=$(OUT)/boot.img bs=512 count=1 conv=notrunc || exit $?

	@# Copy loader.bin into the boot.image
	@#sudo mount $(OUT)/boot.image /media/ -t vfat -o loop || exit $?; \
	#cp $(OUT)/loader.bin /media/ || exit $?; \
	#sync; sudo umount /media || exit $?

.PHONY: boot.bin
boot.bin:
	@echo -e "crate boot.bin..."; \
	$(NASM) $(BOOT_SRC) -o $(OUT)/$@ || exit $?

.PHONY: loader.bin
loader.bin:
	@echo -e "crate loader.bin..."; \
	#$(NASM) $(LOADER_SRC) -o $(OUT)/$@ || exit $?

.PHONY: clean
clean:
	@-rm -rf out/
