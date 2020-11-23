nasm boot/loader.asm -i boot/ -o build/loader.bin

if [ ! -d fd144 ]
then
    mkdir fd144
fi

mount build/fd144.img fd144/ -t vfat -o loop 
rm -rf fd144/*
ls -al fd144/
cp build/loader.bin fd144/ 
sync 
ls -al fd144/
umount fd144/