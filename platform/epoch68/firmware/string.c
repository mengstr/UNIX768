#include "string.h"

/* Copy a fixed number of bytes from src to dst. */
void *memcpy(void *dst, const void *src, size_t count)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    size_t i;

    for (i = 0; i < count; i++) {
        d[i] = s[i];
    }

    return dst;
}

/* Fill a memory region with a repeated byte value. */
void *memset(void *dst, i32 value, size_t count)
{
    unsigned char *d = (unsigned char *)dst;
    size_t i;

    for (i = 0; i < count; i++) {
        d[i] = (unsigned char)value;
    }

    return dst;
}

/* Compare two byte ranges lexicographically. */
i32 memcmp(const void *left, const void *right, size_t count)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    size_t i;

    for (i = 0; i < count; i++) {
        if (a[i] != b[i]) {
            return (i32)a[i] - (i32)b[i];
        }
    }

    return 0;
}

/* Return the length of a NUL-terminated string. */
size_t strlen(const char *s)
{
    size_t n = 0;

    while (s[n] != 0) {
        n++;
    }

    return n;
}

/* Find the first occurrence of a character in a string. */
char *strchr(const char *s, i32 ch)
{
    char c = (char)ch;

    while (*s != 0) {
        if (*s == c) {
            return (char *)s;
        }
        s++;
    }

    if (c == 0) {
        return (char *)s;
    }

    return 0;
}

/* Copy a NUL-terminated string including its terminator. */
char *strcpy(char *dst, const char *src)
{
    char *out = dst;

    while ((*dst++ = *src++) != 0) {
    }

    return out;
}

/* Append one NUL-terminated string to another. */
char *strcat(char *dst, const char *src)
{
    char *out = dst;

    while (*dst != 0) {
        dst++;
    }
    while ((*dst++ = *src++) != 0) {
    }

    return out;
}

/* Parse an unsigned octal integer from a string prefix. */
i32 atoo(const char *s)
{
    i32 value = 0;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' ||
           *s == '\f' || *s == '\v') {
        s++;
    }

    while (*s >= '0' && *s <= '7') {
        value = (value << 3) + (*s - '0');
        s++;
    }

    return value;
}

/* Map ASCII uppercase letters to lowercase. */
static i32 lower_ascii(i32 ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch + ('a' - 'A');
    }

    return ch;
}

/* Compare two NUL-terminated strings case-sensitively. */
i32 strcmp(const char *left, const char *right)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;

    while (*a != 0 && *a == *b) {
        a++;
        b++;
    }

    return (i32)*a - (i32)*b;
}

/* Compare up to count bytes of two strings case-sensitively. */
i32 strncmp(const char *left, const char *right, size_t count)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    size_t i;

    for (i = 0; i < count; i++) {
        if (a[i] != b[i] || a[i] == 0 || b[i] == 0) {
            return (i32)a[i] - (i32)b[i];
        }
    }

    return 0;
}

/* Compare up to count bytes of two strings ignoring ASCII case. */
i32 strncasecmp(const char *left, const char *right, size_t count)
{
    const unsigned char *a = (const unsigned char *)left;
    const unsigned char *b = (const unsigned char *)right;
    size_t i;

    for (i = 0; i < count; i++) {
        i32 ca = lower_ascii(a[i]);
        i32 cb = lower_ascii(b[i]);

        if (ca != cb || a[i] == 0 || b[i] == 0) {
            return ca - cb;
        }
    }

    return 0;
}

/* Return the next space-delimited word from an in-place command line. */
char *next_word(char **line)
{
    char *p;
    char *word;

    p = *line;
    while (*p == ' ') {
        p++;
    }
    if (*p == '\0') {
        *line = p;
        return 0;
    }

    word = p;
    while (*p != '\0' && *p != ' ') {
        p++;
    }
    if (*p == ' ') {
        *p++ = '\0';
    }

    *line = p;
    return word;
}

/*
 * Split a command buffer into command, arg1, and arg2 pointers.
 *
 * arg2 is the trimmed remainder of the line after arg1, not just a single
 * third token.  That keeps simple 2-argument commands working as before,
 * while allowing commands such as:
 *
 *     mknod name c 0 2
 *
 * to carry the trailing "c 0 2" payload in arg2 without redesigning the
 * monitor command table.
 */
void split_cmdbuf(char *cmdbuf, char **cmd, char **arg1, char **arg2)
{
    char *p;

    p = cmdbuf;
    *cmd = next_word(&p);
    *arg1 = next_word(&p);
    while (*p == ' ') {
        p++;
    }
    if (*p == '\0') {
        *arg2 = 0;
    } else {
        *arg2 = p;
    }
}
