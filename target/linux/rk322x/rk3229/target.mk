#
# Copyright (C) 2015-2019 OpenWrt.org
#

SUBTARGET:=rk3229
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
BOARDNAME:=RK322X boards (32 bit)

define Target/Description
	Build firmware image for RK322X devices.
	This firmware features a 32 bit kernel.
endef
