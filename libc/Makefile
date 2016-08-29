CC		:= gcc
CFLAGS		:= -nostdinc -m32 -std=c11 -Wall -Wextra -c -I ../include \
		-fno-builtin -fno-stack-protector 	

AR 		:= ar
ARFLAGS		:= -rcs

.PHONY: all
all: libc.a

%.o: %.c
	$(CC) $< -o $@ $(CFLAGS)

libc.a: ctype/ctype.o
	$(AR) $(ARFLAGS) $@ $^
