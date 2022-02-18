/* Pokemon mini Direct Drawing library, frame buffer functions
 * Copyright 2022 Sapphire Becker (logicplace.com)
 * MIT Licensed
 * 
 * This portion is for functions which draw to the PRC's framebuffer.
 * Thus, the PRC must be enabled for this to work properly.
 */

#include "pm.h"
#include <stdint.h>

#include "pmdd_fb.h"

uint8_t _near *FB_CURSOR;


void fb_seek(uint8_t col, uint8_t page) {
    FB_CURSOR = FRAMEBUFFER + page * 96 + col;
}

void fb_write(uint8_t *data, uint8_t len) {
    // Overwrite mem in PRC's frame buffer at cursor
    for (; len; --len, ++FB_CURSOR, ++data) {
        *FB_CURSOR = *data;
    }
}

void fb_blit(uint8_t *data, uint8_t *mask, uint8_t len) {
    // Read mem and blit with mask+data, following PRC logic
    // i.e. data bit (0 = white, 1 = black), mask (0 = opaque, 1 = transparent)
    for (; len; --len, ++FB_CURSOR, ++data, ++mask) {
        *FB_CURSOR |= (*data & ~*mask);
    }
}

void fb_read(uint8_t len, uint8_t *out) {
    for (; len; --len, ++FB_CURSOR, ++out) {
        *out = *FB_CURSOR;
    }
}