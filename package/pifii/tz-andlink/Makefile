include $(TOPDIR)/rules.mk

PKG_NAME:=tz-andlink
PKG_RELEASE:=1.14

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/tz-andlink
  SECTION:=pifii
  CATEGORY:=PiFii
  TITLE:=AndLink for SZTozed's Router and AP
  DEPENDS:=+lua +luci-lib-json
  
endef

define Package/tz-andlink/description
  AndLink for SZTozed's Gateway 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/tz-andlink/conffiles
/etc/config/andlink
endef

define Package/tz-andlink/install
	$(INSTALL_DIR)  $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/tozed/coap-client $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/tozed/coap-broadcast $(1)/usr/bin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/tozed/coap-ping $(1)/usr/bin
	$(INSTALL_DIR)  $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/tozed/coap-server $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/coap-resources-process $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlink-report $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlink-sysupgrade $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlink-macfilter $(1)/usr/sbin
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlink-qlink $(1)/usr/sbin
	$(INSTALL_DIR)  $(1)/usr/lib/lua
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlinkcommon.lua $(1)/usr/lib/lua
	$(INSTALL_DIR)  $(1)/etc/init.d
	$(INSTALL_BIN)  $(PKG_BUILD_DIR)/mt7628/andlink.init $(1)/etc/init.d/andlink
	$(INSTALL_DIR)  $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/mt7628/andlink.conf $(1)/etc/config/andlink
endef

$(eval $(call BuildPackage,tz-andlink))

