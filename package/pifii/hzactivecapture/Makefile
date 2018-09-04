include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=hzactivecapture
PKG_RELEASE:=1.0.1

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/hzactivecapture
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=HuZhou audit program for FQa 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/hzactivecapture/install
	$(INSTALL_DIR) $(1)/activecap/output
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/activecapture $(1)/activecap/output/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/captureInit.conf $(1)/activecap/output/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hzactivecapture.init $(1)/etc/init.d/hzactivecapture
endef
$(eval $(call BuildPackage,hzactivecapture))
