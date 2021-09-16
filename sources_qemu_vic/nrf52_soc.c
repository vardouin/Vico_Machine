/*
 * Copyright 2021 Victor Ardouin
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/sysbus.h"
#include "hw/misc/unimp.h"
#include "qemu/log.h"

#include "hw/arm/nrf52840.h"
#include "hw/arm/nrf52_soc.h"

/*
 * The size and base is for the nrf52 part. If other parts
 * are supported in the future, add a sub-class of nrf52SoC for
 * the specific variants
 */
#define NRF52822_FLASH_PAGES    256
#define NRF52822_SRAM_PAGES     16
#define NRF52822_FLASH_SIZE     (NRF52822_FLASH_PAGES * NRF52_PAGE_SIZE)
#define NRF52822_SRAM_SIZE      (NRF52822_SRAM_PAGES * NRF52_PAGE_SIZE)

#define BASE_TO_IRQ(base) ((base >> 12) & 0x1F)

/* HCLK (the main CPU clock) on this SoC is always 16MHz */
#define HCLK_FRQ 16000000

static uint64_t clock_read(void *opaque, hwaddr addr, unsigned int size)
{
    qemu_log_mask(LOG_UNIMP, "%s: 0x%" HWADDR_PRIx " [%u]\n",
                  __func__, addr, size);
    return 1;
}

static void clock_write(void *opaque, hwaddr addr, uint64_t data,
                        unsigned int size)
{
    qemu_log_mask(LOG_UNIMP, "%s: 0x%" HWADDR_PRIx " <- 0x%" PRIx64 " [%u]\n",
                  __func__, addr, data, size);
}

static const MemoryRegionOps clock_ops = {
    .read = clock_read,
    .write = clock_write
};


static void nrf52_soc_realize(DeviceState *dev_soc, Error **errp)
{
    NRF52State *s = NRF52_SOC(dev_soc);
    MemoryRegion *mr;
    Error *err = NULL;
    uint8_t i = 0;
    hwaddr base_addr = 0;

    if (!s->board_memory) {
        error_setg(errp, "memory property was not set");
        return;
    }

    system_clock_scale = NANOSECONDS_PER_SECOND / HCLK_FRQ;

    object_property_set_link(OBJECT(&s->cpu), "memory", OBJECT(&s->container),
                             &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->cpu), errp)) {
        return;
    }

    memory_region_add_subregion_overlap(&s->container, 0, s->board_memory, -1);

    memory_region_init_ram(&s->sram, OBJECT(s), "nrf52.sram", s->sram_size,
                           &err);
    if (err) {
        error_propagate(errp, err);
        return;
    }
    memory_region_add_subregion(&s->container, NRF52_SRAM_BASE, &s->sram);

    // /* UART */
    // if (!sysbus_realize(SYS_BUS_DEVICE(&s->uart), errp)) {
    //     return;
    // }
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->uart), 0);
    // memory_region_add_subregion_overlap(&s->container, -, mr, 0);
    // sysbus_connect_irq(SYS_BUS_DEVICE(&s->uart), 0,
    //                    qdev_get_gpio_in(DEVICE(&s->cpu),
    //                    BASE_TO_IRQ(NRF52_UART_BASE)));
    //
    // /* RNG */
    // if (!sysbus_realize(SYS_BUS_DEVICE(&s->rng), errp)) {
    //     return;
    // }
    //
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->rng), 0);
    // memory_region_add_subregion_overlap(&s->container, NRF52_RNG_BASE, mr, 0);
    // sysbus_connect_irq(SYS_BUS_DEVICE(&s->rng), 0,
    //                    qdev_get_gpio_in(DEVICE(&s->cpu),
    //                    BASE_TO_IRQ(NRF52_RNG_BASE)));
    //
    // /* UICR, FICR, NVMC, FLASH */
    // if (!object_property_set_uint(OBJECT(&s->nvm), "flash-size",
    //                               s->flash_size, errp)) {
    //     return;
    // }
    //
    // if (!sysbus_realize(SYS_BUS_DEVICE(&s->nvm), errp)) {
    //     return;
    // }
    //
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->nvm), 0);
    // memory_region_add_subregion_overlap(&s->container, NRF52_NVMC_BASE, mr, 0);
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->nvm), 1);
    // memory_region_add_subregion_overlap(&s->container, NRF52_FICR_BASE, mr, 0);
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->nvm), 2);
    // memory_region_add_subregion_overlap(&s->container, NRF52_UICR_BASE, mr, 0);
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->nvm), 3);
    // memory_region_add_subregion_overlap(&s->container, NRF52_FLASH_BASE, mr, 0);
    //
    // /* GPIO */
    // if (!sysbus_realize(SYS_BUS_DEVICE(&s->gpio), errp)) {
    //     return;
    // }
    //
    // mr = sysbus_mmio_get_region(SYS_BUS_DEVICE(&s->gpio), 0);
    // memory_region_add_subregion_overlap(&s->container, NRF52_GPIO_BASE, mr, 0);
    //
    // /* Pass all GPIOs to the SOC layer so they are available to the board */
    // qdev_pass_gpios(DEVICE(&s->gpio), dev_soc, NULL);
    //
    // /* TIMER */
    // for (i = 0; i < NRF52_NUM_TIMERS; i++) {
    //     if (!object_property_set_uint(OBJECT(&s->timer[i]), "id", i, errp)) {
    //         return;
    //     }
    //     if (!sysbus_realize(SYS_BUS_DEVICE(&s->timer[i]), errp)) {
    //         return;
    //     }
    //
    //     base_addr = NRF52_TIMER_BASE + i * NRF52_PERIPHERAL_SIZE;
    //
    //     sysbus_mmio_map(SYS_BUS_DEVICE(&s->timer[i]), 0, base_addr);
    //     sysbus_connect_irq(SYS_BUS_DEVICE(&s->timer[i]), 0,
    //                        qdev_get_gpio_in(DEVICE(&s->cpu),
    //                                         BASE_TO_IRQ(base_addr)));
    // }
    //
    // /* STUB Peripherals */
    // memory_region_init_io(&s->clock, OBJECT(dev_soc), &clock_ops, NULL,
    //                       "nrf52_soc.clock", NRF52_PERIPHERAL_SIZE);
    // memory_region_add_subregion_overlap(&s->container,
    //                                     NRF52_IOMEM_BASE, &s->clock, -1);
    //
    // create_unimplemented_device("nrf52_soc.io", NRF52_IOMEM_BASE,
    //                             NRF52_IOMEM_SIZE);
    // create_unimplemented_device("nrf52_soc.private",
    //                             NRF52_PRIVATE_BASE, NRF52_PRIVATE_SIZE);
}

