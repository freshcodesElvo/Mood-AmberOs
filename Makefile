

ASM=nasm
CC=gcc
LD=ld

CFLAGS=-m32 -ffreestanding -fno-pie -fno-stack-protector -Iinclude
LDFLAGS=-m elf_i386

all: os.iso

build/boot.o: boot/boot.asm
	$(ASM) -f elf32 boot/boot.asm -o build/boot.o

build/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o build/kernel.o

build/screen.o: kernel/screen.c
	$(CC) $(CFLAGS) -c kernel/screen.c -o build/screen.o

build/interrupts.o: kernel/interrupts.c
	$(CC) $(CFLAGS) -c kernel/interrupts.c -o build/interrupts.o

build/kernel.bin: build/boot.o build/kernel.o build/screen.o build/interrupts.o linker.ld
	$(LD) $(LDFLAGS) -T linker.ld build/boot.o build/kernel.o build/screen.o build/interrupts.o -o build/kernel.bin


os.iso: build/kernel.bin
	mkdir -p build/isodir/boot/grub
	cp build/kernel.bin build/isodir/boot/kernel.bin
	cp boot/grub/grub.cfg build/isodir/boot/grub/grub.cfg
	grub-mkrescue -o build/MudambaOs.iso build/isodir

run: os.iso
	qemu-system-x86_64 -cdrom build/MudambaOs.iso

clean:
	rm -rf build/*



