
##############################################################
#MIDI-READER
# #############################################################

#AESD_CHARDEV_VERSION = '013a194'
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
MIDI_READER_SITE_METHOD = local
MIDI_READER_SITE = ../midi-reader
define MIDI_READER_BUILD_CMDS
        $(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/ all
		$(INSTALL) -D -m 755 $(@D)/S99digital_analog_synth-start-stop $(TARGET_DIR)/etc/init.d/S99digital_analog_synth-start-stop	
		$(INSTALL) -D -m 755 $(@D)/digital_analog_synth $(TARGET_DIR)/usr/bin/digital_analog_synth
endef
$(eval $(generic-package))

