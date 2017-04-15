################################################################################
#
# rt3070
#
################################################################################

RT3070_SOURCE = DPO_RT5572_LinuxSTA_2.6.1.3_20121022.tar.bz2
RT3070_SITE = localhost

define RT3070_BUILD_CMDS
	$(MAKE) LINUX_SRC="/home/work/210-dev/final/kernel/linux-3.10.105" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
	-C $(@D)
endef

define RT3070_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/rt3070sta.ko \
		$(TARGET_DIR)/lib/modules/rt3070sta.ko
	$(INSTALL) -D -m 0644 $(@D)/RT2870STA.dat \
		$(TARGET_DIR)/etc/Wireless/RT2870STA/RT2870STA.dat
endef

$(eval $(generic-package))
