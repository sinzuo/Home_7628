include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=rzxInstallGramFQa
PKG_RELEASE:=V1.0.0.20_062001_Release_20170928

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rzxInstallGramFQa
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=RenZixing Audit InstallGam for FQa 
        DEPENDS:=+rzxAuditFQa +libstdcpp
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/rzxInstallGramFQa/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/Install_gram $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzxInstallGramFQa.init $(1)/etc/init.d/rzxInstallGramFQa
endef
$(eval $(call BuildPackage,rzxInstallGramFQa))
