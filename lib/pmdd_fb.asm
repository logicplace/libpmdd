; Pokemon mini Direct Drawing library, frame buffer functions
; Copyright 2022 Sapphire Becker (logicplace.com)
; MIT Licensed
;
; This portion is for functions which draw to the PRC's framebuffer.
; Thus, the PRC must be enabled for this to work properly.

	NAME    "PMDD_FB"

	DEFSECT ".nbss", DATA, SHORT, CLEAR
	SECT    ".nbss"
	GLOBAL  _FB_CURSOR
_FB_CURSOR: DS 1

	DEFSECT ".text", CODE
	SECT    ".text"
	GLOBAL  _fb_seek
_fb_seek:
	; void fb_seek(
	; 	register("a") uint8_t col,
	; 	register("l") uint8_t page
	; ) {
	; 	FB_CURSOR = FRAMEBUFFER + page * 96 + col;
	; }
	PUSH h
	EX   a,b  ; Fasted way to push ba but pop a first
	PUSH ba
	LD   a,#96
	MLT
	POP  a
	LD   b,#010h  ; Framebuffer base is 0x1000
	ADD  hl,ba
	LD   ep,#@dpag(_FB_CURSOR)
	LD   [_FB_CURSOR],hl
	POP  b
	POP  h
	RET


	GLOBAL _fb_write
_fb_write:
	; void fb_write(
	; 	register("iy") uint8_t _far *data,
	; 	register("a") uint8_t len
	; ) {
	; 	// Overwrite mem in PRC's frame buffer at cursor
	; 	for (; len; --len, ++FB_CURSOR, ++data) {
	; 		*FB_CURSOR = *data;
	; 	}
	; }
	AND  a,a
	JR   Z,FBW_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD   ep,#@dpag(_FB_CURSOR)
	LD   hl,[_FB_CURSOR]

FBW_loop:
	LD   [hl],[iy]
	INC  hl
	INC  iy
	DJR  NZ,FBW_loop

	LD   [_FB_CURSOR],hl
	POP  hl
	POP  b
FBW_ret:
	RET

	GLOBAL _fb_blit
_fb_blit:
	; void fb_blit(
	; 	register("iy") uint8_t *data,
	; 	register("ix") uint8_t *mask,
	; 	register("a") uint8_t len
	; ) {
	; 	// Read mem and blit with mask+data, following PRC logic
	; 	// i.e. data bit (0 = white, 1 = black), mask (0 = opaque, 1 = transparent)
	; 	for (; len; --len, ++FB_CURSOR, ++data, ++mask) {
	; 		*FB_CURSOR |= (*data & ~*mask);
	; 	}
	; }
	AND  a,a
	JR   Z,FBB_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD   ep,#@dpag(_FB_CURSOR)
	LD   hl,[_FB_CURSOR]

FBB_loop:
	LD   a,[ix]  ; read and complement mask
	CPL  a
	AND  a,[iy]  ; & with data
	OR   [hl],a  ; blit to screen
	INC  hl
	INC  iy
	INC  ix
	DJR  NZ,FBB_loop

	LD   [_FB_CURSOR],hl
	POP  hl
	POP  b
FBB_ret:
	RET


	GLOBAL _fb_read
_fb_read:
	; void fb_read(
	; 	uint8_t len,
	; 	uint8_t *out
	; ) {
	; 	for (; len; --len, ++FB_CURSOR, ++out) {
	; 		*out = *FB_CURSOR;
	; 	}
	; }
	AND  a,a
	JR   Z,FBR_ret

	PUSH b
	PUSH hl
	LD   b,a
	LD	 yp,#00h  ; has to be in RAM
	LD   ep,#@dpag(_FB_CURSOR)
	LD   hl,[_FB_CURSOR]

FBR_loop:
	LD   a,[hl]  ; dummy read
	LD   [iy],[hl]
	INC  hl
	INC  iy
	DJR  NZ,FBR_loop

	POP  hl
	POP  b

FBR_ret:
	RET
