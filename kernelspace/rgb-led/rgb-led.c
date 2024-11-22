/******************************************************************************
 *
 *   Copyright (C) 2011  Intel Corporation. All rights reserved.
 *
 *   SPDX-License-Identifier: GPL-2.0-only
 *
 *****************************************************************************/

#include <linux/hrtimer.h>
#include <linux/moduleparam.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ktime.h>

#define GPIO_R (514) // Header J8-3
#define GPIO_G (515) // Header J8-5
#define GPIO_B (516) // Header J8-7

int dutyCycle = 0;
module_param(dutyCycle, int, 0);

long ns = 0;
module_param(ns, long, 200);

struct hrtimer pwmCycler;
ktime_t expTime;

enum hrtimer_restart on_timer(struct hrtimer* timer){
  static uint8_t cycleCount = 0;

  hrtimer_forward_now(timer, expTime);
  
  if(cycleCount == 0 && dutyCycle != 0){
    gpio_set_value(GPIO_R, 1);
    gpio_set_value(GPIO_G, 1);
    gpio_set_value(GPIO_B, 1);
  }else if(cycleCount == dutyCycle){
    gpio_set_value(GPIO_R, 0);
    gpio_set_value(GPIO_G, 0);
    gpio_set_value(GPIO_B, 0);
  }
  
  ++cycleCount;
  return HRTIMER_RESTART;
}

static int __init rgb_led_init(void) {
  pr_info("Hello World!\n");
  pr_info("Using duty cycle: %d\n", dutyCycle);

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

  hrtimer_init(&pwmCycler, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  pwmCycler.function = on_timer;
  expTime = ktime_set(0, ns); // 162760, i.t.
  hrtimer_start(&pwmCycler, expTime, HRTIMER_MODE_REL);

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

  hrtimer_cancel(&pwmCycler);

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