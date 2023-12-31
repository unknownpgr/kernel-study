#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>

typedef struct _custom_device_data
{
    struct cdev cdev;
} custom_device_data;

static char *custom_device_name = "custom_device";
static int dev_major = 0; // Will be automatically assigned later
static struct class *device_class = NULL;
#define DEVICE_COUNT 3
static custom_device_data data[DEVICE_COUNT];

static int custom_open(struct inode *inode, struct file *file)
{
    custom_device_data *data = container_of(inode->i_cdev, custom_device_data, cdev);
    file->private_data = data;
    return 0;
}

static int fill_buffer(char *output, char *buf, size_t len, loff_t *offset)
{
    int output_length = strlen(output);
    int length_to_user = output_length - (*offset);
    if (length_to_user > len)
        length_to_user = len;
    if (length_to_user < 0)
        return 0;
    char tmp_buf[100] = {0};
    for (int i = 0; i < length_to_user; i++)
        tmp_buf[i] = output[*offset + i];
    if (copy_to_user(buf, tmp_buf, length_to_user))
        return -EFAULT;
    *offset += length_to_user;
    return length_to_user;
}

static ssize_t custom_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    custom_device_data *custom_data;
    custom_data = (custom_device_data *)file->private_data;

    struct task_struct *tasks[100] = {NULL};
    int count = 0;
    struct task_struct *task = current;
    // Go up to the root of the process tree
    while (true)
    {
        tasks[count++] = task;
        if (task == task->parent)
            break;
        task = task->parent;
    }
    char output_buffer[500] = {0};
    char tmp_buffer[100] = {0};
    for (int i = count - 1; i >= 0; i--)
    {
        task = tasks[i];
        sprintf(tmp_buffer, "%d %s\n", task->pid, task->comm);
        strcat(output_buffer, tmp_buffer);
    }

    // Add some other information
    custom_device_data *device_data_addr = data;
    sprintf(tmp_buffer, "device_data_addr: %p\n", device_data_addr);
    strcat(output_buffer, tmp_buffer);

    int *local_variable_addr = &count;
    sprintf(tmp_buffer, "local_variable_addr: %p\n", local_variable_addr);
    strcat(output_buffer, tmp_buffer);

    return fill_buffer(output_buffer, buf, len, offset);
}

static int custom_release(struct inode *inode, struct file *file)
{
    return 0;
}

static int custom_dev(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0660);
    return 0;
}

struct file_operations custom_fops = {
    .owner = THIS_MODULE,
    .open = custom_open,
    .read = custom_read,
    .release = custom_release,
};

static int custom_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0440);
    return 0;
}

static char *create_device_name(int index)
{
    char buffer[100] = {0};
    sprintf(buffer, "%s%d", custom_device_name, index);
    int length = strlen(buffer);
    char *name = kmalloc(length + 1, GFP_KERNEL);
    strcpy(name, buffer);
    return name;
}

static int __init m_init(void)
{
    printk("Module loaded\n");

    /**
     * dev_t is just a 32bit number where 12 bits are used for the major number and 20 bits for the minor number.
     */
    dev_t dev;
    /**
     * Register a range of char device numbers. This will assign major number dynamically.
     * dev: output parameter for first assigned number
     * baseminor: first of the requested range of minor numbers
     * count: the number of minor numbers required
     * name: the name of the associated device or driver
     */
    int err = alloc_chrdev_region(&dev, 0, DEVICE_COUNT, custom_device_name);
    dev_major = MAJOR(dev);
    if (err != 0)
        return err;
    device_class = class_create(THIS_MODULE, custom_device_name);
    device_class->dev_uevent = custom_dev_uevent;

    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        /**
         * As its name suggests, cdev_init() is used to initialize a cdev structure.
         */
        cdev_init(&data[i].cdev, &custom_fops);
        data[i].cdev.owner = THIS_MODULE;
        /**
         * cdev_add() adds the device represented by the structure pointed to by dev to the system, making it live immediately.
         */
        cdev_add(&data[i].cdev, MKDEV(dev_major, i), 1);
        /**
         * device_create() creates a device and registers it with sysfs.
         * It means that it will be exposed in /dev directory.
         */
        char *name = create_device_name(i);
        device_create(device_class, NULL, MKDEV(dev_major, i), NULL, name);
    }

    return 0;
}

static void __exit m_exit(void)
{
    printk("Module unloaded\n");
    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        /**
         * device_destroy() removes a device that was created with device_create().
         */
        device_destroy(device_class, MKDEV(dev_major, i));
        /**
         * cdev_del() removes a cdev from the system.
         */
        cdev_del(&data[i].cdev);
    }
    unregister_chrdev_region(MKDEV(dev_major, 0), DEVICE_COUNT);
    class_destroy(device_class);
}

module_init(m_init);
module_exit(m_exit);
MODULE_LICENSE("GPL");