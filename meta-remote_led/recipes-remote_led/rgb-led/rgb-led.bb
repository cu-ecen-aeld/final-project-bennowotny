LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

inherit module

KERNELSPACE_SOURCE_PATH = "/workspaces/final-project-bennowotny/kernelspace"

FILESEXTRAPATHS:prepend := "${KERNELSPACE_SOURCE_PATH}:"
SRC_URI = "file://rgb-led/"
S = "${WORKDIR}/rgb-led"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.

RPROVIDES:${PN} += "kernel-module-rgb-led"