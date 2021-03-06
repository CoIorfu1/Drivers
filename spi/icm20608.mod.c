#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x8d42f378, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x708166b, __VMLINUX_SYMBOL_STR(driver_unregister) },
	{ 0x85e46faa, __VMLINUX_SYMBOL_STR(__spi_register_driver) },
	{ 0x48c65b31, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x8626158a, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xb97db1c8, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xba900668, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xe41cfbf, __VMLINUX_SYMBOL_STR(spi_setup) },
	{ 0x50465868, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x5212df02, __VMLINUX_SYMBOL_STR(of_find_node_opts_by_path) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x5df7145f, __VMLINUX_SYMBOL_STR(spi_write_then_read) },
	{ 0xdf24fbd1, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x49b1da6b, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x61e9e28d, __VMLINUX_SYMBOL_STR(spi_sync) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x5f754e5a, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x1bec770f, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x5363d73f, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xa0df7b5e, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xb2b96aed, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xaedf5f7c, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

