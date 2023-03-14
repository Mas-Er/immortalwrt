# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2020 Tobias Maedel

# FIT will be loaded at 0x02080000. Leave 16M for that, align it to 2M and load the kernel after it.
KERNEL_LOADADDR := 0x03200000

define Device/hk1-mini
  DEVICE_VENDOR := Rockchip
  DEVICE_MODEL := rk3229_box
  SOC := rk3229
  UBOOT_DEVICE_NAME := rk3229-hk1-mini
  IMAGE/sysupgrade.img.gz := boot-common | boot-script hk1-mini | pine64-img | gzip | append-metadata
  DEVICE_PACKAGES := kmod-usb-net-rtl8152
endef
TARGET_DEVICES += hk1-mini

