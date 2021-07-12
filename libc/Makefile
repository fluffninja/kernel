Target := libc.a
Dependencies := \
	ctype.o \
	stdlib.o

## C Compiler Options ##
CC := gcc
CFLAGS := -c
CFLAGS += -std=c11 -masm=intel
CFLAGS += -nostdinc -I ../include
CFLAGS += -m32
CFLAGS += -mno-red-zone
CFLAGS += -fno-builtin
CFLAGS += -fno-stack-protector
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fno-combine-stack-adjustments
CFLAGS += -Wall -Wextra

## Archiver Options ##
AR := ar
ARFLAGS := -rcs

## Recipes ##

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(Target): $(Dependencies)
	$(AR) $(ARFLAGS) $@ $^
