#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 2000
#define TIMER_INTERVAL 10
#define REMOTE_IP_SIZE 100
#define TIME_SIZE 70
#include <time.h>
#include <sys/time.h>



#include <stdint.h>

void split_byte(uint8_t byte, uint8_t *high_nibble, uint8_t *low_nibble) {
    *high_nibble = (byte >> 4) & 0x0F; // Right shift 4 bits and mask to get the higher nibble
    *low_nibble = byte & 0x0F;         // Mask to get the lower nibble
}

enum Command_values {
    NOTE_ON = 9,
    NOTE_OFF = 8,
    UNKNOWN = 0
};

enum Command_values curr_command_value = UNKNOWN;
uint8_t velocity = 0;
uint8_t note_num =0;
bool velocity_set=false;
bool note_num_set=false;

void reset_cached_values(void){
	velocity_set=false;
	note_num_set=false;
	
	printf("reset cached values\n");
}

bool is_command_byte(uint8_t in_byte){
	if ((in_byte >> 7) == 1){
		return(true);
	}
	return(false);
}

void read_config_file(char * location){
		char buffer[100];
		int fd = open("/etc/midi_port", O_RDONLY);
		if (fd < 0){
			printf("CANNOT OPEN MIDI CONFIGURATION FILE: /etc/midi_port\n");
			exit(1);
		}
		int bytes_read=read(fd, buffer, sizeof(buffer) -2);
		int i =0;
		for (;i<bytes_read;i++){
			if (buffer[i] == '\n'){
					buffer[i]='\0';
			break;}
		}
		if (i == (bytes_read -1)){
			buffer[(i + 1)] = '0';
		}
		
		strcpy(location,buffer);
		return;	
}
int main(){
		printf("MIDI PARSER STARTING\n");
		char dev_port[100];
		read_config_file(dev_port);
		printf("USING: %s\n",dev_port);
		char buffer[1];
		int fd = open(dev_port, O_RDONLY);
		if (fd < 0){
			printf("error opening %s\n",dev_port);
			return(1);
		}
		
		
		
		while(1){
			int ret;
			ret = read(fd, buffer, sizeof(buffer));
			uint8_t inbyte = buffer[0];
			printf("##BYTE:%d\n",inbyte);
			uint8_t high, low;
			split_byte(inbyte, &high, &low);
			printf("Byte: 0x%X\n", inbyte);
			printf("   High Nibble: 0x%X\n", high);
			printf("   Low Nibble: 0x%X\n", low);
			
			if (is_command_byte(buffer[0])){
				printf("command byte received\n");
				reset_cached_values();//this is a new note, we will get the values now
				
				if (high == 0x8){
					curr_command_value = NOTE_OFF;
					continue;
					}
				if (high == 0x9){
					reset_cached_values();
					curr_command_value = NOTE_ON;
					printf("NOTE ON RECEIVED\n");
					continue;
					}
			}
			else{
				printf("PROCESSING DATA BYTE\n");
				printf("CURRENT COMMAND VALUE: %d\n",curr_command_value);
				printf("CURRENT note_num%d\n",note_num);
				printf("CURRENT VELOCITY:%d\n",velocity);
				if (curr_command_value == NOTE_ON && (! note_num_set)){ //Note on received, didn't get note num yet
					note_num = buffer[0];
					printf("SET NOTE NUMBER TO %d\n",note_num);
					note_num_set = true;
					continue;
				}
				if (curr_command_value == NOTE_ON && note_num_set && (! velocity_set)){
					velocity = buffer[0];
					velocity_set = true;
					printf("NOTE ON: note#: %d, velocity:%d\n",note_num,velocity);
					continue;	
				}
				if (curr_command_value == NOTE_OFF && (! note_num_set)){ //Note on received, didn't get note num yet
					note_num = buffer[0];
					note_num_set = true;
					printf("setting note number off with note # %d\n",note_num);
					continue;
				}
				if (curr_command_value == NOTE_OFF && velocity== -1){
					velocity = buffer[0];
					velocity_set = true;
					printf("NOTE OFF: note #%d velocity:%d\n",note_num,velocity);
					continue;	
				}
					
			}
		}
	}
	

