#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("El barto");
MODULE_DESCRIPTION("El mejor modulo");

static int __init hello_init(void)
{
    printk(KERN_INFO "El mejor modulo se cargado!\n");
    return 0;
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "El mejor modulo se descargo!\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
