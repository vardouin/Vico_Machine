#include "qemu/osdep.h"
#include "qapi/error.h"
#include "hw/arm/boot.h"
#include "hw/loader.h"

#include "hw/boards.h"
#include "qom/object.h"

#include "qemu/units.h"
#include "qemu/cutils.h"
#include "qemu/error-report.h"
#include "hw/registerfields.h"

#include "hw/arm/vico_machine.h"
#include "hw/cpu/a9mpcore.h"

struct VicoMachineState {
    MachineState         parent_obj;
    A9MPPrivState        soc;
    struct arm_boot_info binfo;
    MemoryRegion         sram;
    MemoryRegion         sram_m;
    MemoryRegion         flash;
    MemoryRegion         flash_m;
};
typedef struct VicoMachineState VicoMachineState;

struct VicoMachineClass {
    MachineClass parent;
    uint32_t     sram_size;
    uint32_t     flash_size;
    uint8_t      flashSectorNumber;
};
typedef struct VicoMachineClass VicoMachineClass;

#define TYPE_VICO_MACHINE       MACHINE_TYPE_NAME("vico-machine")
DECLARE_OBJ_CHEKERS(VicoMachineState, VicoMachineClass, VICO_MACHINE, TYPE_VICO_MACHINE)

////////////////////////__________METHODES MACHINE INIT______________///////////

/* Methodes pour boot */

static void write_smpboot(ARMCPU *cpu, const struct arm_boot_info *info)
{
    static const uint32_t smpboot[] = {
        0xe1a0e00f, /*    mov     lr, pc */
        0xe3a0fe00 + (BOARDSETUP_ADDR >> 4), /* mov pc, BOARDSETUP_ADDR */
        0xee100fb0, /*    mrc     p15, 0, r0, c0, c0, 5;get core ID */
        0xe7e10050, /*    ubfx    r0, r0, #0, #2       ;extract LSB */
        0xe59f5014, /*    ldr     r5, =0x400000CC      ;load mbox base */
        0xe320f001, /* 1: yield */
        0xe7953200, /*    ldr     r3, [r5, r0, lsl #4] ;read mbox for our core*/
        0xe3530000, /*    cmp     r3, #0               ;spin while zero */
        0x0afffffb, /*    beq     1b */
        0xe7853200, /*    str     r3, [r5, r0, lsl #4] ;clear mbox */
        0xe12fff13, /*    bx      r3                   ;jump to target */
        0x400000cc, /* (constant: mailbox 3 read/clear base) */
    };

    /* check that we don't overrun board setup vectors */
    QEMU_BUILD_BUG_ON(SMPBOOT_ADDR + sizeof(smpboot) > MVBAR_ADDR);
    /* check that board setup address is correctly relocated */
    QEMU_BUILD_BUG_ON((BOARDSETUP_ADDR & 0xf) != 0
                      || (BOARDSETUP_ADDR >> 4) >= 0x100);

    rom_add_blob_fixed_as("vico_smpboot", smpboot, sizeof(smpboot),
                          info->smp_loader_start,
                          arm_boot_address_space(cpu, info));
}

static void write_board_setup(ARMCPU *cpu, const struct arm_boot_info *info)
{
    arm_write_secure_board_setup_dummy_smc(cpu, info, MVBAR_ADDR);
}

static void reset_secondary(ARMCPU *cpu, const struct arm_boot_info *info)
{
    CPUState *cs = CPU(cpu);
    cpu_set_pc(cs, info->smp_loader_start);
}

static void setup_boot(MachineState *machine)
{
    VicoMachineState *s = VICO_MACHINE(machine);

    s->binfo.board_id = MACH_TYPE_CORTEXA9;
    s->binfo.ram_size = VICO_RAM_SIZE;
    s->binfo.nb_cpus = machine->smp.cpus;

    /* Bi-cortexA9 requires SMP setup */
    s->binfo.smp_loader_start = SMPBOOT_ADDR;
    s->binfo.write_secondary_boot = write_smpboot;
    s->binfo.secondary_cpu_reset_hook = reset_secondary;

    arm_load_kernel(&s->soc.cpu[0].core, machine, &s->binfo);
}

//Machine init

static void vico_machine_init(MachineState *machine)
{
    VicoMachineState *mms = TYPE_VICO_MACHINE(machine);
    VicoMachineClass *mmc = VICO_MACHINE_GET_CLASS(machine);
    MemoryRegion *system_memory = get_system_memory();
    MachineClass *mc = MACHINE_GET_CLASS(machine);
    DeviceState *a9mpcore, *sccdev;

    // uint32_t vcram_size;
    // DriveInfo *di;
    // BlockBackend *blk;
    // BusState *bus;
    //MemoryRegion *mr;

    // Errors tests
    if (strcmp(machine->cpu_type, mc->default_cpu_type) != 0) {
        error_report("This board can only be used with CPU %s",
                     mc->default_cpu_type);
        exit(1);
    }

    if (machine->ram_size != mc->default_ram_size) {
        char *sz = size_to_str(mc->default_ram_size);
        error_report("Invalid RAM size, should be %s", sz);
        g_free(sz);
        exit(EXIT_FAILURE);
    }

    memory_region_add_subregion_overlap(system_memory, 0,machine->ram, 0);

    object_initialize_child(OBJECT(mms), "soc", &mms->soc, TYPE_A9MPCORE_PRIV);
    object_property_add_const_link(OBJECT(&mms->soc), "memory", OBJECT(machine->ram));
    sysbus_realize(DEVICE(&mms->soc), NULL, &error_fatal);

}

static void vico_machine_class_init(MachineClass *mmc)
{
    mmc->init = vico_machine_init;
    mmc->alias = "vico";
    mmc->block_default_type = IF_SD;
    mmc->no_parallel = 1;
    mmc->no_floppy = 1;
    mmc->no_cdrom = 1;
    mmc->default_cpus = mmc->min_cpus = mmc->max_cpus = cores_count(TYPE_A9MPCORE_PRIV);
    mmc->default_ram_size = board_ram_size(TYPE_A9MPCORE_PRIV);
    mmc->default_ram_id = "ram";
};

static const TypeInfo vico_soc_info[] = {

    .name           = MACHINE_TYPE_NAME("Vico"),
    .parent         = TYPE_VICO_MACHINE,
    .class_init     = vico_machine_class_init,
};

DEFINE_TYPES(vico_machine_types)
