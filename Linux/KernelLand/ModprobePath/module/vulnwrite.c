#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Vulnerable arbitrary-write primitive for the modprobe_path demo");

struct vuln_write_req {
	unsigned long addr;
	char data[256];
};

#define VULN_WRITE _IOW('v', 1, struct vuln_write_req)

static long vulnwrite_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct vuln_write_req req;

	if (cmd != VULN_WRITE)
		return -ENOTTY;

	if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
		return -EFAULT;

	/* No validation on req.addr: arbitrary kernel write. */
	memcpy((void *)req.addr, req.data, sizeof(req.data));

	return 0;
}

static const struct file_operations vulnwrite_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = vulnwrite_ioctl,
};

static struct miscdevice vulnwrite_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = "vulnwrite",
	.fops  = &vulnwrite_fops,
	.mode  = 0666,
};

static int __init vulnwrite_init(void)
{
	return misc_register(&vulnwrite_dev);
}

static void __exit vulnwrite_exit(void)
{
	misc_deregister(&vulnwrite_dev);
}

module_init(vulnwrite_init);
module_exit(vulnwrite_exit);
