/*
 * Copyright 2021 Victor Ardouin
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "hw/char/nrf52_uart.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "migration/vmstate.h"
#include "trace.h"

//fonction vérifiant si un irq à été claquée
static void nrf52_uart_update_irq(NRF52UARTState *s)
{
    bool irq = false;

    irq |= (s->reg[R_UART_RXDRDY] &&
            (s->reg[R_UART_INTEN] & R_UART_INTEN_RXDRDY_MASK));
    irq |= (s->reg[R_UART_TXDRDY] &&
            (s->reg[R_UART_INTEN] & R_UART_INTEN_TXDRDY_MASK));
    irq |= (s->reg[R_UART_ERROR]  &&
            (s->reg[R_UART_INTEN] & R_UART_INTEN_ERROR_MASK));
    irq |= (s->reg[R_UART_RXTO]   &&
            (s->reg[R_UART_INTEN] & R_UART_INTEN_RXTO_MASK));

    qemu_set_irq(s->irq, irq);
}

//fonction quand l'utilisateur va vouloir lire un registre de l'UART
static uint64_t uart_read(void *opaque, hwaddr addr, unsigned int size)
{
    NRF52UARTState *s = NRF52_UART(opaque);
    uint64_t r;

    if (!s->enabled) {
        return 0;
    }
    //Switch test les valeurs de addr
    switch (addr) {
    case A_UART_RXD:
        r = s->rx_fifo[s->rx_fifo_pos];
        if (s->rx_started && s->rx_fifo_len) {
            s->rx_fifo_pos = (s->rx_fifo_pos + 1) % UART_FIFO_LENGTH;
            s->rx_fifo_len--;
            if (s->rx_fifo_len) {
                s->reg[R_UART_RXDRDY] = 1;
                nrf52_uart_update_irq(s);
            }
            qemu_chr_fe_accept_input(&s->chr);
        }
        break;
    case A_UART_INTENSET:
    case A_UART_INTENCLR:

    case A_UART_SHORTS:
    // case A_UART_INTEN:
    //     r = s->reg[R_UART_INTEN];
    //     break;
    // default:
    //     r = s->reg[addr / 4];
    //     break;
    // }

    trace_nrf52_uart_read(addr, r, size);

    return r;
}

static gboolean uart_transmit(GIOChannel *chan, GIOCondition cond, void *opaque)
{
    NRF52UARTState *s = NRF52_UART(opaque);
    int r;
    uint8_t c = s->reg[R_UART_TXD];

    s->watch_tag = 0;

    r = qemu_chr_fe_write(&s->chr, &c, 1);
    /* qemu_chr_fe_write: &c = the data; 1 = the number of bytes to send
     *
     * Write data from front end to a backend character.
     *
     * Returns: the number of bytes consumed (0 if no associated Chardev)*/
    if (r <= 0) {
        s->watch_tag = qemu_chr_fe_add_watch(&s->chr, G_IO_OUT | G_IO_HUP,
                                             uart_transmit, s);
        if (!s->watch_tag) {
            /* The hardware has no transmit error reporting,
             * so silently drop the byte
             */
            goto buffer_drained;
        }
        return FALSE;
    }

buffer_drained:
    s->reg[R_UART_TXDRDY] = 1;
    s->pending_tx_byte = false;
    return FALSE;
}

static void uart_cancel_transmit(NRF52UARTState *s)
{
    if (s->watch_tag) {
        g_source_remove(s->watch_tag);
        s->watch_tag = 0;
    }
}

static void uart_write(void *opaque, hwaddr addr,
                       uint64_t value, unsigned int size)
{
    NRF52UARTState *s = NRF52_UART(opaque);

    trace_nrf52_uart_write(addr, value, size);

    if (!s->enabled && (addr != A_UART_ENABLE)) {
        return;
    }

    switch (addr) {
    case A_UART_TXD:
        if (!s->pending_tx_byte && s->tx_started) {
            s->reg[R_UART_TXD] = value;
            s->pending_tx_byte = true;
            uart_transmit(NULL, G_IO_OUT, s);
        }
        break;
    case A_UART_INTEN:
        s->reg[R_UART_INTEN] = value;
        break;
    case A_UART_INTENSET:
        s->reg[R_UART_INTEN] |= value;
        break;
    case A_UART_INTENCLR:
        s->reg[R_UART_INTEN] &= ~value;
        break;
    case A_UART_TXDRDY ... A_UART_RXTO:
        s->reg[addr / 4] = value;
        break;
    case A_UART_ERRORSRC:
        s->reg[addr / 4] &= ~value;
        break;
    case A_UART_RXD:
        break;
    case A_UART_RXDRDY:
        if (value == 0) {
            s->reg[R_UART_RXDRDY] = 0;
        }
        break;
    case A_UART_STARTTX:
        if (value == 1) {
            s->tx_started = true;
        }
        break;
    case A_UART_STARTRX:
        if (value == 1) {
            s->rx_started = true;
        }
        break;
    case A_UART_ENABLE:
        if (value) {
            if (value == 4) {
                s->enabled = true;
            }
            break;
        }
        s->enabled = false;
        value = 1;
        /* fall through */
    case A_UART_SUSPEND:
    case A_UART_STOPTX:
        if (value == 1) {
            s->tx_started = false;
        }
        /* fall through */
    case A_UART_STOPRX:
        if (addr != A_UART_STOPTX && value == 1) {
            s->rx_started = false;
            s->reg[R_UART_RXTO] = 1;
        }
        break;
    default:
        s->reg[addr / 4] = value;
        break;
    }
    nrf52_uart_update_irq(s);
}

