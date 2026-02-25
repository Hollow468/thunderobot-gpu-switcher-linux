// SPDX-License-Identifier: GPL-2.0
#include <linux/acpi.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>

#define GWMI_PATH   "\\_SB.GWMI"
#define WSAA_METHOD "WSAA"

static struct kobject *kobj;
static acpi_handle h_gwmi;

static void build_req(u8 req[32], u32 mode)
{
	int i;

	/* 32 bytes, all zero */
	for (i = 0; i < 32; i++)
		req[i] = 0;

	/* a0=0xFB00, a1=0x0203 (little-endian u16/u16) */
	req[0] = 0x00; req[1] = 0xFB;
	req[2] = 0x03; req[3] = 0x02;

	/* a2=mode (little-endian u32) */
	req[4] = (u8)(mode & 0xFF);
	req[5] = (u8)((mode >> 8) & 0xFF);
	req[6] = (u8)((mode >> 16) & 0xFF);
	req[7] = (u8)((mode >> 24) & 0xFF);
}

static int wsaa_set_mode(u32 mode)
{
	acpi_status st;
	union acpi_object argv[2];
	struct acpi_object_list args;
	u8 req[32];

	build_req(req, mode);

	argv[0].type = ACPI_TYPE_INTEGER;
	argv[0].integer.value = 0; /* Arg0 */

	argv[1].type = ACPI_TYPE_BUFFER;
	argv[1].buffer.length = sizeof(req);
	argv[1].buffer.pointer = req; /* Arg1 */

	args.count = 2;
	args.pointer = argv;

	st = acpi_evaluate_object(h_gwmi, WSAA_METHOD, &args, NULL);
	return ACPI_FAILURE(st) ? -EIO : 0;
}

static ssize_t mode_store(struct kobject *k, struct kobj_attribute *a,
			  const char *buf, size_t count)
{
	unsigned long mode;
	int ret;

	ret = kstrtoul(buf, 0, &mode);
	if (ret)
		return ret;

	if (mode < 1 || mode > 3)
		return -EINVAL;

	ret = wsaa_set_mode((u32)mode);
	if (ret)
		return ret;

	return count;
}

static struct kobj_attribute mode_attr = __ATTR(mode, 0220, NULL, mode_store);

static int __init lsgpu_init(void)
{
	acpi_status st;
	int ret;

	st = acpi_get_handle(NULL, GWMI_PATH, &h_gwmi);
	if (ACPI_FAILURE(st))
		return -ENODEV;

	/* sysfs directory: /sys/kernel/lsgpu */
	kobj = kobject_create_and_add("lsgpu", kernel_kobj);
	if (!kobj)
		return -ENOMEM;

	/* sysfs file: /sys/kernel/lsgpu/mode */
	ret = sysfs_create_file(kobj, &mode_attr.attr);
	if (ret) {
		kobject_put(kobj);
		kobj = NULL;
		return ret;
	}

	return 0;
}

static void __exit lsgpu_exit(void)
{
	if (kobj) {
		sysfs_remove_file(kobj, &mode_attr.attr);
		kobject_put(kobj);
		kobj = NULL;
	}
}

module_init(lsgpu_init);
module_exit(lsgpu_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Implementation of Thunderbolt Gpu Switching on Linux");
