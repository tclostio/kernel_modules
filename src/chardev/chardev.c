/*
 * chardev.c - Creates a read-only char device that says how many
 * times a user has read from the dev file.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>        /* for put_user */

/*
 * Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file*, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUFF_LEN 80             /* max length of the message */

/*
 * Global variables are declared as static, so are global within
 * the file.
 */
static int Major;
static int Device_Open = 0;

static char msg[BUFF_LEN];
static char *msg_Ptr;

static struct file_operations fops = {
    .read    = device_read,
    .write   = device_write,
    .open    = device_open,
    .release = device_release
};

/*
 * This function is called when the module is loaded.
 */
int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }

    printk(KERN_INFO "I was assigned major number %d to talk to\n", Major);
    printk(KERN_INFO "The driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;
}

/*
 * This function is called when the module is unloaded.
 */
void cleanup_module(void)
{
    // Unregister the device:
    int ret = unregister_chrdev(Major, DEVICE_NAME);
    if (ret < 0)
        printk(KERN_ALERT "Error in unregister_chrdev: %d\n", ret);
}

/*
 * .....::The methods::.....
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
    static int counter = 0;

    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    sprintf(msg, "I already said Hello World %d times!\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
    Device_Open--;

    // Decrement the usage count, or else you'll never close the module
    module_put(THIS_MODULE);

    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts
 * to read from it.
 */
static ssize_t device_read(struct file *filp,
                           char *buffer,
                           size_t length,
                           loff_t * offset)
{
    // Number of bytes actually written to the buffer
    int bytes_read = 0;

    // If we're at the end, return 0
    if (*msg_Ptr == 0)
        return 0;

    // Actually put the data into the buffer
    while (length && *msg_Ptr) {
        // Use put_user to copy data from kernel to user segment
        put_user(*(msg_Ptr++), buffer++);

        length--;
        bytes_read++;
    }

    // Most read functions return the number of bytes in the buffer
    return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
    return -EINVAL;
}
