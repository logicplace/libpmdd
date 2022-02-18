#include "pm.h"
#include "pmdd_fb.h"
#include "pmdd_lcd.h"

#define DEBUG_CHAR (*(volatile unsigned char *)(REG_BASE+0xD0))
#define DEBUG_UHEX8 (*(volatile unsigned char *)(REG_BASE+0xD1))
#define DEBUG_UINT8 (*(volatile unsigned char *)(REG_BASE+0xD2))

// Use Oscillator 2 (31768Hz), Scale 2 (8192 Hz), and enable
#define enable_timer() \
    TMR1_OSC = 0x13; \
    TMR1_SCALE = 0x08 | 0x02; \
    TMR1_CTRL = 0x06

void wait_for(int8_t ticks);

#define pause_timer() TMR1_CTRL = 0;

#define wait_for_a() \
    while(KEY_PAD & KEY_A); \
    while(!(KEY_PAD & KEY_A))

void run_lcd_tests();
void run_fb_tests();
void die();

void dout(char *str) {
    for (; *str; ++str) {
        DEBUG_CHAR = *str;
    }
}

int main(void)
{
    uint8_t keys;

    dout("Press A for LCD tests, B for frame buffer tests\n");

    for (;;) {
        keys = ~KEY_PAD;

        if (keys & KEY_A) {
            while(!(KEY_PAD & KEY_A));
            run_lcd_tests();
        }
        else if (keys & KEY_B) {
            while(!(KEY_PAD & KEY_B));
            run_fb_tests();
        }
    }
}

void run_lcd_tests() {
    uint8_t c, p, x, y;

    PRC_MODE = 0;
    //LCD_RESET();
    lcd_clear();

    // Basic write test
    dout("Starting basic write test...\n");
    for (p = 0; p < 8; ++p) {
        LCD_PAGE(p);
        LCD_COL(0);
        for (c = 0; c < 96; ++c) {
            LCD_DATA = c;
        }
    }

    dout("Press A to continue\n");
    wait_for_a();

    // Basic read test
    dout("Starting basic read test...\n");
    LCD_RMW(OFF);
    for (p = 0; p < 8; ++p) {
        LCD_PAGE(p);
        LCD_COL(0);
        for (c = 0; c < 96; ++c) {
            for (y = 0; (x = LCD_DATA) < c; ++y);
            if (y != 1) {
                dout("Took ");
                DEBUG_UINT8 = y;
                dout(" reads at p=");
                DEBUG_UINT8 = p;
                dout(", c=");
                DEBUG_UINT8 = c;
                DEBUG_CHAR = '\n';
            }
            if (x != c) {
                dout("Mismatch at p=");
                DEBUG_UINT8 = p;
                dout(", c=");
                DEBUG_UINT8 = c;
                dout(", found=");
                DEBUG_UINT8 = x;
                dout(", next=");
                DEBUG_UINT8 = LCD_DATA;
                DEBUG_CHAR = '\n';
                for (; c < 96; ++c) LCD_DATA = 0xff;
                die();
            }
        }
    }

    dout("Press A to continue\n");
    wait_for_a();

    // Contrast test
    dout("Starting contrast test...\n");
    for (p = 0; p < 8; ++p) {
        LCD_PAGE(p);
        LCD_COL(0);
        for (c = 0; c < 32; ++c) {
            LCD_DATA = 0x00;
        }
        for (; c < 64; ++c) {
            LCD_DATA = c & 1 ? 0xaa : 0x55;
        }
        for (; c < 96; ++c) {
            LCD_DATA = 0xff;
        }
    }
    enable_timer();
    for (c = 0; c <= 63; ++c) {
        LCD_CONTRAST(c);
        wait_for(10);
    }
    pause_timer();

    dout("Tests complete.\n");
    die();
}

void run_fb_tests() {

}

void wait_for(int8_t ticks) {
    uint8_t cur = TMR1_CNT_L, x;
    while (ticks > 0) {
        x = TMR1_CNT_L;
        ticks -= cur - x;
        cur = x;
    }
}

void die() {
    dout("Press A to quit\n");
    wait_for_a();
    _int(0x48);
}