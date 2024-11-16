/******************************************************************************
 *
 *   Copyright (C) 2011  Intel Corporation. All rights reserved.
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *****************************************************************************/

#include <linux/gpio.h>
#include <linux/module.h>

#define GPIO_R (514) // Header J8-3
#define GPIO_G (515) // Header J8-5
#define GPIO_B (516) // Header J8-7

static int __init rgb_led_init(void) {
  pr_info("Hello World!\n");

  if (gpio_request(GPIO_R, "RGB_R") < 0) {
    pr_err("ERROR: RGB R (%d) request\n", GPIO_R);
    goto dealloc_gpio_r;
  }
  gpio_direction_output(GPIO_R, 1);

  if (gpio_request(GPIO_G, "RGB_G") < 0) {
    pr_err("ERROR: RGB G (%d) request\n", GPIO_G);
    goto dealloc_gpio_g;
  }
  gpio_direction_output(GPIO_G, 1);

  if (gpio_request(GPIO_B, "RGB_B") < 0) {
    pr_err("ERROR: RGB B (%d) request\n", GPIO_B);
    goto dealloc_gpio_b;
  }
  gpio_direction_output(GPIO_B, 1);

  return 0;

dealloc_gpio_b:
  gpio_free(GPIO_G);
dealloc_gpio_g:
  gpio_free(GPIO_R);
dealloc_gpio_r:
  return -1;
}

static void __exit rgb_led_exit(void) {
  pr_info("Goodbye Cruel World!\n");

  gpio_set_value(GPIO_R, 0);
  gpio_set_value(GPIO_G, 0);
  gpio_set_value(GPIO_B, 0);

  gpio_free(GPIO_R);
  gpio_free(GPIO_G);
  gpio_free(GPIO_B);
}

module_init(rgb_led_init);
module_exit(rgb_led_exit);
MODULE_LICENSE("GPL");