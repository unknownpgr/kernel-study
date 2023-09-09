#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int __init m_init(void)
{
    printk("Module loaded\n");
    return 0;
}

static void __exit m_exit(void)
{
    printk("Module unloaded\n");
}

module_init(m_init);
module_exit(m_exit);
MODULE_LICENSE("GPL");