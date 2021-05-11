#include "multi.h"
#include "gba.h"
#include </opt/devkitpro/libtonc/include/tonc_irq.h>


static int multiplayer_is_master()
{
    return (REG_SIOCNT & (1 << 2)) == 0 && (REG_SIOCNT & (1 << 3));
}


enum {
    MULTI_DEVICE_READY = 0xAAAA,
    MULTI_DEVICE_START = ~0xAAAA,
};


static int multi_validate_modes()
{
    return REG_SIOCNT & (1 << 3);
}


int multi_done = 0;


volatile int sio_got_intr = 0;


static void multi_connect_serial_isr()
{
    sio_got_intr = 1;
}


static void multi_connect_check_device_ready(int* connection_mask,
                                             u16 state,
                                             multi_PlayerId device_id,
                                             multi_ConnectedCallback callback)
{
    if (state == MULTI_DEVICE_READY &&
        !(*connection_mask & device_id)) {
        *connection_mask |= device_id;
        callback(device_id, 1);
    } else if (state != MULTI_DEVICE_READY &&
               *connection_mask & device_id) {
        *connection_mask &= ~device_id;
        callback(device_id, 0);
    }
}


static void multi_connect_check_devices(int* connection_mask,
                                        multi_ConnectedCallback callback)
{
    multi_connect_check_device_ready(connection_mask,
                                     REG_SIOMULTI0,
                                     multi_PlayerId_host,
                                     callback);

    multi_connect_check_device_ready(connection_mask,
                                     REG_SIOMULTI1,
                                     multi_PlayerId_p1,
                                     callback);

    multi_connect_check_device_ready(connection_mask,
                                     REG_SIOMULTI2,
                                     multi_PlayerId_p2,
                                     callback);

    multi_connect_check_device_ready(connection_mask,
                                     REG_SIOMULTI3,
                                     multi_PlayerId_p3,
                                     callback);
}


multi_Status multi_connect(multi_ConnectedCallback callback)
{
    REG_RCNT = R_MULTI;
    REG_SIOCNT = SIO_MULTI;
    REG_SIOCNT |= SIO_IRQ | SIO_115200;

    irq_add(II_SERIAL, multi_connect_serial_isr);

    int connection_mask = 0;

    while (!multi_validate_modes()) {
        // Wait until all gba consoles are in the proper serial device mode.
    }

    if (multiplayer_is_master()) {
        while (1) {
            REG_SIOMLT_SEND = MULTI_DEVICE_READY;
            REG_SIOCNT |= SIO_START;

            for (int i = 0; i < 20000; ++i) {
                // FIXME... busy wait for now
            }

            multi_connect_check_devices(&connection_mask, callback);
        }
    } else {
        REG_SIOMLT_SEND = MULTI_DEVICE_READY;

        while (1) {
            REG_SIOMLT_SEND = MULTI_DEVICE_READY;

            while (!sio_got_intr) ; // Wait for serial interrupt.
            sio_got_intr = 0;

            multi_connect_check_devices(&connection_mask, callback);
        }
    }

    return multi_Status_success;
}


multi_Status multi_send(multi_Message* message)
{

    return multi_Status_failure;
}
