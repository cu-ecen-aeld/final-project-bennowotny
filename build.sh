#!/usr/bin/bash
# don't inherit the environment, set up from scratch

script_dir="$(realpath "$(dirname "$0")")"
machine="raspberrypi3-64"
distro="remote-led-distro"
architecture="aarch64"
image="core-image-remote-led"

pushd "$script_dir"
source poky/oe-init-build-env
bitbake-layers add-layer "$script_dir/meta-remote_led"
bitbake-layers layerindex-fetch meta-oe
bitbake-layers layerindex-fetch openembedded-core
bitbake-layers layerindex-fetch meta-raspberrypi
MACHINE="$machine" DISTRO="$distro" ARCH="$architecture" REMOTE_LED_IMAGE_TYPES="rpi-sdimg" bitbake "$image"
