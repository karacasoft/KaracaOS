if [ $1 = "format" ]; then
    qemu-img create -f qcow2 karacaos.img 20M
fi

qemu-system-i386 -s -S -cdrom karacaos.iso -drive format=qcow2,file=karacaos.img
