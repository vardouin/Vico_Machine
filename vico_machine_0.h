#pragma once
// Boot
#define MACH_TYPE_CORTEXA9      0
#define SMPBOOT_ADDR            0x300 /* this should leave enough space for ATAGS */
#define MVBAR_ADDR              0x400 /* secure vectors */
#define BOARDSETUP_ADDR         (MVBAR_ADDR + 0x20) /* board setup code */

#define FIRMWARE_ADDR_2         0x8000 /* Pi 2 loads kernel.img here by default */
#define FIRMWARE_ADDR_3         0x80000 /* Pi 3 loads kernel.img here by default */
#define SPINTABLE_ADDR          0xd8 /* Pi 3 bootloader spintable */

// Memories defines
#define VICO_FLASH_BASE        0x00400000 // mémoire de masse
#define VICO_SRAM_BASE         0x20400000 // mémoire cash
#define VICO_SRAM_BACKUP_BASE  0x40074000 // BRAM

#define VICO_ITCM_BASE         0x00000000 // code region (Instruction TCM)
#define VICO_DTCM_BASE         0x20000000 // data region (Data TCM)

#define VICO_PERIPHERAL_SIZE   0x4000
#define VICO_UART0_BASE        0x400E0800 // ID : 7
#define VICO_UART1_BASE        0x400E0A00 // ID : 8
#define VICO_TWIHS0_BASE       0x40018000 // ID : 19 (I2C => Two Wire Interface)
#define VICO_TIMER0_CH0_BASE   0x4000C000 // ID : 23
#define VICO_TRNG_BASE         0x40074000 // ID : 57
#define VICO_PIOA_BASE         0x400E0E00 // ID : 10
#define VICO_PIOB_BASE         0x400E1000 // ID : 11
#define VICO_PIOC_BASE         0x400E1200 // ID : 12
#define VICO_PIOD_BASE         0x400E1400 // ID : 16
#define VICO_PIOE_BASE         0x400E1600 // ID : 17

//Sizes defines
#define VICO_RAM_SIZE         4000000000  //4Go
#define VICO_FLASH_SIZE       20000000000 //20Go
