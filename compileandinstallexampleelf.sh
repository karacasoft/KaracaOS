~/opt/cross/bin/i686-elf-karacaos-gcc example/example.c -o example/example.elf -fPIE
sudo mount -o loop,offset=0 karacaos.img ./mnt
sudo cp example/example.elf mnt/EXAMPLE.ELF
sudo umount ./mnt
