#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2023.01
PKG_RELEASE:=1

PKG_HASH:=69423bad380f89a0916636e89e6dcbd2e4512d584308d922d1039d1e4331950f

PKG_MAINTAINER:=Tobias Maedel <openwrt@tbspace.de>

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include ../arm-trusted-firmware-rockchip/atf-version.mk

define U-Boot/Default
  BUILD_TARGET:=rockchip
  UENV:=default
  HIDDEN:=1
endef


# RK3229 boards

define U-Boot/Default/rk3229
  BUILD_SUBTARGET:=armv7
  DEPENDS:=+PACKAGE_u-boot-$(1):trusted-firmware-a-rk3229
  ATF:=$(RK3229_ATF)
endef

define U-Boot/hk1-mini-rk3229
  $(U-Boot/Default/rk3229)
  NAME:=HK1 Mini
  BUILD_DEVICES:= \
    hk1-mini
endef

UBOOT_TARGETS := \
  hk1-mini-rk3229 

UBOOT_CONFIGURE_VARS += USE_PRIVATE_LIBGCC=yes

UBOOT_MAKE_FLAGS += \
  BL31=$(STAGING_DIR_IMAGE)/$(ATF)

define Build/Configure
	$(call Build/Configure/U-Boot)

	$(SED) 's#CONFIG_MKIMAGE_DTC_PATH=.*#CONFIG_MKIMAGE_DTC_PATH="$(PKG_BUILD_DIR)/scripts/dtc/dtc"#g' $(PKG_BUILD_DIR)/.config

ifneq ($(DDR),)
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/tpl
	$(CP) $(STAGING_DIR_IMAGE)/$(DDR) $(PKG_BUILD_DIR)/tpl/u-boot-tpl.bin
endif
endef

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
ifneq ($(DDR),)
	$(CP) $(PKG_BUILD_DIR)/idbloader.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-idbloader.img
	$(CP) $(PKG_BUILD_DIR)/u-boot.itb $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-u-boot.itb
else
	$(STAGING_DIR_IMAGE)/loaderimage --pack --uboot $(PKG_BUILD_DIR)/u-boot-dtb.bin $(PKG_BUILD_DIR)/uboot.img 0x200000
	$(CP) $(PKG_BUILD_DIR)/uboot.img $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-uboot.img
endif
endef

define Package/u-boot/install/default
endef

$(eval $(call BuildPackage/U-Boot))
