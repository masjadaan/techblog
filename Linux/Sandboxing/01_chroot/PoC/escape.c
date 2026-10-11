/**
 * gcc -o escape escape.c
 * sudo ./escape
 */
#include <stdio.h>
#include <unistd.h>

int main(void) {
    if (chroot("/tmp/jail") != 0) {   // root is now /tmp/jail, but cwd is unchanged
        perror("chroot");
        return 1;
    }

    FILE *f = fopen("../../../etc/passwd", "r");
    if (!f) {
        perror("fopen");
        return 1;
    }

    char line[128];
    fgets(line, sizeof(line), f);
    printf("Escaped the jail, read: %s", line);

    fclose(f);
    return 0;
}