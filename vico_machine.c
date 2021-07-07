#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu-common.h"
#include "qemu/datadir.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/arm/boot.h"
#include "hw/arm/primecell.h"

#include "net/net.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/block/flash.h"
#include "sysemu/device_tree.h"
#include "qemu/error-report.h"
#include <libfdt.h>

#include "hw/cpu/a9mpcore.h"

#include "qom/object.h"

#include "hw/arm/vico_machine.h"

/* Structure defining the peculiarities of a specific daughterboard */

typedef struct VICBoardInfo VICBoardInfo;

struct VicoMachineClass {
    MachineClass parent;
    VICBoardInfo *daughterboard;
};

struct VicoMachineState {
    MachineState parent;
    bool secure;
    bool virt;
};

#define TYPE_VICO_MACHINE   "vico"
#define TYPE_VICO_A9_MACHINE   MACHINE_TYPE_NAME("vico-a9")
//#define TYPE_VICO_A15_MACHINE   MACHINE_TYPE_NAME("vico-a15")

OBJECT_DECLARE_TYPE(VicoMachineState, VicoMachineClass, VICO_MACHINE)

typedef void DBoardInitFn(const VicoMachineState *machine,
                          ram_addr_t ram_size,
                          const char *cpu_type,
                          qemu_irq *pic);

struct VICBoardInfo {
    struct arm_boot_info bootinfo;
    const hwaddr *motherboard_map;
    hwaddr loader_start;
    const hwaddr gic_cpu_if_addr;
    uint32_t proc_id;
    uint32_t num_voltage_sensors;
    const uint32_t *voltages;
    uint32_t num_clocks;
    const uint32_t *clocks;
    DBoardInitFn *init;
};

static void init_cpus(MachineState *ms, const char *cpu_type,
                      const char *privdev, hwaddr periphbase,
                      qemu_irq *pic, bool secure, bool virt)
{
    DeviceState *dev;
    SysBusDevice *busdev;
    int n;
    unsigned int smp_cpus = ms->smp.cpus;

    /* Create the actual CPUs */
    for (n = 0; n < smp_cpus; n++) {
        Object *cpuobj = object_new(cpu_type);

        if (!secure) {
            object_property_set_bool(cpuobj, "has_el3", false, NULL);
        }
        if (!virt) {
            if (object_property_find(cpuobj, "has_el2")) {
                object_property_set_bool(cpuobj, "has_el2", false, NULL);
            }
        }

        if (object_property_find(cpuobj, "reset-cbar")) {
            object_property_set_int(cpuobj, "reset-cbar", periphbase,
                                    &error_abort);
        }
        qdev_realize(DEVICE(cpuobj), NULL, &error_fatal);
    }

    /* Create the private peripheral devices (including the GIC);
     * this must happen after the CPUs are created because a15mpcore_priv
     * wires itself up to the CPU's generic_timer gpio out lines.
     */
    dev = qdev_new(privdev);
    qdev_prop_set_uint32(dev, "num-cpu", smp_cpus);
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_realize_and_unref(busdev, &error_fatal);
    sysbus_mmio_map(busdev, 0, periphbase);

    /* Interrupts [42:0] are from the motherboard;
     * [47:43] are reserved; [63:48] are daughterboard
     * peripherals. Note that some documentation numbers
     * external interrupts starting from 32 (because there
     * are internal interrupts 0..31).
     */
    for (n = 0; n < 64; n++) {
        pic[n] = qdev_get_gpio_in(dev, n);
    }

    /* Connect the CPUs to the GIC */
    for (n = 0; n < smp_cpus; n++) {
        DeviceState *cpudev = DEVICE(qemu_get_cpu(n));

        sysbus_connect_irq(busdev, n, qdev_get_gpio_in(cpudev, ARM_CPU_IRQ));
        sysbus_connect_irq(busdev, n + smp_cpus,
                           qdev_get_gpio_in(cpudev, ARM_CPU_FIQ));
        sysbus_connect_irq(busdev, n + 2 * smp_cpus,
                           qdev_get_gpio_in(cpudev, ARM_CPU_VIRQ));
        sysbus_connect_irq(busdev, n + 3 * smp_cpus,
                           qdev_get_gpio_in(cpudev, ARM_CPU_VFIQ));
    }
}

