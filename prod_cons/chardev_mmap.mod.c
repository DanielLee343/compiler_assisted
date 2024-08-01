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
	{ 0x4302d0eb, "free_pages" },
	{ 0x6cdd27b2, "device_destroy" },
	{ 0xf09b5d9a, "get_zeroed_page" },
	{ 0xdc2ec05e, "class_destroy" },
	{ 0x993af295, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x9668dfba, "__class_create" },
	{ 0x9b5856e1, "__register_chrdev" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x2390aeec, "remap_pfn_range" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x92997ed8, "_printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4D8E7BAB4306100DF0FC617");
