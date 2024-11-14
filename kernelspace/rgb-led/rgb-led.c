/******************************************************************************
 *
 *   Copyright (C) 2011  Intel Corporation. All rights reserved.
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *****************************************************************************/

#include <linux/module.h>

static int __init rgb_led_init(void)
{
	pr_info("Hello World!\n");
	return 0;
}

static void __exit rgb_led_exit(void)
{
	pr_info("Goodbye Cruel World!\n");
}

module_init(rgb_led_init);
module_exit(rgb_led_exit);
MODULE_LICENSE("GPL");