static void a9_daughterboard_init(const VicoMachineState *vms,
                                  ram_addr_t ram_size,
                                  const char *cpu_type,
                                  qemu_irq *pic)
{
    MachineState *machine = MACHINE(vms);
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *lowram = g_new(MemoryRegion, 1);

    memory_region_init_alias(lowram, NULL, "Vico.lowmem", machine->ram,
                             0, ram_size);
    memory_region_add_subregion(sysmem, 0x0, lowram);
    memory_region_add_subregion(sysmem, 0x60000000, machine->ram);

    /* 0x1e000000 A9MPCore (SCU) private memory region */
    init_cpus(machine, cpu_type, TYPE_A9MPCORE_PRIV, 0x1e000000, pic,
              vms->secure, vms->virt);
}


/* Reset values for daughterboard oscillators (in Hz) */
static const uint32_t a9_clocks[] = {
    45000000, /* AMBA AXI ACLK: 45MHz */
    23750000, /* daughterboard CLCD clock: 23.75MHz */
    66670000, /* Test chip reference clock: 66.67MHz */
};

static VICBoardInfo a9_daughterboard = {
    .motherboard_map = motherboard_legacy_map,
    .loader_start = 0x60000000,
    .gic_cpu_if_addr = 0x1e000100,
    .proc_id = 0x0c000191,

    .num_clocks = ARRAY_SIZE(a9_clocks),
    .clocks = a9_clocks,
    .init = a9_daughterboard_init,
};

static int add_virtio_mmio_node(void *fdt, uint32_t acells, uint32_t scells,
                                hwaddr addr, hwaddr size, uint32_t intc,
                                int irq)
{
    /* Add a virtio_mmio node to the device tree blob:
     *   virtio_mmio@ADDRESS {
     *       compatible = "virtio,mmio";
     *       reg = <ADDRESS, SIZE>;
     *       interrupt-parent = <&intc>;
     *       interrupts = <0, irq, 1>;
     *   }
     * (Note that the format of the interrupts property is dependent on the
     * interrupt controller that interrupt-parent points to; these are for
     * the ARM GIC and indicate an SPI interrupt, rising-edge-triggered.)
     */
    int rc;
    char *nodename = g_strdup_printf("/virtio_mmio@%" PRIx64, addr);

    rc = qemu_fdt_add_subnode(fdt, nodename);
    rc |= qemu_fdt_setprop_string(fdt, nodename,
                                  "compatible", "virtio,mmio");
    rc |= qemu_fdt_setprop_sized_cells(fdt, nodename, "reg",
                                       acells, addr, scells, size);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupt-parent", intc);
    qemu_fdt_setprop_cells(fdt, nodename, "interrupts", 0, irq, 1);
    qemu_fdt_setprop(fdt, nodename, "dma-coherent", NULL, 0);
    g_free(nodename);
    if (rc) {
        return -1;
    }
    return 0;
}

static uint32_t find_int_controller(void *fdt)
{
    /* Find the FDT node corresponding to the interrupt controller
     * for virtio-mmio devices. We do this by scanning the fdt for
     * a node with the right compatibility, since we know there is
     * only one GIC on a Vico board.
     * We return the phandle of the node, or 0 if none was found.
     */
    const char *compat = "arm,cortex-a9-gic";
    int offset;

    offset = fdt_node_offset_by_compatible(fdt, -1, compat);
    if (offset >= 0) {
        return fdt_get_phandle(fdt, offset);
    }
    return 0;
}

static void vico_modify_dtb(const struct arm_boot_info *info, void *fdt)
{
    uint32_t acells, scells, intc;
    const VICBoardInfo *daughterboard = (const VICBoardInfo *)info;

    acells = qemu_fdt_getprop_cell(fdt, "/", "#address-cells",
                                   NULL, &error_fatal);
    scells = qemu_fdt_getprop_cell(fdt, "/", "#size-cells",
                                   NULL, &error_fatal);
    intc = find_int_controller(fdt);
    if (!intc) {
        /* Not fatal, we just won't provide virtio. This will
         * happen with older device tree blobs.
         */
        warn_report("couldn't find interrupt controller in "
                    "dtb; will not include virtio-mmio devices in the dtb");
    } else {
        int i;
        const hwaddr *map = daughterboard->motherboard_map;

        /* We iterate backwards here because adding nodes
         * to the dtb puts them in last-first.
         */
        for (i = NUM_VIRTIO_TRANSPORTS - 1; i >= 0; i--) {
            add_virtio_mmio_node(fdt, acells, scells,
                                 map[VIRTIO] + 0x200 * i,
                                 0x200, intc, 40 + i);
        }
    }
}


/* Open code a private version of pflash registration since we
 * need to set non-default device width for vico platform.
 */
