CC		:= gcc
CFLAGS	:= -std=c11 -m32 
CFLAGS  += -Wall -Wextra -pedantic
CFLAGS  += -Os -S 
CFLAGS  += -I ../../include
CFLAGS  += -nodefaultlibs -nostdlib -nostdinc -fno-builtin -fno-ident

%.o: %.c
	$(CC) $< -o $@ -c -r $(CFLAGS)
