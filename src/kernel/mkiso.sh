#!/bin/bash

# Download the latest Limine binary release.
git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1

# Build limine-install.
make -C limine

# Create a directory which will be our ISO root.
mkdir -p iso_root

# Copy the relevant files over.
cp -v skyekernel.elf limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/

# Create the bootable ISO.
#cd limine
#xorriso -as mkisofs -b limine-cd.bin \
#        -no-emul-boot -boot-load-size 4 -boot-info-table \
#	-isohybrid-mbr limine-hdd.bin \
#        -eltorito-alt-boot -e limine-eltorito-efi.bin \
#        -no-emul-boot ../iso_root/ -isohybrid-gpt-basdat -o ../image.iso

xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 \
        -boot-info-table -part_like_isohybrid -eltorito-alt-boot \
        -e limine-eltorito-efi.bin -no-emul-boot ./iso_root/ \
        -isohybrid-gpt-basdat -o ./image.iso
./limine/limine-install ./image.iso
