inherit core-image sdcard_image-rpi
CORE_IMAGE_EXTRA_INSTALL += "remote-led"
CORE_IMAGE_EXTRA_INSTALL += "rgb-led"
CORE_IMAGE_EXTRA_INSTALL += "kernel-modules"
IMAGE_FSTYPES:append = " rpi-sdimg"
