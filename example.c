#include "multi.h"
#include </opt/devkitpro/libtonc/include/tonc_tte.h>
#include </opt/devkitpro/libtonc/include/tonc_video.h>
#include </opt/devkitpro/libtonc/include/tonc_irq.h>


static void text_init()
{
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

    tte_init_se(
                0,
                BG_CBB(0)|BG_SBB(31),
                0,
                CLR_YELLOW,
                14,
                NULL,
                NULL);

    pal_bg_bank[1][15]= CLR_RED;
    pal_bg_bank[2][15]= CLR_GREEN;
    pal_bg_bank[3][15]= CLR_BLUE;
    pal_bg_bank[4][15]= CLR_WHITE;
    pal_bg_bank[5][15]= CLR_MAG;
    pal_bg_bank[4][14]= CLR_GRAY;
}


static void text_put(const char* str)
{
    tte_write(str);
}


static void multi_connect_callback(multi_PlayerId player_connected)
{
    switch (player_connected) {
    case multi_PlayerId_host:
        text_put("host connected\n");
        break;

    case multi_PlayerId_p1:
        text_put("p1 connected\n");
        break;

    case multi_PlayerId_p2:
        text_put("p2 connected\n");
        break;

    case multi_PlayerId_p3:
        text_put("p3 connected\n");
        break;
    }
}


int main()
{
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    text_init();

    text_put("multiplayer test...\n");


    multi_Status connect_result = multi_connect(multi_connect_callback);

    if (connect_result == multi_Status_failure) {
        return 1;
    }
}
