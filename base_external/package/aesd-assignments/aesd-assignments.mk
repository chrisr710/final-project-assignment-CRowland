
##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents
AESD_ASSIGNMENTS_VERSION = '2f6a633'
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
AESD_ASSIGNMENTS_SITE = 'git@github.com:cu-ecen-aeld/assignments-3-and-later-chrisr710.git'
AESD_ASSIGNMENTS_SITE_METHOD = git
AESD_ASSIGNMENTS_GIT_SUBMODULES = YES

define AESD_ASSIGNMENTS_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/server all
endef

# TODO add your writer, finder and finder-test utilities/scripts to the installation steps below
define AESD_ASSIGNMENTS_INSTALL_TARGET_CMDS
	echo "TARGET DIR IS ""$(TARGET_DIR)"
	mkdir $(TARGET_DIR)/etc/init.d || echo OK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        chmod 755 $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 0755 $(@D)/server/aesdsocket $(TARGET_DIR)/usr/bin/
	$(INSTALL) -m 0755 $(@D)/server/aesdsocket-start-stop $(TARGET_DIR)/etc/init.d/S99aesdsocket
	#mkdir ./etc/init.d || echo OK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	#chmod 755 ./etc/init.d
	#mkdir ./etc/init.d/rc6.d
	#ln -s ./etc/init.d/aesdsocket-start-stop ./etc/init.d/rc5.d/S99aesdsocket
	#ln -s ./etc/init.d/aesdsocket-start-stop ./etc/init.d/rc0.d/K01aesdsocket
	#ln -s ./etc/init.d/aesdsocket-start-stop ./etc/init.d/rc6.d/K01aesdsocket
endef

$(eval $(generic-package))
