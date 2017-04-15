################################################################################
#
# ffmpeg3
#
################################################################################

FFMPEG3_VERSION = 3.0
FFMPEG3_SOURCE = ffmpeg-$(FFMPEG3_VERSION).tar.gz
FFMPEG3_SITE = http://ffmpeg.org/releases
FFMPEG3_INSTALL_STAGING = YES
FFMPEG3_INSTALL_TARGET = NO
FFMPEG3_DEPENDENCIES = host-pkgconf

define FFMPEG3_CONFIGURE_CMDS
	(cd $(FFMPEG3_SRCDIR) && \
	$(TARGET_CONFIGURE_OPTS) \
	$(TARGET_CONFIGURE_ARGS) \
	./configure \
	\
	--extra-cflags="-fPIC" \
	--prefix="/usr" \
	\
	--enable-gpl \
	--enable-version3 \
	\
	--disable-shared \
	--enable-static \
	--enable-small \
	--disable-runtime-cpudetect \
	\
	--disable-programs \
	\
	--disable-doc \
	\
	--enable-cross-compile \
	--cross-prefix=$(TARGET_CROSS) \
	--sysroot=$(STAGING_DIR) \
	--host-cc="$(HOSTCC)" \
	--arch=$(BR2_ARCH) \
	--target-os="linux" \
	--pkg-config="$(PKG_CONFIG_HOST_BINARY)" \
	\
	--disable-asm \
	--disable-altivec \
	--disable-vsx \
	--disable-power8 \
	--disable-amd3dnow \
	--disable-amd3dnowext \
	--disable-mmx \
	--disable-mmxext \
	--disable-sse \
	--disable-sse2 \
	--disable-sse3 \
	--disable-ssse3 \
	--disable-sse4 \
	--disable-sse42 \
	--disable-avx \
	--disable-xop \
	--disable-fma3 \
	--disable-fma4 \
	--disable-avx2 \
	--disable-aesni \
	--disable-armv5te \
	--disable-armv6 \
	--disable-armv6t2 \
	--disable-vfp \
	--disable-neon \
	--disable-inline-asm \
	--disable-yasm \
	--disable-mipsdsp \
	--disable-mipsdspr2 \
	--disable-msa \
	--disable-mipsfpu \
	--disable-mmi \
	\
	--disable-debug \
	--disable-stripping \
	\
	--disable-lzma \
	--disable-zlib \
	\
	--disable-d3d11va \
	--disable-dxva2 \
	--disable-vaapi \
	--disable-vda \
	--disable-vdpau \
	\
	--disable-avdevice \
	--disable-postproc \
	--disable-avfilter \
	--enable-pthreads \
	--enable-lzo \
	\
	--disable-encoders \
	--disable-decoders \
	--enable-decoder=ape \
	--enable-decoder=flac \
	--enable-decoder=aac \
	--enable-decoder=mp3 \
	--enable-decoder=wmav1 \
	--enable-decoder=wmav2 \
	--enable-decoder=vorbis \
	--disable-hwaccels \
	--disable-muxers \
	--disable-demuxers \
	--enable-demuxer=ape \
	--enable-demuxer=flac \
	--enable-demuxer=aac \
	--enable-demuxer=ogg \
	--disable-parsers \
	--enable-parser=flac \
	--enable-parser=aac \
	--enable-parser=vorbis \
	--disable-bsfs \
	--disable-protocols \
	--disable-indevs \
	--disable-outdevs \
	--disable-devices \
	--disable-filters \
	--disable-iconv \
	)
endef

$(eval $(autotools-package))
