all: kernel.bin

kernel.bin: fakekernel.asm
		nasm $^ -o $@

install:
	mkdir -p fd144
	mount ../build/fd144.img fd144/ -t vfat -o loop 
	cp -f kernel.bin fd144/ 
	sync 
	umount fd144/
	rmdir fd144

clean:
	rm *.bin fd144