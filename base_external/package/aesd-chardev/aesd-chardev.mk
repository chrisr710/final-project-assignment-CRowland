
##############################################################
#AESD_CHARDEv
# #############################################################

#TODO: Fill up the contents below in order to reference your assignment 3 git contents
AESD_CHARDEV_VERSION = '013a194'
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
AESD_CHARDEV_SITE = 'git@github.com:cu-ecen-aeld/assignments-3-and-later-chrisr710.git'
AESD_CHARDEV_SITE_METHOD = git
AESD_CHARDEV_GIT_SUBMODULES = YES
AESD_CHARDEV_MODULE_SUBDIRS = aesd-char-driver/
#LDD_MODULE_SUBDIRS+= scull/

$(eval $(kernel-module))
$(eval $(generic-package))
