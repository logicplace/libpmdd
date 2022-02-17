#include "pm.h"
#include <stdint.h>

#include "pmdd.h"

void lcd_write(uint8_t *data, uint8_t len) {
    // Overwrite mem in LCD RAM at cursor
    for (; len; --len, ++data) {
        LCD_DATA = *data;
    }
}

void lcd_blit(uint8_t *data, uint8_t *mask, uint8_t len) {
    // Read mem and blit with mask+data, following PRC logic
    // i.e. data bit (0 = white, 1 = black), mask (0 = opaque, 1 = transparent)
    LCD_RMW(ON);
    for (; len; --len, ++data, ++mask) {
        LCD_DATA |= (*data & ~*mask);
    }
    LCD_RMW(OFF);
}

void lcd_read(uint8_t len, uint8_t *out) {
    for (; len; --len, ++out) {
        *out = LCD_DATA;
    }
}
