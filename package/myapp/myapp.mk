################################################################################
#
# myapp
#
################################################################################

MYAPP_VERSION = 1.0
MYAPP_SOURCE = myapp-$(MYAPP_VERSION).tar.gz
MYAPP_SITE = localhost
MYAPP_DEPENDENCIES = dbus

define MYAPP_BUILD_CMDS
	(cp package/myapp/src/* $(@D) -a; \
	$(MAKE) CROSS_COMPILE="$(TARGET_CROSS)" PKG="$(PKG_CONFIG_HOST_BINARY)" -C $(@D))
endef

define MYAPP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/agent \
		$(TARGET_DIR)/usr/bin/agent
endef

$(eval $(generic-package))
