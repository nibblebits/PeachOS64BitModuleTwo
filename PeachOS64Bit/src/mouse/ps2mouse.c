#include "mouse/ps2mouse.h"
#include "idt/irq.h"
#include "idt/idt.h"
#include "io/io.h"
#include "io/tsc.h"
#include "kernel.h"
#include "status.h"
#include "graphics/graphics.h"
int ps2_mouse_init(struct mouse* mouse);

struct mouse ps2_mouse = {
    .name= {"ps2_mouse"},
    .init=ps2_mouse_init
};

struct ps2_mouse ps2_mouse_private = {0};


int ps2_mouse_wait(int type)
{
    int res = -ETIMEOUT;
    if(type == PS2_WAIT_FOR_INPUT_TO_CLEAR)
    {
        udelay(100000);
        if((insb(PS2_STATUS_PORT) & 2) == 0)
        {
            res = 0;
        }

        return res;
    }

    udelay(100000);
    if(insb(PS2_STATUS_PORT) & 1)
    {
        res = 0;
    }
    return res;
}

void ps2_mouse_write(uint8_t byte)
{
    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    outb(PS2_STATUS_PORT, PS2_WRITE_TO_MOUSE);
    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    outb(PS2_COMMUNICATION_PORT, byte);
}


int ps2_mouse_init(struct mouse* mouse)
{
    int res = 0;
    idt_register_interrupt_callback(ISR_MOUSE_INTERRUPT, ps2_mouse_handle_interrupt);
    IRQ_enable(IRQ_PS2_MOUSE);

    ps2_mouse_wait(PS2_WAIT_FOR_INPUT_TO_CLEAR);
    outb(PS2_STATUS_PORT, PS2_COMMAND_ENABLE_SECOND_PORT);

    ps2_mouse_wait(PS2_WAIT_FOR_INPUT_TO_CLEAR);
    outb(PS2_STATUS_PORT, PS2_READ_CONFIG_BYTE);
    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    int config_val = insb(PS2_COMMUNICATION_PORT);
    config_val &= ~(1 << 5); // Clear bit 5 to enable the clock
    config_val |= (1 << 1); // Set bit 1 to enable interrupts;
    ps2_mouse_wait(PS2_WAIT_FOR_INPUT_TO_CLEAR);
    outb(PS2_STATUS_PORT, PS2_UPDATE_CONFIG_BYTE);
    outb(PS2_COMMUNICATION_PORT, config_val);

    ps2_mouse_write(PS2_MOUSE_RESET);
    // Wait for ACK
    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    uint8_t ack = insb(PS2_COMMUNICATION_PORT);
    if (ack != PS2_MOUSE_ACK)
    {
        print("Mouse reset: no ack received\n");
        return -EIO;
    }

    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    uint8_t self_test = insb(PS2_COMMUNICATION_PORT);
    if (self_test != PS2_MOUSE_SELF_TEST_PASS)
    {
        print("Mouse reset: self-test failed\n");
        return -EIO;
    }

    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    uint8_t device_id = insb(PS2_COMMUNICATION_PORT);
    if (device_id)
    {
        print("Mouse detected\n");
    }

    ps2_mouse_write(PS2_MOUSE_ENABLE_PACKET_STREAMING);
    ps2_mouse_wait(PS2_WAIT_FOR_OUTPUT_TO_BE_SET);
    ack = insb(PS2_COMMUNICATION_PORT);
    if (ack != PS2_MOUSE_ACK)
    {
        print("Mouse enable data reporting: no ack received\n");
        return -EIO;
    }

    mouse->private = &ps2_mouse_private;
    ps2_mouse_private.device_id = device_id;
    ps2_mouse_private.mouse_packet_size = 3;
    if (device_id == PS2_SCROLL_WHEEL_MOUSE_DEVICE_ID)
    {
        ps2_mouse_private.mouse_packet_size = 4;
    }

out:
    return res;
}

struct mouse* ps2_mouse_get()
{
    return &ps2_mouse;
}