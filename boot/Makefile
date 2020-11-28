all: ../build/fd144.img 

../build/fd144.img: mbr.bin
	rm $@
	bximage -q -mode=create -fd=1.44M $@
	dd if=mbr.bin of=../build/fd144.img bs=512 count=1 conv=notrunc 

%.bin: %.asm *.inc
	nasm $< -o $@

#loader.bin: loader.asm
#	nasm $^ -o $@

install: loader.bin
	mkdir -p fd144
	mount ../build/fd144.img fd144/ -t vfat -o loop 
	cp -f loader.bin fd144/ 
	sync 
	umount fd144/
	rmdir fd144

clean:
	rm -f *.bin fd144