#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/io.h>

#define DEVICE_NAME "my_char_device"
#define DEVICE_SIZE 4096 // 4 KB device memory

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class = NULL;
extern char *intercepted_addr_buffer; // Memory to be mapped
extern int *intercepted_addr_buffer_idx;
extern unsigned long intercepted_addr_buffer_capacity;
extern bool *buffer_is_ready;

// EXPORT_SYMBOL(intercepted_addr_buffer);  // Export the device buffer for use in other parts of the kernel

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device %s opened\n", DEVICE_NAME);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device %s closed\n", DEVICE_NAME);
    return 0;
}

static int my_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned long length = vma->vm_end - vma->vm_start;

    // Ensure the mapping size does not exceed the device buffer size
    if (length > DEVICE_SIZE)
    {
        return -EINVAL;
    }

    // Remap the kernel memory to user space
    if (remap_pfn_range(vma, vma->vm_start,
                        virt_to_phys(intercepted_addr_buffer) >> PAGE_SHIFT,
                        length,
                        vma->vm_page_prot))
    {
        return -EAGAIN;
    }

    printk(KERN_INFO "Device %s mmap'd\n", DEVICE_NAME);
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .mmap = my_mmap,
};

static int __init my_init(void)
{
    int result;

    // Allocate a device number
    result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (result < 0)
    {
        printk(KERN_WARNING "Can't allocate major number\n");
        return result;
    }

    // Initialize the cdev structure and add it to the kernel
    cdev_init(&my_cdev, &fops);
    result = cdev_add(&my_cdev, dev_number, 1);
    if (result < 0)
    {
        printk(KERN_NOTICE "Error %d adding device\n", result);
        unregister_chrdev_region(dev_number, 1);
        return result;
    }

    // Create device class
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(my_class))
    {
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_WARNING "Failed to create device class\n");
        return PTR_ERR(my_class);
    }

    // Create device node
    device_create(my_class, NULL, dev_number, NULL, DEVICE_NAME);

    intercepted_addr_buffer = kmalloc(DEVICE_SIZE, GFP_KERNEL);
    if (!intercepted_addr_buffer)
    {
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        printk(KERN_WARNING "Failed to allocate device memory\n");
        return -ENOMEM;
    }
    memset(intercepted_addr_buffer, 0, DEVICE_SIZE);
    intercepted_addr_buffer_idx = (int *)((char *)intercepted_addr_buffer + DEVICE_SIZE - sizeof(int));
    *intercepted_addr_buffer_idx = 0;
    buffer_is_ready = (bool *)((char *)intercepted_addr_buffer + DEVICE_SIZE - sizeof(int) - sizeof(bool));
    *buffer_is_ready = false;
    intercepted_addr_buffer_capacity = DEVICE_SIZE;

    printk(KERN_INFO "Device %s initialized with major number %d\n", DEVICE_NAME, MAJOR(dev_number));

    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);

    // int i = 0;
    // unsigned long *address_array = (unsigned long *)intercepted_addr_buffer;
    // // printk(KERN_INFO "Kernel received: %s\n",
    // //        shared_buffer);
    // // printk(KERN_INFO "addr data:\n");
    // for (i = 0; i < 10; i++)
    // {
    //     printk(KERN_INFO "kernel get: 0x%lx\n", address_array[i]);
    // }
    kfree(intercepted_addr_buffer);
    intercepted_addr_buffer = NULL;

    printk(KERN_INFO "Device %s removed\n", DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple mmap character device driver");