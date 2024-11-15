inherit core-image sdcard_image-rpi
CORE_IMAGE_EXTRA_INSTALL += "remote-led"
IMAGE_FSTYPES:append = " rpi-sdimg"
