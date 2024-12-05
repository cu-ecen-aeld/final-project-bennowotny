#include "linux/printk.h"
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/uaccess.h>

// HW PWM INTERFACE

#define GPIO_R (514) // Header J8-3
#define GPIO_G (515) // Header J8-5
#define GPIO_B (516) // Header J8-7

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} color_t;

static color_t pingColor = {.red = 0, .green = 0, .blue = 0};
static color_t pongColor = {.red = 0, .green = 0, .blue = 0};
#define PING (1)
#define PONG (0)
// synchronizes the color used with the interrupt
static atomic_t colorBufferInUse = ATOMIC_INIT(PING);
// synchronized between setting the color (not interrupt safe)
DEFINE_MUTEX(changeColorMutex);

static int update_color(color_t newColor) {
  const int bufferUsed = atomic_read(&colorBufferInUse);
  if (bufferUsed != PING && bufferUsed != PONG)
    return EINVAL;
  if (mutex_lock_interruptible(&changeColorMutex) != 0)
    return EINTR;

  if (bufferUsed == PING) {
    // Update pong and swap
    pongColor = newColor;
    atomic_set(&colorBufferInUse, PONG);
  } else {
    // Update ping and swap
    pingColor = newColor;
    atomic_set(&colorBufferInUse, PING);
  }
  mutex_unlock(&changeColorMutex);
  return 0;
}

// load time parameter for the period of the PWM cycle
const long PERIOD_NS = 80000; // roughly 48Hz

struct hrtimer pwmCycler;
ktime_t expTime;

enum hrtimer_restart on_timer(struct hrtimer *timer) {
  static uint8_t cycleCount = 0;

  hrtimer_forward_now(timer, expTime);
  const int bufferUsed = atomic_read(&colorBufferInUse);
  color_t colorValue = bufferUsed == PING ? pingColor : pongColor;

  if (cycleCount == 0) {
    if (colorValue.red != 0)
      gpio_set_value(GPIO_R, 1);
    if (colorValue.green != 0)
      gpio_set_value(GPIO_G, 1);
    if (colorValue.blue != 0)
      gpio_set_value(GPIO_B, 1);
  } else {
    if (cycleCount == colorValue.red)
      gpio_set_value(GPIO_R, 0);
    if (cycleCount == colorValue.green)
      gpio_set_value(GPIO_G, 0);
    if (cycleCount == colorValue.blue)
      gpio_set_value(GPIO_B, 0);
  }

  ++cycleCount;
  return HRTIMER_RESTART;
}

// CHARDEV INTERFACE

static ssize_t color_write(struct file *file, const char __user *data, size_t data_size, loff_t *offset) {
  // assuming global device, the file does not contain useful information
  (void)file;
  // position data is not used
  (void)offset;

  if (!access_ok(data, data_size)) {
    return -EINVAL;
  }

  if (data_size != 3) {
    // Prevent ambiguous interpretations
    return -EINVAL;
  }

  char localData[3];
  if (copy_from_user(localData, data, data_size) != 0) {
    return -EINVAL;
  }

  color_t newColor = {.red = localData[0], .green = localData[1], .blue = localData[2]};
  const int updateColorResult = update_color(newColor);
  if (updateColorResult != 0)
    return -updateColorResult;

  // report success
  file->f_pos += data_size;
  return data_size;
}

static struct cdev led_cdev;
static int led_cdev_major;

// write-only device
static struct file_operations const fileOps = {
    .owner = THIS_MODULE,
    .write = color_write,
};

static int create_chrdev(void) {
  int err;
  int devno = MKDEV(led_cdev_major, 0);

  cdev_init(&led_cdev, &fileOps);
  led_cdev.owner = THIS_MODULE;
  led_cdev.ops = &fileOps;
  err = cdev_add(&led_cdev, devno, 1);
  return err;
}

static int __init
rgb_led_init(void) {

  if (gpio_request(GPIO_R, "RGB_R") < 0) {
    pr_err("ERROR: RGB R (%d) request\n", GPIO_R);
    goto dealloc_gpio_r;
  }
  gpio_direction_output(GPIO_R, 0);

  if (gpio_request(GPIO_G, "RGB_G") < 0) {
    pr_err("ERROR: RGB G (%d) request\n", GPIO_G);
    goto dealloc_gpio_g;
  }
  gpio_direction_output(GPIO_G, 0);

  if (gpio_request(GPIO_B, "RGB_B") < 0) {
    pr_err("ERROR: RGB B (%d) request\n", GPIO_B);
    goto dealloc_gpio_b;
  }
  gpio_direction_output(GPIO_B, 0);

  hrtimer_init(&pwmCycler, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  pwmCycler.function = on_timer;
  expTime = ktime_set(0, PERIOD_NS);
  hrtimer_start(&pwmCycler, expTime, HRTIMER_MODE_REL);

  dev_t dev = 0;
  if (alloc_chrdev_region(&dev, 0, 1, "rgb-led") < 0) {
    pr_err("ERROR: Could not get a device number allocated");
    goto stop_timer;
  }

  led_cdev_major = MAJOR(dev);

  if (create_chrdev() != 0) {
    pr_err("ERROR: Could not create chardev interface");
    goto release_chrdev;
  }

  return 0;

release_chrdev:
  unregister_chrdev_region(dev, 1);
stop_timer:
  hrtimer_cancel(&pwmCycler);
dealloc_gpio_b:
  gpio_free(GPIO_G);
dealloc_gpio_g:
  gpio_free(GPIO_R);
dealloc_gpio_r:
  return -1;
}

static void __exit rgb_led_exit(void) {
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