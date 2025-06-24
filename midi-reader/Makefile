digital_analog_synth: digital_analog_synth.c timer_and_array_vars.c
	$(CROSS_COMPILE)gcc digital_analog_synth.c -o digital_analog_synth
	
clean:
	rm -rf $(DEST_DIR)*.o
	rm $(DEST_DIR)digital_analog_synth

install:
	cp ./digital_anlog_synth $(DEST_DIR) 
	chmod +x $(DEST_DIR)digital_anlog_synth.c

all:
	$(CC) $(CFLAGS) $^ digital_analog_synth.c $^ timer_and_array_vars.c -o digital_analog_synth $(INCLUDES) $(LDFLAGS) $(DEVFLAGS)
