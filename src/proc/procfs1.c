/*
 * procfs1.c - create a "file" in /proc
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define procfs_name "helloworld"

/*
 * This structure holds information about the /proc file
 */
struct proc_dir_entry *Our_Proc_File;

/*
 * Put data into the proc fs file
 *
 * Arguments
 * =========
 * 1. The buffer where the data is to be inserted, if you decide to use it.
 * 2. A pointer to a pointer to characters. This is useful if you don't want
 *    to use the buffer allocated by the kernel.
 * 3. The current position in the file.
 * 4. The size of the buffer in the first argument.
 * 5. Write a '1' here to indicate EOF.
 * 6. A pointer to data (useful in case one common read for multiple /proc/...
 *    entries).
 *
 * Usage and Return Value
 * ======================
 * A return value of zero means you have no further information at this time
 * (end of file). A negative return value is an error condition.
 *
 * For more information
 * ====================
 * check out the get_info field of proc_dir
 */
int procfile_read(char *buffer,
                  char **buffer_location,
                  off_t offset, int buffer_length, int *eof, void *data)
{
    int ret;

    printk(KERN_INFO "procfile_read (/proc/%s) called\n", procfs_name);

    // We give all our info in one go, so if the user asks for more, the answer
    // should always be no.

    if (offset > 0) {
        ret = 0;
    } else {
        ret = sprintf(buffer, "Hello World!\n");
    }

    return ret;
}

int init_module()
{
    Our_Proc_File = create_proc_entry(procfs_name, 0644, NULL);

    if (Our_Proc_File == NULL) {
        remove_proc_entry(procfs_name, &proc_root);
        printk(KERN_ALERT "ERROR: Could not initialize /proc/%s\n",
                procfs_name);
        return -ENOMEM;
    }

    Our_Proc_File->read_proc = procfile_read;
    Our_Proc_File->owner     = THIS_MODULE;
    Our_Proc_File->mode      = S_IFREG | S_IRUGO;
    Our_Proc_File->uid       = 0;
    Our_Proc_File->gid       = 0;
    Our_Proc_File->size      = 37;

    printk(KERN_INFO "/proc/%s created\n", procfs_name);
    return 0; // Everything is okay
}

void cleanup_module()
{
    remove_proc_entry(procfs_name, &proc_root);
    printk(KERN_INFO "/proc/%s removed\n", procfs_name);
}
