/*
 * Copyright 2021 Victor Ardouin
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/boards.h"
#include "hw/arm/boot.h"
#include "sysemu/sysemu.h"
#include "exec/address-spaces.h"

#include "hw/arm/nrf52_soc.h"
#include "hw/qdev-properties.h"
#include "qom/object.h"

struct CapMachineState {
    MachineState parent;

    NRF52State nrf52;
};

#define TYPE_CAP_MACHINE MACHINE_TYPE_NAME("cap_machine")

OBJECT_DECLARE_SIMPLE_TYPE(CapMachineState, CAP_MACHINE)

static void cap_init(MachineState *machine)
{
    CapMachineState *s = CAP_MACHINE(machine);
    MemoryRegion *system_memory = get_system_memory();
    //MemoryRegion *mr;

    object_initialize_child(OBJECT(machine), "nrf52", &s->nrf52,
                            TYPE_NRF52_SOC);
    qdev_prop_set_chr(DEVICE(&s->nrf52), "serial0", serial_hd(0));
    object_property_set_link(OBJECT(&s->nrf52), "memory",
                             OBJECT(system_memory), &error_fatal);
    sysbus_realize(SYS_BUS_DEVICE(&s->nrf52), &error_fatal);


    // armv7m_load_kernel(ARM_CPU(first_cpu), machine->kernel_filename,
    //                    s->nrf52.flash_size);
}

static void cap_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "nrf52 (Cortex-M4)";
    mc->init = cap_init;
    mc->max_cpus = 1;
}

static const TypeInfo cap_info = {
    .name = TYPE_CAP_MACHINE,
    .parent = TYPE_MACHINE,
    .instance_size = sizeof(CapMachineState),
    .class_init = cap_machine_class_init,
};

static void cap_machine_init(void)
{
    type_register_static(&cap_info);
}

type_init(cap_machine_init);
