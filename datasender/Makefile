include $(TOPDIR)/rules.mk

PKG_NAME:=datasender
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/datasender
	CATEGORY:=Base system
	TITLE:=datasender
	DEPENDS:=+libwattson +libubus +libblobmsg-json
endef

define Package/datasender/description
	This send data to IBM cloud
endef

define Package/datasender/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/rc.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/datasender $(1)/usr/bin
	$(INSTALL_BIN) ./files/datasender.init $(1)/etc/init.d/datasender
	$(INSTALL_CONF) ./files/datasender.config $(1)/etc/config/datasender
	ln -s /etc/init.d/datasender $(1)/etc/rc.d/S99datasender
endef

$(eval $(call BuildPackage,datasender))
