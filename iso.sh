export CROSS_COMP_BASE=$HOME/opt/cross

export PATH=$PATH:$CROSS_COMP_BASE/bin

mkdir -p sysroot/boot/grub

cat > sysroot/boot/grub/grub.cfg << EOF
default 1
timeout 5
menuentry "KaOSVengeance" {
    multiboot /boot/karacaos.kernel
}
EOF
grub-mkrescue -o karacaos.iso sysroot --directory=/lib/grub/i386-pc
