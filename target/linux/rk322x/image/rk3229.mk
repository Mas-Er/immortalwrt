# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

# FIT will be loaded at 0x02080000. Leave 16M for that, align it to 2M and load the kernel after it.
KERNEL_LOADADDR := 0x03200000

define Device/rk322x_hk1_mini
  DEVICE_VENDOR := Rockchip
  DEVICE_MODEL := Hk1 Mini
  SOC := rk3229
  UBOOT_DEVICE_NAME := rk322x_hk1_mini-rk3229
  IMAGE/sysupgrade.img.gz := boot-common | boot-script | pine64-bin | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += rk322x_hk1_mini
