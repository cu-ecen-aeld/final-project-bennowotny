inherit update-rc.d

LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

# No information for SRC_URI yet (only an external source tree was specified)
SRC_URI:append = "file://remote-led"

INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME:${PN} = "remote-led"

FILES:${PN} += "${sysconfdir}/init.d/remote-led"

do_install:append(){
  install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/remote-led ${D}${sysconfdir}/init.d
}