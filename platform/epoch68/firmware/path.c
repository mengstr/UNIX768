#include "path.h"
#include "string.h"

/* Append one path segment to the stored cwd string. */
i32 cwd_push(char *cwd, i32 cwdsz, const char *segment)
{
    i32 n;

    if (cwd[0] == '.' && cwd[1] == '\0') {
        if ((i32)strlen(segment) + 1 > cwdsz) {
            return -1;
        }
        strcpy(cwd, segment);
        return 0;
    }

    n = strlen(cwd);
    if (n + 1 + (i32)strlen(segment) + 1 > cwdsz) {
        return -1;
    }
    cwd[n++] = '/';
    strcpy(cwd + n, segment);
    return 0;
}

/* Remove the final path segment from the stored cwd string. */
void cwd_pop(char *cwd)
{
    char *p;
    char *slash;

    if (cwd[0] == '.' && cwd[1] == '\0') {
        return;
    }
    slash = 0;
    for (p = cwd; *p != '\0'; p++) {
        if (*p == '/') {
            slash = p;
        }
    }
    if (slash == 0) {
        cwd[0] = '.';
        cwd[1] = '\0';
        return;
    }
    *slash = '\0';
}

/* Normalize an input path against the current working directory string. */
i32 normalize_cwd(char *dst, i32 dstsz, const char *cwd, char *path)
{
    char work[128];
    char *p;
    char *seg;

    if (dst == 0 || dstsz <= 0 || path == 0 || *path == '\0') {
        return -1;
    }

    if ((i32)strlen(path) + 1 > (i32)sizeof(work)) {
        return -1;
    }
    strcpy(work, path);

    if (*path == '/') {
        strcpy(dst, ".");
    } else {
        if (cwd == 0 || *cwd == '\0') {
            strcpy(dst, ".");
        } else {
            if ((i32)strlen(cwd) + 1 > dstsz) {
                return -1;
            }
            strcpy(dst, cwd);
        }
    }

    p = work;
    while (*p == '/') {
        p++;
    }
    while (*p != '\0') {
        seg = p;
        while (*p != '\0' && *p != '/') {
            p++;
        }
        if (*p == '/') {
            *p++ = '\0';
            while (*p == '/') {
                p++;
            }
        }
        if (seg[0] == '\0' || (seg[0] == '.' && seg[1] == '\0')) {
            continue;
        }
        if (seg[0] == '.' && seg[1] == '.' && seg[2] == '\0') {
            cwd_pop(dst);
            continue;
        }
        if (cwd_push(dst, dstsz, seg) < 0) {
            return -1;
        }
    }

    return 0;
}
