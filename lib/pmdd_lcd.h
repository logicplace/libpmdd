/* Pokemon mini Direct Drawing library header
 * Copyright 2022 Sapphire Becker (logicplace.com)
 * MIT Licensed
 * 
 * Note that all mentions of RAM below refer to the on-LCD chip RAM.
 */
#ifndef __PMDD_LCD_H__
#define __PMDD_LCD_H__

#include "pm.h"
#include <stdint.h>

//// Arguments for macros defined below. See macros for usage.
#define OFF 0
#define ON 1
#define NORMAL 0
#define REVERSE 1
#define DARK 1


//// Macros for low-level ops

// Set column address. 0-131
uint8_t LCD_COL(uint8_t);

// Set page address (where each page is 8 rows (pixels) tall). 0-8
#define LCD_PAGE(x) \
    LCD_CTRL = 0xb0 | ((x) & 0x0f)

// Set "Display Start Line", essentially shifting the lines up by x. 0-63
#define LCD_SHIFT(x) \
    LCD_CTRL = 0x40 | ((x) & 0x3f)

/* ADC Select (Segment Driver Direction Select), reverses columns. x may be:
 * NORMAL  - Column indexes on the screen are 0~95 (inclusive)
 * REVERSE - Column indexes on the screen are 131~36 (inclusive)
 */
#define LCD_ADC(x) \
    LCD_CTRL = 0xa0 | ((x) & 1)

/* Select "Common Output Mode", essentially inverting rows (not just pages)
 * NORMAL  - Page indexes on the screen are 0~7 (inclusive), MSB of data bit is lowest row in page
 * REVERSE - Page indexes on the screen are 7~0 (inclusive), MSB of data bit is highest row in page
 * TODO: Page 8 is below the screen in NORMAL and likely above the screen in REVERSE,
 *       and LCD_SHIFT shifts up in NORMAL but likely down in REVERSE.
 *       No idea what shifting past 8 does in either case...
 */
#define LCD_ROW(x)  \
    LCD_CTRL = 0xc0 | (((x) & 1) << 3)

// Called "Electronic Volume" in the docs. 0-63, where 0 is off and 32 is default
#define LCD_CONTRAST(x) \
    LCD_CTRL = 0x81; \
    LCD_CTRL = (x)

/* LCD Voltage Bias. x may be:
 * NORMAL - 1/9th
 * DARK   - 1/7th
 */
#define LCD_VBIAS(x) \
    LCD_CTRL = 0xa2 | ((x) & 1)

/* Turn all LCD pixels on (without affecting RAM). x can be:
 * OFF - disable effect
 * ON  - enable effect, turning all pixels on (TODO: with invert enabled, are they all off?)
 */
#define LCD_SETALL(x) \
    LCD_CTRL = 0xa4 | ((x) & 1)

/* Invert all LCD pixels (without affecting RAM). x can be:
 * OFF - disable effect
 * ON  - enable effect, inverting all pixels
 */
#define LCD_INVERT(x) \
    LCD_CTRL = 0xa6 | ((x) & 1)

// Turn on/off display. x can be ON or OFF
#define LCD_PWR(x) \
    LCD_CTRL = 0xae | ((x) & 1)

/* Read/Modify/Write mode, where reads won't increase the column pointer and column assignment is disabled
 * ON  - begin Read/Modify/Write mode
 * OFF - end Read/Modify/Write mode
 */
#define LCD_RMW(x) \
    LCD_CTRL = (x) ? 0xe0 : 0xee

/* Reset display to default state, specifically:
 * LCD_RMW(OFF) - releases the read/modify/write mode
 * LCD_STATIC(0) - resets the static indicator: (D1, D2) = (0, 0)
 * LCD_SHIFT(0) - initializes the display start line
 * LCD_COL(0) - resets the column address
 * LCD_PAGE(0) - resets the page address
 * LCD_ROW(NORMAL) - resets the common output mode
 * resets the V5 voltage regulator internal resistor ratio
 * LCD_CONTRAST(32) - resets the electronic volume
 * LCD_NOP() - releases the test mode
 There is no impact on the display data RAM
 */
#define LCD_RESET() LCD_CTRL = 0xe2

// Also LCD_TEST(OFF) but docs say don't use test mode so...
#define LCD_NOP() LCD_CTRL = 0xe3

// UNCONFIRMED: LCD_TEST(ON) LCD_CTRL = 0xf0

/* UNCONFIRMED: Power Controller Set, x of for 0bBVF
 * 1 is on, 0 is off for each bit
 * B = Booster circuit
 * V = Voltage regulator circuit
 * F = V/F, Voltage follower circuit
 * Docs say it's only enabled in "master operation"
 */
#define LCD_PWR_CTL(x) \
    LCD_CTRL = 0x28 | ((x) & 0x07)

// UNCONFIRMED: Set V5 Voltage Regulator Internal Resistor Ratio. 0-7, small to large
// Docs say it's only enabled in "master operation"
#define LCD_V5(x) \
    LCD_CTRL = 0x20 | ((x) & 0x07)

/* UNCONFIRMED: Static Indicator, no clue. x can be:
 * OFF - disable
 * 1 - blinking at approximately one second intervals
 * 2 - blinking at approximately half second intervals
 * 3 - constantly on
 */
#define LCD_STATIC(x) \
    if (x) { \
        LCD_CTRL = 0xad; \
        LCD_CTRL = x & 3; \
    } \
    else LCD_CTRL = 0xac

// UNCONFIRMED: Compound commands for sleep and standby
#define LCD_SLEEP() \
    LCD_STATIC(0) \
    LCD_PWR(OFF) \
    LCD_SETALL(ON)

#define LCD_STANDBY(x) \
    LCD_STATIC(x) \
    LCD_PWR(OFF) \
    LCD_SETALL(ON)

// move from standby to sleep
#define LCD_AROUSE() \
    LCD_SETALL(OFF)

// end sleep mode
#define LCD_AWAKEN(x) \
    LCD_SETALL(OFF) \
    LCD_STATIC(x)


//// Library methods

#define lcd_seek(c,p) \
    LCD_COL(c); \
    LCD_PAGE(p)

void lcd_write(uint8_t *data, uint8_t len);
void lcd_blit(uint8_t *data, uint8_t *mask, uint8_t len);
void lcd_read(uint8_t len, uint8_t *out);

void lcd_clear();

#endif // __PMDD_LCD_H__
