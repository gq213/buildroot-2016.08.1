################################################################################
#
# mplayer
#
################################################################################

MPLAYER_VERSION = 1.3.0
MPLAYER_SOURCE = MPlayer-$(MPLAYER_VERSION).tar.xz
MPLAYER_SITE = http://www.mplayerhq.hu/MPlayer/releases
MPLAYER_DEPENDENCIES = host-pkgconf ffmpeg3
MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_ALSA_LIB),alsa-lib)
MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_PULSEAUDIO),pulseaudio)
MPLAYER_LICENSE = GPLv2
MPLAYER_LICENSE_FILES = LICENSE Copyright
MPLAYER_CFLAGS = $(TARGET_CFLAGS)
MPLAYER_LDFLAGS = $(TARGET_LDFLAGS)

# Adding $(STAGING_DIR)/usr/include in the header path is normally not
# needed. Except that mplayer's configure script has a completely
# brain-damaged way of looking for X11/Xlib.h (it parses extra-cflags
# for -I options).
MPLAYER_CFLAGS += -I$(STAGING_DIR)/usr/include

define MPLAYER_CONFIGURE_CMDS
	(cd $(@D); \
		$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_CONFIGURE_ARGS) \
		./configure \
		--prefix=/usr \
		--confdir=/etc \
		--target=$(GNU_TARGET_NAME) \
		--host-cc="$(HOSTCC)" \
		--cc="$(TARGET_CC)" \
		--as="$(TARGET_AS)" \
		--charset=UTF-8 \
		--extra-cflags="$(MPLAYER_CFLAGS)" \
		--extra-ldflags="$(MPLAYER_LDFLAGS)" \
		\
		--enable-cross-compile \
		\
		--disable-ffmpeg_a \
		\
		--disable-mencoder \
		--disable-tv \
		--disable-unrarexec \
		--disable-ftp \
		--disable-xanim \
		--disable-v4l2 \
		--disable-dvb \
		--disable-tga \
		--disable-pnm \
		--disable-md5sum \
		--disable-yuv4mpeg \
		\
		--disable-inet6 \
		--disable-iconv \
		\
		--disable-ossaudio \
		--enable-alsa \
		--enable-pulse \
	)
endef

define MPLAYER_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define MPLAYER_INSTALL_TARGET_CMDS
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(@D) install
	$(INSTALL) -D -m 755 package/mplayer/S39mplayer \
		$(TARGET_DIR)/etc/init.d/S39mplayer
endef

ifeq ($(BR2_PACKAGE_PULSEAUDIO),y)
define MPLAYER_OUTPUT_TO_PULSE
	sed -i '7s/alsa/pulse/g' $(TARGET_DIR)/etc/init.d/S39mplayer
endef
TARGET_FINALIZE_HOOKS += MPLAYER_OUTPUT_TO_PULSE
endif

$(eval $(generic-package))