static const MemoryRegionOps uart_ops = {
    .read =  uart_read,
    .write = uart_write,
    .endianness = DEVICE_LITTLE_ENDIAN,
};

static void nrf52_uart_reset(DeviceState *dev)
{
    NRF52UARTState *s = NRF52_UART(dev);

    s->pending_tx_byte = 0;

    uart_cancel_transmit(s);

    memset(s->reg, 0, sizeof(s->reg));

    s->reg[R_UART_PSELRTS] = 0xFFFFFFFF;
    s->reg[R_UART_PSELTXD] = 0xFFFFFFFF;
    s->reg[R_UART_PSELCTS] = 0xFFFFFFFF;
    s->reg[R_UART_PSELRXD] = 0xFFFFFFFF;
    s->reg[R_UART_BAUDRATE] = 0x4000000;

    s->rx_fifo_len = 0;
    s->rx_fifo_pos = 0;
    s->rx_started = false;
    s->tx_started = false;
    s->enabled = false;
}

static void uart_receive(void *opaque, const uint8_t *buf, int size)
{

    NRF52UARTState *s = NRF52_UART(opaque);
    int i;

    if (size == 0 || s->rx_fifo_len >= UART_FIFO_LENGTH) {
        return;
    }

    for (i = 0; i < size; i++) {
        uint32_t pos = (s->rx_fifo_pos + s->rx_fifo_len) % UART_FIFO_LENGTH;
        s->rx_fifo[pos] = buf[i];
        s->rx_fifo_len++;
    }

    s->reg[R_UART_RXDRDY] = 1;
    nrf52_uart_update_irq(s);
}

static int uart_can_receive(void *opaque)
{
    NRF52UARTState *s = NRF52_UART(opaque);

    return s->rx_started ? (UART_FIFO_LENGTH - s->rx_fifo_len) : 0;
}

static void uart_event(void *opaque, QEMUChrEvent event)
{
    NRF52UARTState *s = NRF52_UART(opaque);

    if (event == CHR_EVENT_BREAK) {
        s->reg[R_UART_ERRORSRC] |= 3;
        s->reg[R_UART_ERROR] = 1;
        nrf52_uart_update_irq(s);
    }
}

static void nrf52_uart_realize(DeviceState *dev, Error **errp)
{
    NRF52UARTState *s = NRF52_UART(dev);

    qemu_chr_fe_set_handlers(&s->chr, uart_can_receive, uart_receive,
                             uart_event, NULL, s, NULL, true);
}

static void nrf52_uart_init(Object *obj)
{
    NRF52UARTState *s = NRF52_UART(obj);
    SysBusDevice *sbd = SYS_BUS_DEVICE(obj);

    memory_region_init_io(&s->iomem, obj, &uart_ops, s,
                          "nrf52_soc.uart", UART_SIZE);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->irq);
}

static int nrf52_uart_post_load(void *opaque, int version_id)
{
    NRF52UARTState *s = NRF52_UART(opaque);

    if (s->pending_tx_byte) {
        s->watch_tag = qemu_chr_fe_add_watch(&s->chr, G_IO_OUT | G_IO_HUP,
                                             uart_transmit, s);
    }

    return 0;
}

static const VMStateDescription nrf52_uart_vmstate = {
    .name = "nrf52_soc.uart",
    .post_load = nrf52_uart_post_load,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(reg, NRF52UARTState, 0x56C),
        VMSTATE_UINT8_ARRAY(rx_fifo, NRF52UARTState, UART_FIFO_LENGTH),
        VMSTATE_UINT32(rx_fifo_pos, NRF52UARTState),
        VMSTATE_UINT32(rx_fifo_len, NRF52UARTState),
        VMSTATE_BOOL(rx_started, NRF52UARTState),
        VMSTATE_BOOL(tx_started, NRF52UARTState),
        VMSTATE_BOOL(pending_tx_byte, NRF52UARTState),
        VMSTATE_BOOL(enabled, NRF52UARTState),
        VMSTATE_END_OF_LIST()
    }
};

static Property nrf52_uart_properties[] = {
    DEFINE_PROP_CHR("chardev", NRF52UARTState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void nrf52_uart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = nrf52_uart_reset;
    dc->realize = nrf52_uart_realize;
    device_class_set_props(dc, nrf52_uart_properties);
    dc->vmsd = &nrf52_uart_vmstate;
}

static const TypeInfo nrf52_uart_info = {
    .name = TYPE_NRF52_UART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(NRF52UARTState),
    .instance_init = nrf52_uart_init,
    .class_init = nrf52_uart_class_init
};

static void nrf52_uart_register_types(void)
{
    type_register_static(&nrf52_uart_info);
}

type_init(nrf52_uart_register_types)