static void nrf52_soc_init(Object *obj)
{
    uint8_t i = 0;

    NRF52State *s = NRF52_SOC(obj);

    memory_region_init(&s->container, obj, "nrf52-container", UINT64_MAX);

    object_initialize_child(OBJECT(s), "armv6m", &s->cpu, TYPE_ARMV7M);
    qdev_prop_set_string(DEVICE(&s->cpu), "cpu-type",
                         ARM_CPU_TYPE_NAME("cortex-m4"));
    qdev_prop_set_uint32(DEVICE(&s->cpu), "num-irq", 32);

    object_initialize_child(obj, "uart", &s->uart, TYPE_NRF52_UART);
    object_property_add_alias(obj, "serial0", OBJECT(&s->uart), "chardev");

    // object_initialize_child(obj, "rng", &s->rng, TYPE_NRF52_RNG);
    //
    // object_initialize_child(obj, "nvm", &s->nvm, TYPE_NRF52_NVM);
    //
    // object_initialize_child(obj, "gpio", &s->gpio, TYPE_NRF52_GPIO);
    //
    // for (i = 0; i < NRF52_NUM_TIMERS; i++) {
    //     object_initialize_child(obj, "timer[*]", &s->timer[i],
    //                             TYPE_NRF52_TIMER);
    //
    // }
}

static Property nrf52_soc_properties[] = {
    DEFINE_PROP_LINK("memory", NRF52State, board_memory, TYPE_MEMORY_REGION,
                     MemoryRegion *),
    DEFINE_PROP_UINT32("sram-size", NRF52State, sram_size, NRF52822_SRAM_SIZE),
    DEFINE_PROP_UINT32("flash-size", NRF52State, flash_size,
                       NRF52822_FLASH_SIZE),
    DEFINE_PROP_END_OF_LIST(),
};

static void nrf52_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = nrf52_soc_realize;
    device_class_set_props(dc, nrf52_soc_properties);
}

static const TypeInfo nrf52_soc_info = {
    .name          = TYPE_NRF52_SOC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NRF52State),
    .instance_init = nrf52_soc_init,
    .class_init    = nrf52_soc_class_init,
};

static void nrf52_soc_types(void)
{
    type_register_static(&nrf52_soc_info);
}
type_init(nrf52_soc_types)
