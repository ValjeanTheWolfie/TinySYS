nasm boot/mbr.asm -i boot/ -o build/mbr.bin
dd if=build/mbr.bin of=build/fd144.img bs=512 count=1 conv=notrunc
