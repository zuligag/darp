#include <stdio.h>
#include <string.h>

#define BUF_SIZE 4096

int fDone = 0;

int eval(const char *buf);

int main(void) {
    char buf[BUF_SIZE];
    buf[0] = '\0';

    while (!fDone) {
        size_t used = strlen(buf);
        printf(used == 0 ? "   " : ".. ");
        fflush(stdout);

        if (!fgets(buf + used, (int)(sizeof(buf) - used), stdin))
            break;

        int rc = eval(buf);
        if (rc == 0) {
            buf[0] = '\0';
        } else if (rc == -1) {
            fprintf(stderr, "error\n");
            buf[0] = '\0';
        }
        /* rc == 1: incomplete — leave buf as-is and read another line */
    }
    return 0;
}
