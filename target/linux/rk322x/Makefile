# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2012-2020 OpenWrt.org
# Copyright (C) 2017 LEDE project

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=rk322x
BOARDNAME:=Rochchip RK322x
FEATURES:=ext4 audio usb usbgadget display gpio fpu pci pcie rootfs-part boot-part squashfs
SUBTARGETS:=rk3229

KERNEL_PATCHVER:=5.15

define Target/Description
	Build firmware image for Rochchip RK322x SoC devices.
	Currently produces SD Card image for RK322X BOX.
endef

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES := $(filter-out urngd,$(DEFAULT_PACKAGES))
DEFAULT_PACKAGES += uboot-envtools partx-utils e2fsprogs mkf2fs kmod-gpio-button-hotplug \
		    automount fdisk e2fsprogs

KERNELNAME:=Image dtbs

$(eval $(call BuildTarget))
