TARGET = pmdd

C_SOURCES = test/isr.c test/main.c
ASM_SOURCES = test/startup.asm
OBJS = lib/libpmdd.a lib/pmdd.obj

CCFLAGS += -Ilib
LDFLAGS += -Llib -lpmdd

include ../../pm.mk

lib/libpmdd.a: lib/pmdd.obj
    ar88 -cr $@ $!
