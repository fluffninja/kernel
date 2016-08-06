// ctype.h for Isaac OS

#ifndef _INC_CTYPE
#define _INC_CTYPE 1

#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int isblank(int c); // C99/C++11
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isupper(int c);
int isprint(int c);
int isspace(int c);
int isxdigit(int c); // Wreck our beautiful alignment, why don't you?

int tolower(int c);
int toupper(int c);

#ifdef __cplusplus
}
#endif

#endif /* _INC_CTYPE */
