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

define INSTALL_MY_FILES
	$(INSTALL) -D -m 0600 package/myapp/conf/root \
		$(TARGET_DIR)/etc/cron/crontabs/root
	$(INSTALL) -D -m 0644 package/myapp/conf/radio.list \
		$(TARGET_DIR)/etc/radio.list
endef
TARGET_FINALIZE_HOOKS += INSTALL_MY_FILES

$(eval $(generic-package))
