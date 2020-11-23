rm -f build/*.bin
echo "times 512*50 db 0" > empty.asm
nasm empty.asm -o empty.bin
dd if=empty.bin of=build/fd144.img bs=$[512*50] count=1 conv=notrunc 
rm empty*

nasm boot/mbr.asm -i boot/ -o build/mbr.bin 
dd if=build/mbr.bin of=build/fd144.img bs=512 count=1 conv=notrunc 
