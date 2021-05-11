#include "multi.h"
#include "gba.h"


static int multiplayer_is_master()
{
    return (REG_SIOCNT & (1 << 2)) == 0 && (REG_SIOCNT & (1 << 3));
}


enum {
    MULTI_DEVICE_READY = 0xAAAA,
    MULTI_DEVICE_START = ~0xAAAA,
};


multi_Status multi_connect(multi_ConnectedCallback callback)
{
    REG_RCNT = R_MULTI;
    REG_SIOCNT = SIO_MULTI;
    REG_SIOCNT |= SIO_IRQ | SIO_115200;

    int connection_mask = 0;

    if (multiplayer_is_master()) {
        while (1) {
            REG_SIOMLT_SEND = MULTI_DEVICE_READY;
            REG_SIOCNT |= SIO_START;

            for (int i = 0; i < 20000; ++i) {
                // FIXME... busy wait for now
           }

            if (!(connection_mask & multi_PlayerId_host)) {
                connection_mask |= multi_PlayerId_host;
                callback(multi_PlayerId_host);
            }

            if (REG_SIOMULTI1 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p1)) {
                connection_mask |= multi_PlayerId_p1;
                callback(multi_PlayerId_p1);
            }

            if (REG_SIOMULTI2 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p2)) {
                connection_mask |= multi_PlayerId_p2;
                callback(multi_PlayerId_p2);
            }

            if (REG_SIOMULTI3 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p3)) {
                connection_mask |= multi_PlayerId_p3;
                callback(multi_PlayerId_p3);
            }

        }
    } else {
        REG_SIOMLT_SEND = MULTI_DEVICE_READY;

        while (1) {
            if (REG_SIOMULTI0 == MULTI_DEVICE_START) {
                // The master says that we're done establishing the connection.
                break;
            }

            REG_SIOMLT_SEND = MULTI_DEVICE_READY;

            for (int i = 0; i < 20000; ++i) {
                // FIXME... busy wait for now
            }

            if (REG_SIOMULTI0 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_host)) {
                callback(multi_PlayerId_host);
            }

            if (REG_SIOMULTI1 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p1)) {
                callback(multi_PlayerId_p1);
            }

            if (REG_SIOMULTI2 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p2)) {
                callback(multi_PlayerId_p2);
            }

            if (REG_SIOMULTI3 == MULTI_DEVICE_READY &&
                !(connection_mask & multi_PlayerId_p3)) {
                callback(multi_PlayerId_p3);
            }
        }
    }

    return multi_Status_success;
}


multi_Status multi_send(multi_Message* message)
{

    return multi_Status_failure;
}
