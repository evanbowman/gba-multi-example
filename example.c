#include "multi.h"
#include </opt/devkitpro/libtonc/include/tonc_tte.h>
#include </opt/devkitpro/libtonc/include/tonc_video.h>
#include </opt/devkitpro/libtonc/include/tonc_irq.h>
#include </opt/devkitpro/libtonc/include/tonc_bios.h>
#include </opt/devkitpro/libtonc/include/tonc_oam.h>
#include <string.h>


static volatile unsigned int* keys = (volatile unsigned int*)0x04000130;


static void multi_connect_callback(multi_PlayerId player, int connected)
{
    switch (player) {
    case multi_PlayerId_host:
        if (connected) {
            tte_write("host connected\n");
        } else {
            tte_write("host disconnected\n");
        }
        break;

    case multi_PlayerId_p1:
        if (connected) {
            tte_write("p1 connected\n");
        } else {
            tte_write("p1 disconnected\n");
        }
        break;

    case multi_PlayerId_p2:
        if (connected) {
            tte_write("p2 connected\n");
        } else {
            tte_write("p2 disconnected\n");
        }
        break;

    case multi_PlayerId_p3:
        if (connected) {
            tte_write("p3 connected\n");
        } else {
            tte_write("p3 disconnected\n");
        }
        break;

    case multi_PlayerId_unknown:
        // This condition can't really happen anyway.
        tte_write("??? connected");
        break;
    }
}


static int multi_host_callback()
{
    // If the host player presses the B button, then return true, thus
    // indicating to the multi library that we're ready to establish a
    // connection.
    if ((~(*keys) & (1 << 1))) {
        return 1;
    }

    return 0;
}


volatile u16 host_counter = 0;
volatile u16 p1_counter = 0;
volatile u16 p2_counter = 0;
volatile u16 p3_counter = 0;

int my_counter = 0;


void multi_data_function(unsigned short host_data,
                         unsigned short p1_data,
                         unsigned short p2_data,
                         unsigned short p3_data,
                         volatile unsigned short* output)
{
    host_counter = host_data;
    p1_counter = p1_data;
    p2_counter = p2_data;
    p3_counter = p3_data;

    *output = my_counter;
}


OBJ_ATTR obj_buffer[128];


void init_sprite_graphics()
{
    REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_1D;

    // Because I'm feeling lazy. Just memset vram to ff, and draw colored boxes
    // as sprites.
    memset(&tile_mem[4][0], 0xff, 2000);
    for (int i = 0; i < 16; ++i) {
        pal_obj_mem[i] = CLR_RED;
    }
    for (int i = 16; i < 32; ++i) {
        pal_obj_mem[i] = CLR_GREEN;
    }
    for (int i = 32; i < 48; ++i) {
        pal_obj_mem[i] = CLR_YELLOW;
    }
    for (int i = 48; i < 64; ++i) {
        pal_obj_mem[i] = CLR_BLUE;
    }

    oam_init(obj_buffer, 128);

    obj_set_attr(&obj_buffer[0],
                 ATTR0_SQUARE,
                 ATTR1_SIZE_32,
                 ATTR2_PALBANK(0) | 0);

    obj_set_attr(&obj_buffer[1],
                 ATTR0_SQUARE,
                 ATTR1_SIZE_32,
                 ATTR2_PALBANK(1) | 0);

    obj_set_attr(&obj_buffer[2],
                 ATTR0_SQUARE,
                 ATTR1_SIZE_32,
                 ATTR2_PALBANK(2) | 0);

    obj_set_attr(&obj_buffer[3],
                 ATTR0_SQUARE,
                 ATTR1_SIZE_32,
                 ATTR2_PALBANK(3) | 0);
}


int main()
{
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

    tte_init_se_default(0, BG_CBB(0)|BG_SBB(31));

    tte_write("multiplayer test...\n");

    // Wait for player to press A button. We don't want to attempt to connect
    // right away, before all players power on their gbas. If all gbas are not
    // turned on when commencing a serial connection, stuff might not connect
    // correctly.
    while (!(~(*keys) & 1)) {
        // ...
    }

    multi_Status connect_result = multi_connect(multi_connect_callback,
                                                multi_host_callback,
                                                multi_data_function);

    if (connect_result == multi_Status_failure) {
        return 1;
    }

    tte_erase_screen();

    init_sprite_graphics();

    while (true) {
        if (~(*keys) & KEY_DOWN) {
            my_counter -= 1;
        }
        if (~(*keys) & KEY_UP) {
            my_counter += 1;
        }

        obj_set_pos(&obj_buffer[0], host_counter, 80);
        obj_set_pos(&obj_buffer[1], p1_counter, 80);
        obj_set_pos(&obj_buffer[2], p2_counter, 80);
        obj_set_pos(&obj_buffer[3], p3_counter, 80);

        VBlankIntrWait();

        oam_copy(oam_mem, obj_buffer, 4);
    }
}
