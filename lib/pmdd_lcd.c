/* Pokemon mini Direct Drawing library, LCD functions
 * Copyright 2022 Sapphire Becker (logicplace.com)
 * MIT Licensed
 * 
 * This is the main library file for low level LCD functions.
 */

#include "pm.h"
#include <stdint.h>

#include "pmdd_lcd.h"

uint8_t LCD_COL(uint8_t x) {
    LCD_CTRL = ((x) & 0x0f);
    LCD_CTRL = 0x10 | (((x) & 0xf0) >> 4);
    return LCD_DATA;
}

uint8_t lcd_read1() {
    #pragma optimize 0
    uint8_t dummy = LCD_DATA;
    #pragma endoptimize
    return LCD_DATA;
}

void lcd_write(uint8_t *data, uint8_t len) {
    // Overwrite mem in LCD RAM at cursor
    for (; len; --len, ++data) {
        LCD_DATA = *data;
    }
}

void lcd_blit(uint8_t *data, uint8_t *mask, uint8_t len) {
    // Read mem and blit with mask+data, following PRC logic
    // i.e. data bit (0 = white, 1 = black), mask (0 = opaque, 1 = transparent)
    uint8_t dummy;
    LCD_RMW(ON);
    for (; len; --len, ++data, ++mask) {
        #pragma optimize 0
        dummy = LCD_DATA;
        #pragma endoptimize
        LCD_DATA |= (*data & ~*mask);
    }
    LCD_RMW(OFF);
}

void lcd_read(uint8_t len, uint8_t *out) {
    uint8_t dummy;
    for (; len; --len, ++out) {
        #pragma optimize 0
        dummy = LCD_DATA;
        #pragma endoptimize
        *out = LCD_DATA;
    }
}

void lcd_clear() {
    uint8_t c, p;

    for (p = 0; p <= 8; ++p) {
        LCD_PAGE(p);
        LCD_COL(0);
        for (c = 0; c <= 131; ++c) {
            LCD_DATA = 0;
        }
    }
}
