################################################################################
#
# dlna
#
################################################################################

DLNA_VERSION = 1.0
DLNA_SOURCE = dlna-$(DLNA_VERSION).tar.gz
DLNA_SITE = localhost

define DLNA_BUILD_CMDS
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" BUILD_DIR="$(@D)" -C $(@D)
endef

define DLNA_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/dms \
		$(TARGET_DIR)/usr/bin/dms
	$(INSTALL) -D -m 755 $(@D)/dmr \
		$(TARGET_DIR)/usr/bin/dmr
	$(INSTALL) -D -m 755 $(@D)/libdlna.so \
		$(TARGET_DIR)/usr/lib/libdlna.so
	$(INSTALL) -D -m 755 package/dlna/S98dlna \
		$(TARGET_DIR)/etc/init.d/S98dlna
	$(INSTALL) -d -m 755 $(TARGET_DIR)/etc/dlna
endef

$(eval $(generic-package))