static PFlashCFI01 *vi_pflash_cfi01_register(hwaddr base, const char *name,
                                             DriveInfo *di)
{
    DeviceState *dev = qdev_new(TYPE_PFLASH_CFI01);

    if (di) {
        qdev_prop_set_drive(dev, "drive", blk_by_legacy_dinfo(di));
    }

    qdev_prop_set_uint32(dev, "num-blocks",
                         VICO_FLASH_SIZE / VICO_FLASH_SECT_SIZE);
    qdev_prop_set_uint64(dev, "sector-length", VICO_FLASH_SECT_SIZE);
    qdev_prop_set_uint8(dev, "width", 4);
    qdev_prop_set_uint8(dev, "device-width", 2);
    qdev_prop_set_bit(dev, "big-endian", false);
    qdev_prop_set_uint16(dev, "id0", 0x89);
    qdev_prop_set_uint16(dev, "id1", 0x18);
    qdev_prop_set_uint16(dev, "id2", 0x00);
    qdev_prop_set_uint16(dev, "id3", 0x00);
    qdev_prop_set_string(dev, "name", name);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, base);
    return PFLASH_CFI01(dev);
}

static void vico_common_init(MachineState *machine)
{
    VicoMachineState *vms = VICO_MACHINE(machine);
    VicoMachineClass *vmc = VICO_MACHINE_GET_CLASS(machine);
    VICBoardInfo *daughterboard = vmc->daughterboard;
    DeviceState *sysctl;// *pl041, *dev,;
    qemu_irq pic[64];
    uint32_t sys_id;
    DriveInfo *dinfo;
    PFlashCFI01 *pflash0;

    ram_addr_t vram_size, sram_size;
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *vram = g_new(MemoryRegion, 1);
    MemoryRegion *sram = g_new(MemoryRegion, 1);
    MemoryRegion *flashalias = g_new(MemoryRegion, 1);
    MemoryRegion *flash0mem;
    const hwaddr *map = daughterboard->motherboard_map;
    int i;

    daughterboard->init(vms, machine->ram_size, machine->cpu_type, pic);

    /*
     * If a bios file was provided, attempt to map it into memory
     */
    if (machine->firmware) {
        char *fn;
        int image_size;

        if (drive_get(IF_PFLASH, 0, 0)) {
            error_report("The contents of the first flash device may be "
                         "specified with -bios or with -drive if=pflash... "
                         "but you cannot use both options at once");
            exit(1);
        }
        fn = qemu_find_file(QEMU_FILE_TYPE_BIOS, machine->firmware);
        if (!fn) {
            error_report("Could not find ROM image '%s'", machine->firmware);
            exit(1);
        }
        image_size = load_image_targphys(fn, map[NORFLASH0],
                                         VICO_FLASH_SIZE);
        g_free(fn);
        if (image_size < 0) {
            error_report("Could not load ROM image '%s'", machine->firmware);
            exit(1);
        }
    }

    /* Motherboard peripherals: the wiring is the same but the
     * addresses vary between the legacy and A-Series memory maps.
     */

    sys_id = 0x1190f500;

    sysctl = qdev_new("realview_sysctl");
    qdev_prop_set_uint32(sysctl, "sys_id", sys_id);
    qdev_prop_set_uint32(sysctl, "proc_id", daughterboard->proc_id);
    qdev_prop_set_uint32(sysctl, "len-db-voltage",
                         daughterboard->num_voltage_sensors);
    for (i = 0; i < daughterboard->num_voltage_sensors; i++) {
        char *propname = g_strdup_printf("db-voltage[%d]", i);
        qdev_prop_set_uint32(sysctl, propname, daughterboard->voltages[i]);
        g_free(propname);
    }
    qdev_prop_set_uint32(sysctl, "len-db-clock",
                         daughterboard->num_clocks);
    for (i = 0; i < daughterboard->num_clocks; i++) {
        char *propname = g_strdup_printf("db-clock[%d]", i);
        qdev_prop_set_uint32(sysctl, propname, daughterboard->clocks[i]);
        g_free(propname);
    }
    sysbus_realize_and_unref(SYS_BUS_DEVICE(sysctl), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(sysctl), 0, map[SYSREGS]);


    dinfo = drive_get_next(IF_PFLASH);
    pflash0 = vi_pflash_cfi01_register(map[NORFLASH0], "vico.flash0",
                                       dinfo);
    if (!pflash0) {
        error_report("vico: error registering flash 0");
        exit(1);
    }

    if (map[NORFLASHALIAS] != -1) {
        /* Map flash 0 as an alias into low memory */
        flash0mem = sysbus_mmio_get_region(SYS_BUS_DEVICE(pflash0), 0);
        memory_region_init_alias(flashalias, NULL, "vico.flashalias",
                                 flash0mem, 0, VICO_FLASH_SIZE);
        memory_region_add_subregion(sysmem, map[NORFLASHALIAS], flashalias);
    }

    dinfo = drive_get_next(IF_PFLASH);
    if (!vi_pflash_cfi01_register(map[NORFLASH1], "vico.flash1",
                                  dinfo)) {
        error_report("vico: error registering flash 1");
        exit(1);
    }

    sram_size = 0x2000000;
    memory_region_init_ram(sram, NULL, "vico.sram", sram_size,
                           &error_fatal);
    memory_region_add_subregion(sysmem, map[SRAM], sram);

    vram_size = 0x800000;
    memory_region_init_ram(vram, NULL, "vico.vram", vram_size,
                           &error_fatal);
    memory_region_add_subregion(sysmem, map[VIDEORAM], vram);

    /* 0x4e000000 LAN9118 Ethernet */
    // if (nd_table[0].used) {
    //     lan9118_init(&nd_table[0], map[ETHERNET], pic[15]);
    // }

    /* USB: not modelled */

    /* DAPROM: not modelled */

    /* Create mmio transports, so the user can create virtio backends
     * (which will be automatically plugged in to the transports). If
     * no backend is created the transport will just sit harmlessly idle.
     */
    for (i = 0; i < NUM_VIRTIO_TRANSPORTS; i++) {
        sysbus_create_simple("virtio-mmio", map[VIRTIO] + 0x200 * i,
                             pic[40 + i]);
    }

    daughterboard->bootinfo.ram_size = machine->ram_size;
    daughterboard->bootinfo.nb_cpus = machine->smp.cpus;
    daughterboard->bootinfo.board_id = VICO_BOARD_ID;
    daughterboard->bootinfo.loader_start = daughterboard->loader_start;
    daughterboard->bootinfo.smp_loader_start = map[SRAM];
    daughterboard->bootinfo.smp_bootreg_addr = map[SYSREGS] + 0x30;
    daughterboard->bootinfo.gic_cpu_if_addr = daughterboard->gic_cpu_if_addr;
    daughterboard->bootinfo.modify_dtb = vico_modify_dtb;
    /* When booting Linux we should be in secure state if the CPU has one. */
    daughterboard->bootinfo.secure_boot = vms->secure;
    arm_load_kernel(ARM_CPU(first_cpu), machine, &daughterboard->bootinfo);
}

