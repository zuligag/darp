#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define BUF_SIZE 4096

int fDone = 0;

char buf[BUF_SIZE];

/* tokenizer state */
int  pos;   /* index of next char to consume */
int  mark;  /* index of first char of current token */
char t;     /* token type: 'A'=text, '1'=number, symbol char, 0=EOF */
int  tn;    /* numeric value when t=='1' */

/* Advance to the next token.
   Returns 0 on success, -1 on error.
   Sets t=0 at EOF. */
int next(void) {
    while (buf[pos] == ' ' || buf[pos] == '\t' ||
           buf[pos] == '\n' || buf[pos] == '\r')
        pos++;

    mark = pos;

    char c = buf[pos];

    if (c == '\0') {
        t = 0;
        return 0;
    }

    if (isalpha((unsigned char)c)) {
        while (isalpha((unsigned char)buf[pos]) ||
               isdigit((unsigned char)buf[pos]) ||
               buf[pos] == '_')
            pos++;
        while (buf[pos - 1] == '_')  /* retreat past trailing underscores */
            pos--;
        t = 'A';
        return 0;
    }

    if (isdigit((unsigned char)c)) {
        long val = 0;
        while (isdigit((unsigned char)buf[pos])) {
            int d = buf[pos++] - '0';
            if (val > (INT_MAX - d) / 10)
                return -1;          /* overflow */
            val = val * 10 + d;
        }
        tn = (int)val;
        t = '1';
        return 0;
    }

    if (isprint((unsigned char)c)) {
        t = c;
        pos++;
        return 0;
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
