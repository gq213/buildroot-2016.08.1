################################################################################
#
# myapp
#
################################################################################

MYAPP_VERSION = 1.0
MYAPP_SOURCE = myapp-$(MYAPP_VERSION).tar.gz
MYAPP_SITE = localhost
MYAPP_DEPENDENCIES = dbus json-c

define MYAPP_BUILD_CMDS
	(cp package/myapp/src/* $(@D) -a; \
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" PKG="$(PKG_CONFIG_HOST_BINARY)" -C $(@D))
endef

define MYAPP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/agent \
		$(TARGET_DIR)/usr/bin/agent
	$(INSTALL) -D -m 0755 $(@D)/radio \
		$(TARGET_DIR)/usr/bin/radio
endef

ifeq ($(BR2_PACKAGE_MYAPP),y)
define INSTALL_MY_FILES
	$(INSTALL) -D -m 0600 package/myapp/conf/root \
		$(TARGET_DIR)/etc/cron/crontabs/root
	$(INSTALL) -D -m 0644 package/myapp/conf/radio.list \
		$(TARGET_DIR)/etc/radio.list
	$(INSTALL) -D -m 0644 package/myapp/firmwares/rt3070/rt2870.bin \
		$(TARGET_DIR)/lib/firmware/rt2870.bin
	$(INSTALL) -D -m 0755 package/myapp/scripts/wifi-status \
		$(TARGET_DIR)/etc/wifi-status
	$(INSTALL) -D -m 0755 package/myapp/scripts/S40wifi \
		$(TARGET_DIR)/etc/wifi
endef
TARGET_FINALIZE_HOOKS += INSTALL_MY_FILES
endif

$(eval $(generic-package))
