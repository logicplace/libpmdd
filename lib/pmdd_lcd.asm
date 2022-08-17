; Pokemon mini Direct Drawing library, frame buffer functions
; Copyright 2022 Sapphire Becker (logicplace.com)
; MIT Licensed
; 
; This portion is for functions which draw to the LCD directly.

	NAME "PMDD_LCD"

; TODO: macros
LCD_RMW_ON MACRO
	LD br,#020h
	LD [br:0feh],#0e0h
	ENDM

LCD_RMW_OFF MACRO
	LD br,#020h
	LD [br:0feh],#0eeh
	ENDM


	DEFSECT ".text", CODE
	SECT    ".text"
	GLOBAL  _LCD_COL
_LCD_COL:
	; uint8_t LCD_COL(register uint8_t a) {
	; 	LCD_CTRL = ((a) & 0x0f);
	; 	LCD_CTRL = 0x10 | (((a) & 0xf0) >> 4);
	; 	return LCD_DATA;
	; }
	PUSH b
	LD   b,a
	AND  b,#0fh
	LD   br,#020h
	LD   [br:0feh],b
	AND  a,#0fh
	SRL  a
	SRL  a
	SRL  a
	SRL  a
	OR   a,#10h
	LD   [br:0feh],a
	LD   a,[br:0ffh]
	POP  b
	RET


	GLOBAL	_lcd_read1
_lcd_read1:
	; uint8_t lcd_read1(void) {
	; 	LCD_DATA;
	; 	return LCD_DATA;
	; }
	LD br,#020h
	LD a,[br:0ffh]
	LD a,[br:0ffh]
	RET


	GLOBAL	_lcd_write
_lcd_write:
	; void lcd_write(
	; 	register("iy") uint8_t _far *data,
	; 	register("a") uint8_t len
	; ) {
	; 	// Overwrite mem in LCD RAM at cursor
	; 	for (; len; --len, ++data) {
	; 		LCD_DATA = *data;
	; 	}
	; }
	AND  a,a
	JR   Z,LCDW_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD   ep,#00h
	LD   hl,#020ffh  ; faster than br:0ffh

LCDW_loop:
	LD   [hl],[iy]
	INC  iy
	DJR  NZ,LCDW_loop

	POP  hl
	POP  b
LCDW_ret:
	RET


	GLOBAL	_lcd_blit
_lcd_blit:
	; void lcd_blit(
	; 	register("iy") uint8_t _far *data,
	; 	register("ix") uint8_t _far *mask,
	; 	register("a") uint8_t len
	; ) {
	; 	// Read mem and blit with mask+data, following PRC logic
	; 	// i.e. data bit (0 = white, 1 = black), mask (0 = opaque, 1 = transparent)
	; 	LCD_RMW(ON);
	; 	for (; len; --len, ++data, ++mask) {
	; 		LCD_DATA;
	; 		LCD_DATA |= (*data & ~*mask);
	; 	}
	; 	LCD_RMW(OFF);
	; }
	AND  a,a
	JR   Z,LCDB_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD   ep,#00h
	LD   hl,#020ffh
	LCD_RMW_ON

LCDB_loop:
	LD   a,[hl]  ; dummy read
	LD   a,[ix]  ; read and complement mask
	CPL  a
	AND  a,[iy]  ; & with data
	OR   [hl],a  ; blit to screen
	INC  iy
	INC  ix
	DJR  NZ,LCDB_loop

	LCD_RMW_OFF
	POP  hl
	POP  b
LCDB_ret:
	RET


	GLOBAL	_lcd_read
_lcd_read:
	; void lcd_read(
	; 	register("a") uint8_t len,
	; 	register("yi") uint8_t *out
	; ) {
	; 	for (; len; --len, ++out) {
	; 		LCD_DATA;
	; 		*out = LCD_DATA;
	; 	}
	; }
	AND  a,a
	JR   Z,LCDR_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD   ep,#00h
	LD	 yp,#00h  ; has to be in RAM
	LD   hl,#020ffh

LCDR_loop:
	LD   a,[hl]  ; dummy read
	LD   [iy],[hl]
	INC  iy
	DJR  NZ,LCDR_loop

	POP  hl
	POP  b

LCDR_ret:
	RET


	GLOBAL	_lcd_clear
_lcd_clear:
	; void lcd_clear(void) {
	; 	uint8_t c, p;

	; 	for (p = 0; p <= 8; ++p) {
	; 		LCD_PAGE(p);
	; 		LCD_COL(0);
	; 		for (c = 0; c <= 131; ++c) {
	; 			LCD_DATA = 0;
	; 		}
	; 	}
	; }
	PUSH ba
	PUSH l
	LD   br,#020h

	LD   l,#0b0h  ; LCD page command
LCDC_outer:
	LD	 [br:0feh],l
	LD   a,#0
	CAR  _LCD_COL
	LD   b,#132
LCDC_inner:
	LD   [br:0ffh],#0
	DJR  NZ,LCDC_inner

	INC  l
	CP   l,#0b9h ; run thru pages 0~8 inclusive
	JR   NZ,LCDC_outer

	POP  l
	POP  ba
	RET

	END