static bool vico_get_secure(Object *obj, Error **errp)
{
    VicoMachineState *vms = VICO_MACHINE(obj);

    return vms->secure;
}

static void vico_set_secure(Object *obj, bool value, Error **errp)
{
    VicoMachineState *vms = VICO_MACHINE(obj);

    vms->secure = value;
}


static void vico_instance_init(Object *obj)
{
    VicoMachineState *vms = VICO_MACHINE(obj);

    /* EL3 is enabled by default on vico */
    vms->secure = true;
}


static void vico_a9_instance_init(Object *obj)
{
    VicoMachineState *vms = VICO_MACHINE(obj);

    /* The A9 doesn't have the virt extensions */
    vms->virt = false;
}

static void vico_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "ARM Versatile Express";
    mc->init = vico_common_init;
    mc->max_cpus = 4;
    mc->ignore_memory_transaction_failures = true;
    mc->default_ram_id = "vico.highmem";

    object_class_property_add_bool(oc, "secure", vico_get_secure,
                                   vico_set_secure);
    object_class_property_set_description(oc, "secure",
                                          "Set on/off to enable/disable the ARM "
                                          "Security Extensions (TrustZone)");
}

static void vico_a9_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);
    VicoMachineClass *vmc = VICO_MACHINE_CLASS(oc);

    mc->desc = "ARM Versatile Express for Cortex-A9";
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-a9");

    vmc->daughterboard = &a9_daughterboard;
}


static const TypeInfo vico_info = {
    .name = TYPE_VICO_MACHINE,
    .parent = TYPE_MACHINE,
    .abstract = true,
    .instance_size = sizeof(VicoMachineState),
    .instance_init = vico_instance_init,
    .class_size = sizeof(VicoMachineClass),
    .class_init = vico_class_init,
};

static const TypeInfo vico_a9_info = {
    .name = TYPE_VICO_A9_MACHINE,
    .parent = TYPE_VICO_MACHINE,
    .class_init = vico_a9_class_init,
    .instance_init = vico_a9_instance_init,
};

static void vico_machine_init(void)
{
    type_register_static(&vico_info);
    type_register_static(&vico_a9_info);
    //type_register_static(&vico_a15_info);
}

type_init(vico_machine_init);
