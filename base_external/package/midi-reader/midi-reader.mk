
##############################################################
#MIDI-READER
# #############################################################

#AESD_CHARDEV_VERSION = '013a194'
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
MIDI_READER_SITE_METHOD = local
MIDI_READER_SITE = /school_data/final-project-assignment-CRowland/midi-reader
define MIDI_READER_BUILD_CMDS
        $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/ all
endef



$(eval $(generic-package))
#AESD_CHARDEV_GIT_SUBMODULES = YES
#AESD_CHARDEV_MODULE_SUBDIRS = aesd-char-driver/
#LDD_MODULE_SUBDIRS+= scull/

#$(eval $(kernel-module))
#$(eval $(generic-package))
