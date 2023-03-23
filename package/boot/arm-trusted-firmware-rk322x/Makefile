# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2021-2023 ImmortalWrt.org

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/trusted-firmware-a.mk

PKG_NAME:=rkbin
#PKG_VERSION:=linux-5.10-gen-rkr3.5
PKG_RELEASE:=1

#PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
#PKG_SOURCE_URL:=https://gitlab.com/rk3588_linux/rk/rkbin/-/archive/$(PKG_VERSION)/
#PKG_HASH:=012b0d7bdc269480f38ccc06d6f2e88fd794e05f2227184989aa9863279c90ca


# Version is: knaerzche/tag:libreelec-991d0e4
# https://github.com/ilmich/LibreELEC.tv/blob/3cb942b373ee8b0ad23854ff319d136e18b4c566/projects/Rockchip/devices/RK322x/packages/tools/rkbin/package.mk
PKG_VERSION="7b83eae7baf1166672f4cc34db397c2665fdcbc0"
PKG_SHA256="6ba07342d7d78c8be23f3b2745ee4d273449304b13ceb60a3df40a7ec4245dbc"
#PKG_ARCH="arm aarch64"
#PKG_LICENSE="nonfree"
#PKG_SITE="https://github.com/rockchip-linux/rkbin"
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL="https://github.com/ilmich/rkbin/archive/$PKG_VERSION.tar.gz"
#PKG_LONGDESC="rkbin: Rockchip Firmware and Tool Binaries"
#PKG_TOOLCHAIN="manual"

PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

include $(INCLUDE_DIR)/package.mk
include ./atf-version.mk

define Trusted-Firmware-A/Default
  NAME:=Rockchip $(1)
  BUILD_TARGET:=rk322x
endef

define Trusted-Firmware-A/rk3229
  BUILD_SUBTARGET:=rk3229
  ATF:=rk32/$(RK3229_ATF)
  DDR:=rk32/$(RK3229_DDR)
  LOADER:=rk32/$(RK3229_LOADER)
endef

TFA_TARGETS:= \
	rk3229

define Build/Compile
	# This comment is the workaround for "extraneous 'endif'" error
ifneq ($(filter rk32%,$(BUILD_VARIANT)),)
	( \
		pushd $(PKG_BUILD_DIR) ; \
		$(SED) 's,$$$$(PKG_BUILD_DIR),$(PKG_BUILD_DIR),g' trust.ini ; \
		$(SED) 's,$$$$(VARIANT),$(BUILD_VARIANT),g' trust.ini ; \
		./tools/mkimage -n $(BUILD_VARIANT) -T rksd -d bin/$(DDR) \
			$(BUILD_VARIANT)-idbloader.bin ; \
		cat bin/$(LOADER) >> $(BUILD_VARIANT)-idbloader.bin ; \
		./tools/trust_merger --replace bl31.elf bin/$(ATF) trust.ini ; \
		popd ; \
	)
endif
endef

define Package/trusted-firmware-a/install
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)

	$(CP) $(PKG_BUILD_DIR)/bin/$(ATF) $(STAGING_DIR_IMAGE)/
ifneq ($(filter rk32%,$(BUILD_VARIANT)),)
	$(CP) $(PKG_BUILD_DIR)/tools/loaderimage $(STAGING_DIR_IMAGE)/
	$(CP) $(PKG_BUILD_DIR)/$(BUILD_VARIANT)-idbloader.bin $(STAGING_DIR_IMAGE)/
	$(CP) $(PKG_BUILD_DIR)/$(BUILD_VARIANT)-trust.bin $(STAGING_DIR_IMAGE)/
else ifneq ($(filter rk35%,$(BUILD_VARIANT)),)
	$(CP) $(PKG_BUILD_DIR)/bin/$(DDR) $(STAGING_DIR_IMAGE)/
endif
endef

$(eval $(call BuildPackage/Trusted-Firmware-A))
