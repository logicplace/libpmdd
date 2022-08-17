/* Pokemon mini Direct Drawing library, frame buffer header
 * Copyright 2022 Sapphire Becker (logicplace.com)
 * MIT Licensed
 */
#ifndef __PMDD_FB_H__
#define __PMDD_FB_H__

#include "pm.h"
#include <stdint.h>

#define FRAMEBUFFER ((uint8_t *)(0x1000))

//// Cursor access
extern uint8_t _near *FB_CURSOR;
#define FB_DATA (*FB_CURSOR)

//// Library methods

void fb_seek(uint8_t col, uint8_t page);
void fb_write(uint8_t _far *data, uint8_t len);
void fb_blit(uint8_t _far *data, uint8_t _far *mask, uint8_t len);
void fb_read(uint8_t len, uint8_t *out);

#endif // __PMDD_FB_H__
