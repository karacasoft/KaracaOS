# KaracaOS
The successor of my last operating system project. (Sadly,) no creative name.

## Requirements:
* A i386-elf or i686-elf cross compiler
* Grub bootloader
* xorriso (to create .iso images)
* qemu (to test the system)

## To build...
Change the build.sh script according to your system setup. I have my cross compiler binaries installed in a different directory. You might want to change the `CROSS_COMP_BASE` variable to point to where your cross compiler is. Or maybe remove it if it is available through system environment by default. Then simply run the script.

## To run...
Run the qemu.sh script.
