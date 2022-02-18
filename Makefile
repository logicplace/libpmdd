TARGET = pmdd

C_SOURCES = test/isr.c test/main.c
ASM_SOURCES = test/startup.asm
OBJS = lib/libpmdd.a

CCFLAGS += -Ilib
LDFLAGS += -Llib -lpmdd

include ../../pm.mk

lib/libpmdd.a: lib/pmdd_fb.obj lib/pmdd_lcd.obj
    ar88 -cr $@ $!
