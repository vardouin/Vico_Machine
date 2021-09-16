/*
 * Copyright 2021 Victor Ardouin
 */

#ifndef NRF52_SOC_H
#define NRF52_SOC_H

#include "hw/sysbus.h"
#include "hw/arm/armv7m.h"
#include "hw/char/nrf52_uart.h"
// #include "hw/misc/nrf52_rng.h"
// #include "hw/gpio/nrf52_gpio.h"
// #include "hw/nvram/nrf52_nvm.h"
// #include "hw/timer/nrf52_timer.h"
#include "qom/object.h"

#define TYPE_NRF52_SOC "nrf52-soc"
OBJECT_DECLARE_SIMPLE_TYPE(NRF52State, NRF52_SOC)

#define NRF52_NUM_TIMERS 3

struct NRF52State {
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    ARMv7MState cpu;

    NRF52UARTState uart;
    // NRF52RNGState rng;
    // NRF52NVMState nvm;
    // NRF52GPIOState gpio;
    // NRF52TimerState timer[NRF52_NUM_TIMERS];

    MemoryRegion iomem;
    MemoryRegion sram;
    MemoryRegion flash;
    MemoryRegion clock;
    // MemoryRegion twi;

    uint32_t sram_size;
    uint32_t flash_size;

    MemoryRegion *board_memory;

    MemoryRegion container;

};

#endif
