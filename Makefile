all: 
	make -C boot
	make -C kernel

install: boot/loader.bin kernel/kernel.bin
	mkdir -p fd144
	mount build/fd144.img fd144/ -t vfat -o loop 
	cp -f $^ fd144/ 
	sync 
	umount fd144/
	rmdir fd144

clean:
	rm -f boot/*.bin
	rm -f kernel/*.bin