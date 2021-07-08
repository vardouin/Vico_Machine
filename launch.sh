#!/bin/bash

#udp serial
#build/qemu-system-arm -machine same70 -chardev udp,id=serial01,host=127.0.0.1,port=6969,localaddr=127.0.0.1,localport=6970,ipv4=on -serial chardev:serial01 -device loader,file=/home/legerv/HarmonyProjects/MyProject/firmware/test.X/dist/default/debug/test.X.debug.elf -s -S
#build/qemu-system-arm -machine same70 -chardev udp,id=serial01,host=127.0.0.1,port=6969,localaddr=127.0.0.1,localport=6970,ipv4=on -mon chardev=serial01 -serial chardev:serial01 -device loader,file=/home/legerv/HarmonyProjects/MyProject/firmware/test.X/dist/default/debug/test.X.debug.elf -s -S

#stdio to serial
#build/qemu-system-arm -machine same70 -chardev udp,id=serial01,host=127.0.0.1,port=6969,localaddr=127.0.0.1,localport=6970,ipv4=on -serial mon:stdio -device loader,file=/home/legerv/HarmonyProjects/MyProject/firmware/test.X/dist/default/debug/test.X.debug.elf -s -S

# build/qemu-system-arm -cpu cortex-a9 -smp 2 -m 4G \
#     -machine type=vexpress-a9 -serial mon:stdio \
#     -kernel ~/Project/linux-5.12.12/arch/arm/boot/zImage  \
#     -dtb ~/Project/linux-5.12.12/arch/arm/boot/dts/vexpress-v2p-ca9.dtb\
#     -append "console=ttyAMA0 root=/dev/mmcblk0 ro" \
#     -s
#    -S \
#-s active GDB
#-S n'active pas le processeur virtuel

# build/qemu-system-arm -cpu cortex-a9 -smp 2 -m 3G \
#     -machine type=vico-a9 -serial mon:stdio \
#     -kernel ~/Project/linux-5.12.12/arch/arm/boot/zImage  \
#     -dtb ~/Project/sourcesvic/vexpress-v2p-ca9.dtb\
#     -append "console=ttyAMA0 root=/dev/mmcblk0 ro" \
#     -s

# build/qemu-system-arm -cpu cortex-a9 -smp 2 -m 2G \
#     -machine type=vexpress-a9 -serial mon:stdio \
#     -drive if=sd,driver=file,filename=../buildroot-2021.02.3/output/images/rootfs.ext2\
#     -kernel ~/Project/linux-5.12.12/arch/arm/boot/zImage  \
#     -dtb ~/Project/sourcesvic/vexpress-v2p-ca9.dtb \
#     -append "console=ttyAMA0 root=/dev/mmcblk0 ro"

build/qemu-system-arm -M vexpress-a9 -smp 2 -m 256 \
    -kernel ../buildroot-2021.02.3/output/images/zImage \
    -dtb ../buildroot-2021.02.3/output/images/vexpress-v2p-ca9.dtb \
    -drive file=../buildroot-2021.02.3/output/images/rootfs.ext2,if=sd,format=raw \
    -append "console=ttyAMA0,115200 rootwait root=/dev/mmcblk0" \
    -serial stdio -net nic,model=lan9118 -net user # qemu_arm_vexpress_defconfig
