#include <ctype.h>

#include "kutil.h"
#include "kio.h"

static void __hexdump_word(unsigned char *ptr, size_t word_size)
{
    for (size_t i = 0; i < word_size; ++i) {
        kprintf(" %02x", ptr[i]);
    }
}

static void __hexdump_hex_row(int *ptr, int word_count)
{
    for (int i = 0; i < word_count; ++i) {
        kprintf(" ");
        __hexdump_word((unsigned char *) &(ptr[i]), sizeof(int));
    }
}

static void __hexdump_char_row(char *ptr, int char_count)
{
    for (int i = 0; i < char_count; ++i) {
        char c = ptr[i];
        if (!isgraph(c) && c != ' ') {
            c = '.';
        }
        kprintf("%c", c);
    }
}

void hexdump(int *ptr, int words_per_row, int rows)
{
    for (int i = 0; i < rows; ++i) {
        // Print out row start address
        kprintf(" %08x", ptr);

        // Print out row _words_ as hex
        __hexdump_hex_row(ptr, words_per_row);

        kprintf("  |");

        // Print out ASCII reinterpretation
        __hexdump_char_row((char *) ptr, (int) (words_per_row * sizeof(int)));

        kprintf("|");

        // Increment pointer past row
        ptr += words_per_row;

        kprintf("\n");
    }
}

void print_cpustat(const struct cpustat cs)
{
    kprintf(
        " eax=%08x ebx=%08x ecx=%08x edx=%08x\n"
        " esi=%08x edi=%08x ebp=%08x esp=%08x\n"
        " efl=%08x\n",
        cs.regset.a, cs.regset.b, cs.regset.c, cs.regset.d,
        cs.regset.si, cs.regset.di, cs.regset.bp, cs.regset.sp,
        cs.eflags
    );
}
