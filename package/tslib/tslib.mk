################################################################################
#
# tslib
#
################################################################################

TSLIB_VERSION = 1.1
TSLIB_SITE = https://github.com/kergoth/tslib/releases/download/$(TSLIB_VERSION)
TSLIB_LICENSE = GPL, LGPL
TSLIB_LICENSE_FILES = COPYING

TSLIB_AUTORECONF = YES
TSLIB_INSTALL_STAGING = YES
TSLIB_INSTALL_STAGING_OPTS = DESTDIR=$(STAGING_DIR) LDFLAGS=-L$(STAGING_DIR)/usr/lib install

ifeq ($(BR2_PACKAGE_TSLIB),y)
define SETTING_TSLIB_ENV
	$(INSTALL) -D -m 0755 package/tslib/S50tslib.sh \
		$(TARGET_DIR)/etc/init.d
endef
TARGET_FINALIZE_HOOKS += SETTING_TSLIB_ENV
endif

$(eval $(autotools-package))
