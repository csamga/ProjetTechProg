#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void list_print(char *list[], short n, int base) {
#define CHUNK 256
    short i;
    char tmp[CHUNK];
    char *buf;
    int buf_len, written, tmp_written;

    buf = malloc(CHUNK * (sizeof *buf));
    buf_len = CHUNK;
    written = 0;

    for (i = 0; i < n; i++) {
        tmp_written = sprintf(tmp, "%hd: %s\n", base + i, list[i]);

        if (written + tmp_written >= buf_len) {
            buf = realloc(buf, (buf_len * 2) * (sizeof *buf));
        }

        strcpy(buf + written, tmp);
        written += tmp_written;
    }

    fputs(buf, stdout);
    free(buf);
}


