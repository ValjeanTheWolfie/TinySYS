all: kernel.bin

kernel.bin: fakekernel.asm
	nasm $^ -o $@

clean:
	rm *.bin