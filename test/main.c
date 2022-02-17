#include "pm.h"
#include "pmdd.h"

int main(void)
{
    // TODO: test code
    PRC_MODE = 0;

    LCD_COL(0);
    LCD_PAGE(0);
    LCD_DATA = 0x0f;
    LCD_DATA = 0x00;
    LCD_DATA = 0xf0;
    LCD_DATA = 0x00;
    LCD_DATA = 0x0a;
    LCD_DATA = 0x00;
    LCD_DATA = 0xa0;
    for (;;) {}
}