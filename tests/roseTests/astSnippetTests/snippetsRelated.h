/* This is the stuff related to snippets, to be included by the target when the injection process is told not to copy related
 * global declarations, #includes, and imports. */
#ifndef snippetRelated_H
#define snippetRelated_H

#define COPY_RELATED_THINGS_no 0
#define COPY_RELATED_THINGS_yes 1

#if COPY_RELATED_THINGS == COPY_RELATED_THINGS_no

/* From snippets2.c */
int snippet_usage_counter = 0;

/* From snippets3.c */
#include <stdio.h>

/* From snippets4.c */
#include <stdlib.h>
#include <string.h>

struct Struct1 {
    int int_member;
    char char_member;
    const char *const_string_member;
    char* string_member;
    double double_member;
};

/* From snippets5.c */
void randomOffByOne(int addend1);
void addWithError(int addend1, int addend2, int result);
void copy_string10(char *dst, const char *src);
void allocate_string(const char *s);

/* From snippets6.c */
#include <stdlib.h>
#include <string.h>


#endif
#endif
