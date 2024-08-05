#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>

#define DEVICE_NAME "shmchar"
#define CLASS_NAME  "shm"

MODULE_LICENSE("GPL");

static int majorNumber;
static char *kernel_buffer;
static struct class*  shmcharClass  = NULL;
static struct device* shmcharDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static int dev_mmap(struct file *, struct vm_area_struct *);

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .mmap = dev_mmap,
};

static int __init shmchar_init(void) {
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ALERT "shmchar failed to register a major number\n");
        return majorNumber;
    }

    shmcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(shmcharClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(shmcharClass);
    }

    shmcharDevice = device_create(shmcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(shmcharDevice)) {
        class_destroy(shmcharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(shmcharDevice);
    }

    kernel_buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!kernel_buffer) {
        device_destroy(shmcharClass, MKDEV(majorNumber, 0));
        class_destroy(shmcharClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to allocate memory\n");
        return -ENOMEM;
    }
    memset(kernel_buffer, 0, PAGE_SIZE);

    printk(KERN_INFO "shmchar: device class created correctly\n");
    return 0;
}

static void __exit shmchar_exit(void) {
    kfree(kernel_buffer);
    device_destroy(shmcharClass, MKDEV(majorNumber, 0));
    class_destroy(shmcharClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "shmchar: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    return 0;
}

static int dev_mmap(struct file *filep, struct vm_area_struct *vma) {
    return remap_pfn_range(vma, vma->vm_start, virt_to_phys(kernel_buffer) >> PAGE_SHIFT,
                           vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

module_init(shmchar_init);
module_exit(shmchar_exit);