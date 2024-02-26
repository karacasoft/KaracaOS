if [ $1 = "format" ]; then
    qemu-img create -f raw karacaos.img 20M
fi

qemu-system-i386 -cdrom karacaos.iso -drive format=raw,file=karacaos.img -boot menu=on -s
