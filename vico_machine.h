#pragma once

#define VICO_BOARD_ID 0x8e0
#define VICO_FLASH_SIZE (64 * 1024 * 1024)
#define VICO_FLASH_SECT_SIZE (256 * 1024)

/* Number of virtio transports to create (0..8; limited by
 * number of available IRQ lines).
 */
#define NUM_VIRTIO_TRANSPORTS 4

/* Address maps for peripherals:
 * the Versatile Express motherboard has two possible maps,
 * the "legacy" one (used for A9) and the "Cortex-A Series"
 * map (used for newer cores).
 * Individual daughterboards can also have different maps for
 * their peripherals.
 */

enum {
    SYSREGS,
    SP810,
    SERIALPCI,
    PL041,
    MMCI,
    KMI0,
    KMI1,
    UART0,
    UART1,
    UART2,
    UART3,
    WDT,
    TIMER01,
    TIMER23,
    SERIALDVI,
    RTC,
    COMPACTFLASH,
    CLCD,
    NORFLASH0,
    NORFLASH1,
    NORFLASHALIAS,
    SRAM,
    VIDEORAM,
    ETHERNET,
    USB,
    DAPROM,
    VIRTIO,
};

static hwaddr motherboard_legacy_map[] = {
    [NORFLASHALIAS] = 0,
    /* CS7: 0x10000000 .. 0x10020000 */
    [SYSREGS] = 0x10000000,
    [SP810] = 0x10001000,
    [SERIALPCI] = 0x10002000,
    [PL041] = 0x10004000,
    [MMCI] = 0x10005000,
    [KMI0] = 0x10006000,
    [KMI1] = 0x10007000,
    [UART0] = 0x10009000,
    [UART1] = 0x1000a000,
    [UART2] = 0x1000b000,
    [UART3] = 0x1000c000,
    [WDT] = 0x1000f000,
    [TIMER01] = 0x10011000,
    [TIMER23] = 0x10012000,
    [VIRTIO] = 0x10013000,
    [SERIALDVI] = 0x10016000,
    [RTC] = 0x10017000,
    [COMPACTFLASH] = 0x1001a000,
    [CLCD] = 0x1001f000,
    /* CS0: 0x40000000 .. 0x44000000 */
    [NORFLASH0] = 0x40000000,
    /* CS1: 0x44000000 .. 0x48000000 */
    [NORFLASH1] = 0x44000000,
    /* CS2: 0x48000000 .. 0x4a000000 */
    [SRAM] = 0x48000000,
    /* CS3: 0x4c000000 .. 0x50000000 */
    [VIDEORAM] = 0x4c000000,
    [ETHERNET] = 0x4e000000,
    [USB] = 0x4f000000,
};

// static hwaddr motherboard_aseries_map[] = {
//     [NORFLASHALIAS] = 0,
//     /* CS0: 0x08000000 .. 0x0c000000 */
//     [NORFLASH0] = 0x08000000,
//     /* CS4: 0x0c000000 .. 0x10000000 */
//     [NORFLASH1] = 0x0c000000,
//     /* CS5: 0x10000000 .. 0x14000000 */
//     /* CS1: 0x14000000 .. 0x18000000 */
//     [SRAM] = 0x14000000,
//     /* CS2: 0x18000000 .. 0x1c000000 */
//     [VIDEORAM] = 0x18000000,
//     [ETHERNET] = 0x1a000000,
//     [USB] = 0x1b000000,
//     /* CS3: 0x1c000000 .. 0x20000000 */
//     [DAPROM] = 0x1c000000,
//     [SYSREGS] = 0x1c010000,
//     [SP810] = 0x1c020000,
//     [SERIALPCI] = 0x1c030000,
//     [PL041] = 0x1c040000,
//     [MMCI] = 0x1c050000,
//     [KMI0] = 0x1c060000,
//     [KMI1] = 0x1c070000,
//     [UART0] = 0x1c090000,
//     [UART1] = 0x1c0a0000,
//     [UART2] = 0x1c0b0000,
//     [UART3] = 0x1c0c0000,
//     [WDT] = 0x1c0f0000,
//     [TIMER01] = 0x1c110000,
//     [TIMER23] = 0x1c120000,
//     [VIRTIO] = 0x1c130000,
//     [SERIALDVI] = 0x1c160000,
//     [RTC] = 0x1c170000,
//     [COMPACTFLASH] = 0x1c1a0000,
//     [CLCD] = 0x1c1f0000,
// };
