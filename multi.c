#include "multi.h"
#include "gba.h"
#include </opt/devkitpro/libtonc/include/tonc_irq.h>


static int multiplayer_is_master()
{
    return (REG_SIOCNT & (1 << 2)) == 0;
}


// These enums don't have anything to do with the gba hardware, they're just
// magic constants that we're using during startup to detect which gbas are want
// to connect.
enum {
    MULTI_DEVICE_READY = 0xAA,

    // The host (master) device will broadcast a start command when the host
    // player decides to start the multiplayer game. (e.g. when the host player
    // decides that enough players have connected, and presses a button, or
    // something).
    MULTI_DEVICE_START = 0xFF,
};


// Cache our device's multiplayer id.
static multi_PlayerId g_multi_id = multi_PlayerId_unknown;

volatile int sio_got_intr = 0;


// This is just some boilerplate code for waiting on a serial interrupt, used
// while initializing the connection. We want to wait until the transmission is
// complete before reading serial registers.
static void multi_connect_serial_isr()
{
    sio_got_intr = 1;

    // NOTE: it's only safe to read the multiplayer id immediatly after a
    // transmission, otherwise, the register might contain a garbage value.
    g_multi_id = (REG_SIOCNT & (0x30)) >> 4;
}


static void multi_connect_check_device_ready(int* connection_mask,
                                             u16 state,
                                             multi_PlayerId device_id,
                                             multi_ConnectedCallback callback)
{
    if (state == MULTI_DEVICE_READY && !(*connection_mask & device_id)) {
        *connection_mask |= device_id;
        callback(device_id, 1);
    } else if (state != MULTI_DEVICE_READY && *connection_mask & device_id) {
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


static void __attribute__((noinline)) busy_wait(unsigned max)
{
    for (unsigned i = 0; i < max; i++) {
        __asm__ volatile("" : "+g"(i) : :);
    }
}


static void multi_serial_init();


multi_Status multi_connect(multi_ConnectedCallback callback,
                           multi_ConnectionHostCallback host_callback)
{
    REG_RCNT = R_MULTI;
    REG_SIOCNT = SIO_MULTI;
    REG_SIOCNT |= SIO_IRQ | SIO_115200;

    irq_add(II_SERIAL, multi_connect_serial_isr);

    int connection_mask = 0;

    if (multiplayer_is_master()) {
        while (1) {
            // Ok, so we're going to send out a ready integer constant, and see
            // which devices ping back a ready response.
            REG_SIOMLT_SEND = MULTI_DEVICE_READY;
            REG_SIOCNT |= SIO_START;

            // FIXME... busy wait for now. We should really be waiting on a
            // timer interrupt. But I'm feeling lazy.
            busy_wait(20000);

            multi_connect_check_devices(&connection_mask, callback);

            busy_wait(10000);

            // When the host determines that it's time to advance to an active
            // multiplayer session, it writes a start command, and returns.
            if (host_callback()) {
                REG_SIOMLT_SEND = MULTI_DEVICE_START;
                REG_SIOCNT |= SIO_START;

                // Wait a bit for the start command to propagate.
                busy_wait(40000);

                multi_serial_init();

                return multi_Status_success;
            }
        }
    } else {
        REG_SIOMLT_SEND = MULTI_DEVICE_READY;

        while (1) {
            REG_SIOMLT_SEND = MULTI_DEVICE_READY;

            while (!sio_got_intr) ; // Wait for serial interrupt.
            sio_got_intr = 0;

            // If we've received a start command from the master, now let's set
            // up the multiplayer session.
            if (REG_SIOMULTI0 == MULTI_DEVICE_START) {

                multi_serial_init();

                return multi_Status_success;
            } else {
                multi_connect_check_devices(&connection_mask, callback);
            }

        }
    }

    return multi_Status_success;
}


static void multi_serial_init()
{
    REG_SIOMLT_SEND = 0;

    irq_add(II_SERIAL, NULL);
}


multi_PlayerId multi_id()
{
    return g_multi_id;
}
