#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x4114c0ec, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0x6cdd27b2, "device_destroy" },
	{ 0x50745143, "intercepted_addr_buffer_capacity" },
	{ 0xcb8a6846, "intercepted_addr_buffer_idx" },
	{ 0x72b88987, "buffer_is_ready" },
	{ 0xdc2ec05e, "class_destroy" },
	{ 0x35589975, "kmem_cache_alloc_trace" },
	{ 0x420150eb, "kmalloc_caches" },
	{ 0x993af295, "device_create" },
	{ 0xd024243e, "cdev_del" },
	{ 0x9668dfba, "__class_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x4ffef938, "cdev_add" },
	{ 0x247f267a, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x2390aeec, "remap_pfn_range" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x5deb2711, "intercepted_addr_buffer" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D6CA2659F55206FBDD091ED");
