inherit core-image
CORE_IMAGE_EXTRA_INSTALL += "remote-led"
CORE_IMAGE_EXTRA_INSTALL += "rgb-led"
CORE_IMAGE_EXTRA_INSTALL += "kernel-modules"
EXTRA_IMAGE_FEATURES += "ssh-server-dropbear"

# avoid use of rpi-sdimg
# it causes devtool to barf; rpi imager can interpret a *.gz file just fine
IMAGE_FSTYPES += "wic.gz"
