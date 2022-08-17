# libpmdd
Pokemon mini Direct Drawing library

## Status

Reading back the LCD does not work on hardware. The rest of the tests work in both PokeMini and on hardware. Not all LCD commands have been tested and blit is not expected to work properly due to the reading errors. fb_blit is untested but should work.

## LCD direct draw

The LCD is organized into 9 pages indexed 0 to 8 (taller than PM screen of 8 pages). Each page is 8 pixels high. Each page contains 132 columns wide (wider than the PM screen of 96 columns). By default page 0, column 0 is the top left 1x8 pixel section of the screen.

The main registers which you can USE (from pm.h) are LCD_CTRL and LCD_DATA. For the most part, this library makes it so you can avoid using these. The only thing you'll ever need to do with these, realistically, is write to LCD_DATA directly.

In order to use the LCD library effectively, it's best to disable the PRC entirely with `PRC_MODE = 0;` Not disabling it is possible, but requires you to use interrupts which are beyond the scope of this readme.

The LCD maintains an internal cursor for where to read and write from next. Normally, when you read or write, the cursor increments by one. However, the cursor will not loop to the next page when it reaches the end of a page; you must do this manually. When RMW mode is enabled, reading will not increment the cursor. The cursor position cannot be read back.

The commands you'll typically use are:

* Seek to a specific position: `lcd_seek(column, page);`
  * Note: use `LCD_COL(column);` and `LCD_PAGE(page);` directly if you don't need to change both.
* Clear the screen: `lcd_clear();`
* Write one column: `LCD_DATA = x;`
* Write tile-like data from ROM: `lcd_write(data addr, len);`
* Blit sprite-like data from ROM: `lcd_blit(data addr, mask addr, len);`
  * For dealing with data not aligned to page boundaries, you'll have to shift it yourself in RAM and blit from there.

Other commands:

* Read one column: `x = lcd_read1();`
* Read multiple columns: `lcd_read(len, &out);`
* Offset starting line (shifts screen up): `LCD_SHIFT(line);` Default is 0.
* Reverse rows: `LCD_ROW(REVERSE);` Default is `LCD_ROW(NORMAL);`
* Set the screen contrast: `LCD_CONTRAST(contrast);`
  * A minimum contrast of 0 sets the screen to pure white.
  * A contrast of 32 is the default.
  * A maximum contrast of 63 sets the screen to pure black.
* Turn on RMW mode: `LCD_RMW(ON);` Default is `LCD_RMW(OFF);`
* Reset cursor and all effects to default: `LCD_RESET();`

There are more commands implemented from the closest datasheet we've found, but not all are tested on PM, and some don't work at all like LCD_ADC.

## Frame buffer direct draw

This part of the library draws directly to the PRC frame buffer. In order to use it effectively, it's best to set `PRC_MODE = COPY_ENABLE;` While it is possible to use it with other parts of the PRC enabled, it will require careful interrupt usage beyond the scope of this readme.

Similar to the LCD, we regard the frame buffer as having 8 pages which are each 8 pixels high and contain 96 columns each. The library maintains a cursor into the frame buffer which can be access via FB_CURSOR but should not be set directly (increments and decrements are okay, just keep in mind there is no internal bounds checking).

The commands you'll typically use are:

* Seek to a specific position: `fb_seek(column, page);`
* Write one column: `FB_DATA = x;`
  * The cursor does not automatically increment.
* Write tile-like data from ROM: `fb_write(data addr, len);`
* Blit sprite-like data from ROM: `fb_blit(data addr, mask addr, len);`
  * For dealing with data not aligned to page boundaries, you'll have to shift it yourself in RAM and blit from there.

Other commands:

* Quickly seek to start: `FB_CURSOR = FRAMEBUFFER;`
* Read one column: `x = FB_DATA;`
  * The cursor does not automatically increment.
* Read multiple columns: `fb_read(len, &out);`
