#include "pm.h"
#include "pmdd_fb.h"
#include "pmdd_lcd.h"

static uint8_t test;

#define DEBUG_CHAR (*(volatile unsigned char *)(REG_BASE+0xD0))
#define DEBUG_UHEX8 (*(volatile unsigned char *)(REG_BASE+0xD1))
#define DEBUG_UINT8 (*(volatile unsigned char *)(REG_BASE+0xD2))

// Use Oscillator 2 (31768Hz), Scale 2 (8192 Hz), and enable
#define enable_timer() \
    TMR1_OSC = 0x13; \
    TMR1_SCALE = 0x08 | 0x07; \
    TMR1_CTRL_L = 0x06

void wait_for(int8_t ticks);

#define pause_timer() \
    TMR1_CTRL = 0

#define wait_for_a() \
    while(KEY_PAD & KEY_A); \
    while(!(KEY_PAD & KEY_A))

void run_lcd_tests(void);
void run_fb_tests(void);
void die(void);

void dout(char *str) {
    for (; *str; ++str) {
        DEBUG_CHAR = *str;
    }
}

//#define dout(x)

int main(void)
{
    uint8_t keys;
    test = 2;

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

void run_lcd_tests(void) {
    uint8_t c, p, x, y;

    dout("Running LCD tests...\n");
    PRC_MODE = 0;
    //LCD_RESET();
    lcd_clear();

    // Basic write test
    dout("Starting basic write test...\n");
    for (p = 0; p < 8; ++p) {
        lcd_seek(0, p);
        for (c = 0; c < 132; ++c) {
            LCD_DATA = c;
        }
    }
    lcd_seek(0, 8);
    for (c = 132; c > 0; --c) {
        LCD_DATA = c;
    }

    dout("Press A to continue\n");
    wait_for_a();

    // Basic read test
    dout("Starting basic read test...\n");
    LCD_RMW(OFF);
    for (p = 0; p < 8; ++p) {
        lcd_seek(0, p);
        for (c = 0; c < 96; ++c) {
            // if (c) DEBUG_CHAR = ',';
            x = lcd_read1();
            // DEBUG_UHEX8 = x;
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
                for (; c < 8; ++c) LCD_DATA = 0xff;
                for (; c < 96; ++c) LCD_DATA = x;
                die();
            }
        }
        // DEBUG_CHAR = '\n';
    }

    dout("Press A to continue\n");
    wait_for_a();

    // Flip tests
    enable_timer();
    dout("Starting row reverse test...\n");
    LCD_ROW(REVERSE);
    wait_for(20);

    dout("Press A to continue\n");
    wait_for_a();
    LCD_ROW(NORMAL);

    // Shift test
    dout("Starting shift rows test...\n");
    for (c = 0; c <= 63; ++c) {
        LCD_SHIFT(c);
        wait_for(20);
    }
    pause_timer();

    dout("Press A to continue\n");
    wait_for_a();
    LCD_SHIFT(0);

    // Contrast test
    dout("Starting contrast test...\n");
    for (p = 0; p < 8; ++p) {
        lcd_seek(0, p);
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
        wait_for(20);
    }
    pause_timer();

    // TODO: vbias, setall, invert, pwr?, unconfirmed stuff?

    dout("Tests complete.\n");
    die();
}

void run_fb_tests(void) {
    uint8_t c, p, x, y;

    dout("Running framebuffer tests...\n");
    PRC_MODE = COPY_ENABLE;

    // Basic write test
    dout("Starting basic write test...\n");
    for (p = 0; p < 8; ++p) {
        fb_seek(0, p);
        for (c = 0; c < 132; ++c, ++FB_CURSOR) {
            FB_DATA = c;
        }
    }
    fb_seek(0, 8);
    for (c = 132; c > 0; --c, ++FB_CURSOR) {
        FB_DATA = c;
    }

    dout("Press A to continue\n");
    wait_for_a();

    // Basic read test
    dout("Starting basic read test...\n");
    for (p = 0; p < 8; ++p) {
        fb_seek(0, p);
        for (c = 0; c < 96; ++c, ++FB_CURSOR) {
            // if (c) DEBUG_CHAR = ',';
            x = FB_DATA;
            // DEBUG_UHEX8 = x;
            if (x != c) {
                dout("Mismatch at p=");
                DEBUG_UINT8 = p;
                dout(", c=");
                DEBUG_UINT8 = c;
                dout(", found=");
                DEBUG_UINT8 = x;
                dout(", next=");
                DEBUG_UINT8 = FB_DATA;
                DEBUG_CHAR = '\n';
                for (; c < 8; ++c) FB_DATA = 0xff;
                for (; c < 96; ++c) FB_DATA = x;
                die();
            }
        }
        // DEBUG_CHAR = '\n';
    }

    dout("Press A to continue\n");
    wait_for_a();

    dout("Tests complete.\n");
    die();
}

void wait_for(int8_t ticks) {
    TMR1_PRE_L = ticks;
    TMR1_CTRL_L |= 0x06;  // reset (and re-enable, if disabled)
    IRQ_ACT1 = IRQ1_TIM1_LO_UF;
    while (!(IRQ_ACT1 & IRQ1_TIM1_LO_UF));
}

void die(void) {
    dout("Press A to quit\n");
    wait_for_a();
    _int(0x48);
}
