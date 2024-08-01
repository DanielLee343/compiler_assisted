#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/io.h>

#define DEVICE_NAME "chardev"
#define CLASS_NAME "charclass"
#define BUFFER_SIZE PAGE_SIZE

MODULE_LICENSE("GPL");

static int majorNumber;
static char *deviceBuffer;
static struct class *charClass = NULL;
static struct device *charDevice = NULL;

// Function prototypes for file operations
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int dev_mmap(struct file *, struct vm_area_struct *);

// File operations structure
static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
    .mmap = dev_mmap,
};

// Module initialization
static int __init chardev_init(void)
{
    // Allocate a major number dynamically
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "CharDev failed to register a major number\n");
        return majorNumber;
    }

    // Register device class
    charClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(charClass))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(charClass);
    }

    // Create device node
    charDevice = device_create(charClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(charDevice))
    {
        class_destroy(charClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(charDevice);
    }

    // Allocate memory for the buffer
    deviceBuffer = (char *)get_zeroed_page(GFP_KERNEL);
    if (!deviceBuffer)
    {
        device_destroy(charClass, MKDEV(majorNumber, 0));
        class_destroy(charClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to allocate memory\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "CharDev: device class created correctly, major number: %d\n", majorNumber);
    return 0;
}

// Module cleanup
static void __exit chardev_exit(void)
{
    free_page((unsigned long)deviceBuffer);
    device_destroy(charClass, MKDEV(majorNumber, 0));
    class_destroy(charClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "CharDev: Goodbye from the LKM!\n");
}

// File operations implementations
static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "CharDev: Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    error_count = copy_to_user(buffer, deviceBuffer, len);

    if (error_count == 0)
    {
        printk(KERN_INFO "CharDev: Sent %d characters to the user\n", len);
        return (len = 0);
    }
    else
    {
        printk(KERN_ALERT "CharDev: Failed to send %d characters to the user\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    sprintf(deviceBuffer, "%.*s", (int)len, buffer);
    printk(KERN_INFO "CharDev: Received %zu characters from the user\n", len);
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "CharDev: Device successfully closed\n");
    return 0;
}

static int dev_mmap(struct file *filep, struct vm_area_struct *vma)
{
    return remap_pfn_range(vma, vma->vm_start, virt_to_phys((void *)deviceBuffer) >> PAGE_SHIFT,
                           vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

module_init(chardev_init);
module_exit(chardev_exit);