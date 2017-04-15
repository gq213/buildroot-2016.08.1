################################################################################
#
# goahead
#
################################################################################

GOAHEAD_VERSION = 3.6.3
GOAHEAD_SOURCE = goahead-$(GOAHEAD_VERSION).tar.gz
GOAHEAD_SITE = localhost
GOAHEAD_DEPENDENCIES = json-c

define GOAHEAD_BUILD_CMDS
	$(MAKE) SHOW=1 ARCH=arm \
		ME_COM_MBEDTLS=0 \
		ME_COM_SSL=0 \
		CC="$(TARGET_CROSS)gcc" \
		-C $(@D)
endef

define GOAHEAD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(@D)/build/linux-arm-default/bin/goahead \
		$(TARGET_DIR)/usr/bin/goahead
	$(INSTALL) -D -m 755 $(@D)/build/linux-arm-default/bin/libgo.so \
		$(TARGET_DIR)/usr/lib/libgo.so
	$(INSTALL) -D -m 755 package/goahead/S90goahead \
		$(TARGET_DIR)/etc/init.d/S90goahead
	$(INSTALL) -D -m 755 package/goahead/conf/auth.txt \
		$(TARGET_DIR)/etc/goahead/auth.txt
	$(INSTALL) -D -m 755 package/goahead/conf/route.txt \
		$(TARGET_DIR)/etc/goahead/route.txt
	$(INSTALL) -D -m 755 package/goahead/web/favicon.ico \
		$(TARGET_DIR)/web/favicon.ico
	$(INSTALL) -D -m 755 package/goahead/web/index.html \
		$(TARGET_DIR)/web/index.html
endef

$(eval $(generic-package))
