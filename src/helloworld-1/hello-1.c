/*
 * hello-1.c - The simplest kernel module.
 */

#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */

int init_module(void)
{
    printk(KERN_INFO "Hello, world 1.\n");

    /* Remember, a non-zero return means init_module failed,
     * and that the module cannot be loaded
     */

    return 0;

}

void cleanup_module(void)
{
    printk(KERN_INFO "Goodbye, world 1.\n");
}
