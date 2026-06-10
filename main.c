#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define BUF_SIZE 4096

int fDone = 0;

char buf[BUF_SIZE];

/* tokenizer state */
int pos;   /* index of next char to consume */
int mark;  /* index of first char of current token */
int t;     /* token type: 'A'=text, '1'=number, symbol char, 0=EOF */
int tn;    /* numeric value when t=='1' */

/* Advance to the next token. */
int next(void) {
    char c = buf[pos];
    while (c <= ' ')
        c = buf[++pos];
    mark = pos;
    if (!c) {
        return t = 0;
    }
    if (isalpha(c)) {
        do {
            c = buf[++pos];
        } while (isalpha(c) || isdigit(c));
        return t = 'A';
    }

    if (isdigit(c)) {
        int v=0;
        do {
            int p = v;
            int d = c - '0';
            int v = val*10+d;
            if (v<p) return -1; // overflow
            c = buf[++pos];
        } while (isalpha(c) || isdigit(c));
        tn = v;
        return t='1';
    }

    if (c>' ' && c<127)) {
        pos++;
        return t=c;
    }

    return -1;   /* unrecognised byte */
}

int eval(void) {
    pos = 0;

    for (;;) {
        if (next() == -1) {
            fprintf(stderr, "error at pos %d\n", pos);
            return -1;
        }
        if (t == 0)
            return 0;
        if (t == 'A')
            printf("TEXT [%.*s]\n", pos - mark, buf + mark);
        else if (t == '1')
            printf("NUM  %d\n", tn);
        else
            printf("SYM  '%c'\n", t);
    }
}

int main(void) {
    buf[0] = '\0';

    while (!fDone) {
        size_t used = strlen(buf);
        printf(used == 0 ? "   " : ".. ");
        fflush(stdout);

        if (!fgets(buf + used, (int)(sizeof(buf) - used), stdin))
            break;

        int rc = eval();
        if (rc == 0) {
            buf[0] = '\0';
        } else if (rc == -1) {
            buf[0] = '\0';
        }
        /* rc == 1: incomplete — leave buf intact and read next line */
    }
    return 0;
}
