# #!/bin/bash

/home/vardouin/Project/qemu/build/qemu-system-arm -machine vico-a9 -smp 2 -m 256 \
    -kernel /home/vardouin/Project/buildroot-2021.02.3/output/images/zImage \
    -dtb /home/vardouin/Project/buildroot-2021.02.3/output/images/vexpress-v2p-ca9.dtb \
    -drive file=/home/vardouin/Project/buildroot-2021.02.3/output/images/rootfs.ext2,if=sd,format=raw \
    -append "console=ttyAMA0,115200 rootwait root=/dev/mmcblk0"\
    -serial udp:localhost:4321
         # Si tu veux lancer la machine sur un terminal fait ça : -serial stdio
         # qemu_arm_vexpress_defconfig

    #-s active GDB
    #-S n'active pas le processeur virtuel
    #-net nic,model=lan9118
    #-net user
    #-cpu cortex-a9

##Machine avec l'echo
    # build/qemu-system-arm -machine vico-a9 -smp 2 -m 256 \
    #     -device loader,file=/home/vardouin/Project/sourcesvic/test.X.debug.elf \
    #     -serial udp:localhost:4321

##Essaie de lancement de la machine nrf52
# /home/vardouin/Project/qemu/build/qemu-system-arm -M cap_machine \
#     -device loader,file=uart_pca10040.hex \
#     -serial mon:stdio \
#     -s
