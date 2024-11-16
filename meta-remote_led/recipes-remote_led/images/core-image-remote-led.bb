inherit core-image
inherit sdcard_image-rpi
CORE_IMAGE_EXTRA_INSTALL += "remote-led"
CORE_IMAGE_EXTRA_INSTALL += "rgb-led"
CORE_IMAGE_EXTRA_INSTALL += "kernel-modules"
EXTRA_IMAGE_FEATURES += "ssh-server-dropbear"

IMAGE_FSTYPES += " rpi-sdimg"
