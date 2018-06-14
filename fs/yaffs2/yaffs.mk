################################################################################
#
# Build the yaffs2 root filesystem image
#
################################################################################

define ROOTFS_YAFFS2_CMD
	$(TOPDIR)/fs/yaffs2/bin/mkyaffs2image_for_TQ210 $(TARGET_DIR) $@
endef

$(eval $(call ROOTFS_TARGET,yaffs2))
