# Defines
is_$(MAKE) := true
ifdef is_mk88
RM := $(PRODDIR)\..\rm.bat
SREC_CAT := $(PRODDIR)\..\srec_cat.bat
else
PRODDIR := ../../c88tools
SREC_CAT := srec_cat
endif

CCFLAGS := -I"$(PRODDIR)\..\include" -Ilib -v -d pokemini -Tc-v -Tlc"-e -M -f2"


# Phony steps
test: pmdd.min

lib: lib/libpmdd.a

.PHONY: test, lib


# Library build steps
lib/libpmdd.a: lib/pmdd_lcd.obj lib/pmdd_fb.obj
	ar88 -cr $@ $!


# Test ROM build steps
pmdd.min: pmdd.sre
	$(SREC_CAT) $! -o $@ -binary

pmdd.sre: test/isr.obj test/startup.obj test/main.obj lib/libpmdd.a
	$(CC) $(CCFLAGS) -o $@ $!

test/isr.obj: test/isr.c
	$(CC) -c -o $@ $(CCFLAGS) $!

test/startup.obj: test/startup.asm
	$(AS) -c -o $@ $(CCFLAGS) $!

test/main.obj: test/main.c lib/pmdd_lcd.h lib/pmdd_fb.h
	$(CC) -c -o $@ $(CCFLAGS) test/main.c


# Debugging build steps
.c.src:
	$(CC) $(CCFLAGS) -cs -Tc"-o $@" $<

clean:
	$(RM) lib/*.obj lib/*.src test/*.obj test/*.src
	$(RM) pmdd.sre pmdd.map pmdd.min lib/libpmdd.a
