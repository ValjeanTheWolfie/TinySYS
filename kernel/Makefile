CFLAGS = -mcmodel=large -fno-builtin -m64

all: kernel.bin

kernel.bin: kernel.elf
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary kernel.elf kernel.bin

kernel.elf: head.o main.o printk.o tables.o gates.o entry.o
	ld -b elf64-x86-64 -z muldefs -o $@ -T kernel.lds $^

%.o: %.c *.h
	gcc $(CFLAGS) $< -c -o $@

%.o: %.S *.h
	gcc $(CFLAGS) $< -c -o $@

clean:
	rm -f *.bin *.o *.elf