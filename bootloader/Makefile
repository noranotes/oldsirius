ASFLAGS=-f elf32 -Ox
CFLAGS16=-Wall -Os -m16 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-align-functions -I ./include
CFLAGS32=-Wall -Os -m32 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-align-functions -I ./include
CFLAGS64=-Wall -Os -m64 -ffreestanding -fno-stack-protector -nostdlib -nostdinc -fno-align-functions -I ./include
LDFLAGS =-m elf_i386 -T link.ld
AS=nasm
CC=gcc
LD=ld

OBJstage2= setup.o io16.o vesa.o gdt.o main.o idt.o flush.o isrs.o exceptions.o irqs.o io.o part.o string.o mboot.o 
OBJdrivers= console.o pic.o pit.o keyboard.o pci.o ata.o ata_pci.o ide.o ahci.o
OBJmemory= page.o heap.o
OBJfs= vfs.o iso9660.o #fat.o 

.PHONY: stage2.bin clean move.bin

stage2.bin: $(OBJstage2) $(OBJdrivers) $(OBJmemory) $(OBJfs) 
	$(LD) $(LDFLAGS) -o $@ $^

#STAGE2
%.o: stage2/%.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: stage2/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
#DRIVERS
%.o: drivers/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
%.o: drivers/ata/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<
#MEMORY
%.o: memory/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<

#FS
%.o: fs/%.c
	$(CC) $(CFLAGS32) -c -o $@ $<

move.bin:
	-mv *.bin ../bin/Boot/Sirius

clean: move.bin
	rm *.o
