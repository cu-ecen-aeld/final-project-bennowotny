#!/bin/bash
# Do not inherit the environment, it's configured for development and we want to configure to build

script_dir="$(dirname "$(realpath "$0")")"
yocto_prj_root="$script_dir/../.." # path to the project root from the 'modify' workspace
pushd "$yocto_prj_root" || exit 1
source "poky/oe-init-build-env"
bitbake -c clean rgb-led
"$script_dir/../../build.sh"
pushd "$script_dir" || exit 1 # go to the 'modify' workspace
make_command="$(grep -Pho -m1 '(?<=NOTE: )make .*$' $script_dir/../../build/tmp-glibc/work/raspberrypi3_64-oe-linux/rgb-led/1.0/temp/log.do_compile)"
make_command="$(echo "$make_command" | head -n1)"
# quote the CC arguments
make_command="${make_command/aarch64-oe-linux-gcc//workspaces/final-project-bennowotny/build/tmp-glibc/work/x86_64-linux/gcc-cross-aarch64/13.3.0/sysroot-destdir/workspaces/final-project-bennowotny/build/tmp-glibc/work/x86_64-linux/gcc-cross-aarch64/13.3.0/recipe-sysroot-native/usr/bin/aarch64-oe-linux/aarch64-oe-linux-gcc}"
make_command="${make_command/CC/\"CC}"
make_command="${make_command/ LD/\" LD}"
echo "$make_command"
sh -c "bear -- $make_command"
popd || exit 1
popd || exit 1
