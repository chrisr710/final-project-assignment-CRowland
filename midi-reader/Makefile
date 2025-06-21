midi_reader: midi_reader.c
	$(CROSS_COMPILE)gcc midi_reader.c -o midi_reader
	$(CROSS_COMPILE)gcc vco_writer.c -o vco_writer
	$(CROSS_COMPILE)gcc vca_writer.c -o vca_writer

clean:
	rm -rf $(DEST_DIR)*.o
	rm $(DEST_DIR)midi_reader

install:
	cp ./midi_reader $(DEST_DIR) 
	cp ./vco_writer $(DEST_DIR)
	cp ./vca_writer $(DEST_DIR)
	chmod +x $(DEST_DIR)midi_reader

all:
	$(CC) $(CFLAGS) $^ midi_reader.c -o midi_reader $(INCLUDES) $(LDFLAGS) $(DEVFLAGS)
	$(CC) $(CFLAGS) $^ vco_writer.c -o vco_writer $(INCLUDES) $(LDFLAGS) $(DEVFLAGS)
	$(CC) $(CFLAGS) $^ vca_writer.c -o vca_writer $(INCLUDES) $(LDFLAGS) $(DEVFLAGS)
	$(CC) $(CFLAGS) $^ array_writer.c -o array_writer $(INCLUDES) $(LDFLAGS) $(DEVFLAGS)
