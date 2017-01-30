#include <kernel/kernel.h>
#include <kernel/types.h>
#include <kernel/kerror.h>
#include <kernel/klog.h>
#include <kernel/asm/portio.h>

#include "kb.h"
#include "irq.h"
#include "ps2.h"
#include "con.h"
#include "panic.h"
#include "kio.h"

// TODO: Once we have dynamic memory allocation, remove this limit.
#define KB_HOOK_FUNC_COUNT_MAX 16

static int (*s_hook_funcs[KB_HOOK_FUNC_COUNT_MAX])(const struct kb_key *);

#define PS2_POLL_BUFFER_SIZE 16

static const u8 s_keymap[128] = {
    #include "keymap-en-us"
};

struct ps2_kb_packet {
    u8 status;
    u8 data;
};

static struct kb_key convert_packet(const struct ps2_kb_packet *p)
{
    struct kb_key key;
    struct ps2_kb_packet packet = *p;
    u8 scancode = packet.data & 0x7f;

    KZEROMEM(&key, sizeof(key));

    key.scancode = scancode;
    key.is_pressed = !(packet.data & 0x80);
    key.keycode = s_keymap[scancode];

    return key;
}

// Poll the ps2 controller until it returns no more data. Combine the keyboard
// device data with the controller status at that time, and place it into the
// given buffer of the specified length.
static int poll_ps2(struct ps2_kb_packet *buffer, int buffer_size)
{
    struct ps2_kb_packet *packet;
    u8 ps2_status;
    int i = 0;

    while (true) {
        // If the buffer size is reached, don't process any more packets.
        if (i > buffer_size) {
            klog_printf("kb: ps2 poll packet buffer full (%d packets)\n",
                buffer_size);
            FLUSH_INPUT_BUFFER();
            return -1;
        }

        // Get ps2 controller status
        ps2_status = inportb(PS2_PORT_STATUS);
        portwait();

        // Check that there's data waiting
        if (ps2_status & PS2_STATUS_OUTPUT_BUFFER_FULL) {
            packet = buffer + i;
            packet->status = ps2_status;
            packet->data = inportb(PS2_PORT_DATA);
            portwait();

            ++i;
        } else {
            break;
        }
    }

    // Return number of packets received
    return i;
}

static int call_hook_funcs(const struct kb_key *key)
{
    int i;

    for (i = 0; i < KB_HOOK_FUNC_COUNT_MAX; ++i) {
        if (s_hook_funcs[i]) {
            s_hook_funcs[i](key);
        }
    }

    return 0;
}

int kb_irq_hook(int irqnum) {
    struct ps2_kb_packet buffer[PS2_POLL_BUFFER_SIZE];
    int num_received = poll_ps2(buffer, PS2_POLL_BUFFER_SIZE);
    struct kb_key key;

    if (num_received) {
        key = convert_packet(buffer);
        call_hook_funcs(&key);
    }

    irq_done(irqnum);
    return 0;
}

int kb_init(void)
{
    KZEROMEM(s_hook_funcs, sizeof(s_hook_funcs));

    if (irq_set_hook(1, kb_irq_hook)) {
        klog_printf("kb: failed to hook irq\n");
        return 1;
    }

    klog_printf("kb: irq hooked\n");

    ps2_set_enabled(1, 1);

    kb_set_typematic_config(0x0b, 2);

    return 0;
}

int kb_set_typematic_config(int repeat_rate, int typematic_delay)
{
    struct ps2_kb_typematic_byte tb = { 0, 0 };

    if ((repeat_rate < 0) || (repeat_rate > 0x1f)) {
        return KERROR_ARG_OUT_OF_RANGE;
    }

    if ((typematic_delay < 0) || (typematic_delay > 3)) {
        return KERROR_ARG_OUT_OF_RANGE;
    }

    tb.repeat_rate = repeat_rate;
    tb.typematic_delay = typematic_delay;

    WAIT_FOR_OUTPUT_BUFFER();
    FLUSH_INPUT_BUFFER();

    outportb(PS2_PORT_CMD, PS2_KB_CMD_SET_TYPEMATIC_CONFIG);
    portwait();

    outportb(PS2_PORT_DATA, *((u8 *) &tb));
    portwait();

    if (inportb(PS2_PORT_DATA) == PS2_DEV_RESP_OK) {
        return KERROR_HARDWARE_PORT;
    }

    klog_printf("kb: set repeat rate to %#02x, typematic delay to %d\n",
        repeat_rate, typematic_delay);

    return 0;
}

int kb_add_hook(int (*func)(const struct kb_key *))
{
    int i;

    if (!func) {
        return KERROR_ARG_NULL;
    }

    for (i = 0; i < KB_HOOK_FUNC_COUNT_MAX; ++i) {
        if (!s_hook_funcs[i]) {
            s_hook_funcs[i] = func;
            return 0;
        }
    }

    klog_printf("kb: cannot add hook %p, limit of %d reached\n",
        func, KB_HOOK_FUNC_COUNT_MAX);

    return KERROR_LIMIT_EXCEEDED;
}

int kb_remove_hook(int (*func)(const struct kb_key *))
{
    int i;

    if (!func) {
        return KERROR_ARG_NULL;
    }

    for (i = 0; i < KB_HOOK_FUNC_COUNT_MAX; ++i) {
        if (s_hook_funcs[i] == func) {
            s_hook_funcs[i] = NULL;
            return 0;
        }
    }

    return KERROR_ARG_INVALID;
}
