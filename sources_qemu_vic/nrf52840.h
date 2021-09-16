/* Adresses utiles
- Les GPIOs
- L’interface I2C (ou TWI)
- L’interface SWD (pour le debug, la lecture et l’écriture sur la mémoire flash du SoC nRF52840)
- USB
- UART
- Timer
- RTC (Real Time Counter)
- Radio - 2.4 GHz
*/

#ifndef NRF52_H
#define NRF52_H

#define NRF52_FLASH_BASE      0x00000000
#define NRF52_FICR_BASE       0x10000000
#define NRF52_FICR_SIZE       0x00000100
#define NRF52_UICR_BASE       0x10001000
#define NRF52_SRAM_BASE       0x20000000

#define NRF52_IOMEM_BASE      0x40000000
#define NRF52_IOMEM_SIZE      0x20000000

#define NRF52_PERIPHERAL_SIZE 0x00001000
#define NRF52_RADIO_BASE      0x40001000
#define NRF52_UART_BASE       0x40002000
#define NRF52_TWI_BASE        0x40003000
#define NRF52_RTC_BASE        0x40024000
#define NRF52_TIMER_BASE      0x40008000
#define NRF52_RNG_BASE        0x4000D000
#define NRF52_NVMC_BASE       0x4001E000
#define NRF52_GPIO_BASE       0x50000000

#define NRF52_PRIVATE_BASE    0xF0000000
#define NRF52_PRIVATE_SIZE    0x10000000

#define NRF52_PAGE_SIZE       1024

/* Trigger */
#define NRF52_TRIGGER_TASK 0x01

/* Events */
#define NRF52_EVENT_CLEAR  0x00

#endif
