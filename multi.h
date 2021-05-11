#pragma once


enum {
    MULTI_PACKET_SIZE = 12  // Default packet size.
};


typedef char multi_Byte;


typedef struct {
    multi_Byte data_[MULTI_PACKET_SIZE];
} multi_Message;


typedef enum {
    multi_Status_success,
    multi_Status_failure
} multi_Status;


typedef enum {
    multi_PlayerId_host = (1 << 0),
    multi_PlayerId_p1   = (1 << 1),
    multi_PlayerId_p2   = (1 << 2),
    multi_PlayerId_p3   = (1 << 3),
} multi_PlayerId;


typedef void (*multi_ConnectedCallback)(multi_PlayerId, int);
typedef int (*multi_ConnectionUpdateCallback)();


// A Blocking call, but invokes a callback as each other player
// connects. multi_connect will call the device connected callback for each
// connected gba device.
multi_Status multi_connect(multi_ConnectedCallback device_connected);


multi_Status multi_send(multi_Message* message);


multi_Status multi_receive_poll_begin(multi_Message** output);
multi_Status multi_receive_poll_end(multi_Message** output);
