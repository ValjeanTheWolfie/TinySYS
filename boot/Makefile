all: ../build/fd144.img loader.bin

../build/fd144.img: mbr.bin
	rm $@
	bximage -q -mode=create -fd=1.44M $@
	dd if=mbr.bin of=../build/fd144.img bs=512 count=1 conv=notrunc 

%.bin: %.asm *.inc
	nasm $< -o $@

clean:
	rm -f *.bin