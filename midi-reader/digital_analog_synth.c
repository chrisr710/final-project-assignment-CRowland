#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <i2c/smbus.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>    
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include "timer_and_array_vars.h"
#include <stdio.h> // //printf()
#include <sys/types.h> // open()
#include <sys/stat.h> // open()
#include <fcntl.h> // open()
#include <sys/ioctl.h> // ioctl()
#include <errno.h> // errno
#include <string.h> // strerror()
#include <unistd.h> // close()
#include <linux/i2c-dev.h> // struct i2c_msg
#include <linux/i2c.h>
#include <stdbool.h>

int update_milliseconds=10;
int size_of_envelope_arrays=99;
int i2cbus_fd=0;
int midi_input_fd=0;
int vca_addr=0x61;
int vco_addr=0x60;
pthread_mutex_t i2cbus_lock;

uint16_t target_note_i2cval =0;
uint16_t current_note_i2cval =0;

uint16_t target_velocity_i2cval = 45000;
uint16_t current_velocity_i2cval = 45000;

uint16_t current_vca_envelope_position=0;
uint16_t current_vco_envelope_position=0;

uint16_t current_selected_vca_array=11;
uint16_t current_selected_vco_array=11;
uint16_t current_selected_vca_decay_array=12;
bool use_decay_array=false;

bool loop_vco=false;
bool loop_vca=false;

//opens the i2c bus, leave it open through the duration of the application.
int open_i2cbus(void){	
	int i2c_fd = open("/dev/i2c-2",O_RDWR);
	return(i2c_fd);

}

//opens the midi input device.
int open_midi_input(void){
	int midi_fd = open("/dev/midi1", O_RDONLY);
	return(midi_fd);
}	

void split_byte(uint8_t byte, uint8_t *high_nibble, uint8_t *low_nibble) {
    *high_nibble = (byte >> 4) & 0x0F; // Right shift 4 bits and mask to get the higher nibble
    *low_nibble = byte & 0x0F;         // Mask to get the lower nibble
}

char buffer[1];

enum Command_values {
    NOTE_ON = 9,
    NOTE_OFF = 8,
    UNKNOWN = 0
};

enum Command_values curr_command_value = UNKNOWN;

uint8_t velocity = 0;
uint8_t note_num =0;
uint8_t channel = 0;
bool velocity_set=false;
bool note_num_set=false;

void reset_cached_values(void){
	velocity_set=false;
	note_num_set=false;
	
	////printf("reset cached values\n");
}

bool is_command_byte(uint8_t in_byte){
	if ((in_byte >> 7) == 1){
		return(true);
	}
	return(false);
}


void write_vca_value(uint16_t value){
	if (value > 40000){value = 55000;}
	printf("writing vca value:%hu\n",value);
	//calculate the actual bits to be sent before locking and sending them
	uint8_t input_array[2];
	input_array[0]=(uint8_t)(value>>12);
	input_array[1]=(uint8_t)(value>>4);
	struct i2c_rdwr_ioctl_data vca_packets; //the big struct
	struct i2c_msg messages[1]; //messages go in here
	messages[0].addr=vca_addr;
	messages[0].flags=0;
	messages[0].len = sizeof(input_array);
	messages[0].buf  = input_array;
	vca_packets.msgs  = messages;
    vca_packets.nmsgs = 1;
	
	int result = ioctl(i2cbus_fd, I2C_RDWR, &vca_packets);
		if (result != 1){
			////printf("vca send result was : %d!!!\n",result);
		}
}


void write_vco_value(uint16_t value){
	//calculate the actual bits to be sent before locking and sending them
	//printf("writing vco value: %hu\n",value);
	uint8_t input_array[2];
	input_array[0]=(uint8_t)(value>>12);
	input_array[1]=(uint8_t)(value>>4);
	struct i2c_rdwr_ioctl_data vco_packets;
	struct i2c_msg vco_messages[1];
	vco_messages[0].addr  = vco_addr;
	vco_messages[0].flags = 0;
	vco_messages[0].len = sizeof(input_array);
	vco_messages[0].buf  = input_array;
	vco_packets.msgs  = vco_messages;
    vco_packets.nmsgs = 1;
	if(ioctl(i2cbus_fd, I2C_RDWR, &vco_packets) < 0) {
			//printf("error i2c-send\n");
		}
	
}

uint16_t get_note_i2c_val(uint8_t note_number){
	//printf("returning %hu i2c val for note number %d\n",note_array[note_number],note_number);
	return(note_array[note_number]);
}

uint16_t get_velocity_i2c_val(uint8_t velocity){
	return(velocity_array[velocity]);
}

void vca_envelope_runner(bool reset){
	//printf("vca envelope updater: target i2c val = %d, current i2cval = %d\n", target_velocity_i2cval, current_velocity_i2cval);
	////printf("vca current selected array = %d, 
	//already locked.
	if (reset == true){
		current_vca_envelope_position=0;
		
	}

	if (current_vca_envelope_position > size_of_envelope_arrays){
		if (loop_vca == false){
		return;} //stay there
		else{
		current_vca_envelope_position=0;
		}
	}
	if (use_decay_array == true){
		printf("using decay array\n");
		printf("decay array value at %d is %hu\n", current_vca_envelope_position, envelope_arrays[current_selected_vca_decay_array][current_vca_envelope_position]);
		if (current_velocity_i2cval != target_velocity_i2cval + (envelope_arrays[current_selected_vca_decay_array][current_vca_envelope_position])){
			
			write_vca_value(target_velocity_i2cval + (envelope_arrays[current_selected_vca_decay_array][current_vca_envelope_position]));
			current_velocity_i2cval=target_velocity_i2cval + (envelope_arrays[current_selected_vca_decay_array][current_vca_envelope_position]);
		}
	}
	else{
		    //does the current value = the target velocity + the offset?
		if (current_velocity_i2cval != target_velocity_i2cval + (envelope_arrays[current_selected_vca_array][current_vca_envelope_position])){
			////printf("current velocity setting %hu current_velocity_i2cval does not match the target %hu + the offset %hu\n", current_velocity_i2cval, target_velocity_i2cval, envelope_arrays[current_selected_vca_decay_array][current_vca_envelope_position]);
			write_vca_value(target_velocity_i2cval + (envelope_arrays[current_selected_vca_array][current_vca_envelope_position]));
			////printf("VCA value written is %hu + %hu\n", target_note_i2cval, envelope_arrays[current_selected_vca_array][current_vca_envelope_position]);
			current_velocity_i2cval=target_velocity_i2cval + (envelope_arrays[current_selected_vca_array][current_vca_envelope_position]);
		}
	}
		
	
	current_vca_envelope_position = current_vca_envelope_position +1;
		
}


void vco_envelope_runner(bool reset){
	//already locked.
	if (reset == true){
		current_vco_envelope_position=0;
	}
	//printf("(2) VCO envelope runner is operating under these settings: current_note_i2cval %hu, target_note_i2cval %hu, current_selected_vco_array %hu, current_vco_envelope_position %hu\n",current_note_i2cval,target_note_i2cval,current_selected_vco_array,current_vco_envelope_position);
	if (current_vco_envelope_position > size_of_envelope_arrays){
		if (loop_vco == false){
		return;}
		else{
		current_vco_envelope_position=0;
		}
	}
	
	if (current_note_i2cval != target_note_i2cval + (envelope_arrays[current_selected_vco_array][current_vco_envelope_position])){
		write_vco_value(target_note_i2cval + envelope_arrays[current_selected_vco_array][current_vco_envelope_position]);
		current_note_i2cval=target_note_i2cval + envelope_arrays[current_selected_vco_array][current_vco_envelope_position];
	}
	current_vco_envelope_position= current_vco_envelope_position +1;
		
}





void handle_note_on(uint8_t note_number, uint8_t note_velocity,uint8_t channel){
	//printf("(1)handling note on: notenum %d, velocity %d, channel %d\n",note_number,note_velocity,channel);
	//lock everything, envelope runner must complete
	pthread_mutex_lock(&i2cbus_lock);
	//set the target note
	target_note_i2cval = get_note_i2c_val(note_number);
	//set the target velocity
	target_velocity_i2cval = get_velocity_i2c_val(velocity);
	//set the envelopes to the attack mode
	use_decay_array=false;
	//send the signal to reset both envelopes (that will kick off the note)
	vco_envelope_runner(true);
	vca_envelope_runner(true);
	pthread_mutex_unlock(&i2cbus_lock);
	
}

void set_vco_envelope_array(void){
	current_selected_vco_array = current_selected_vco_array + 1;
	if (current_selected_vco_array > 14){
	current_selected_vco_array=0;}
	printf("current selected vco array is %d\n", current_selected_vco_array);
	
}

void set_vca_envelope_array(void){
	current_selected_vca_array = current_selected_vca_array + 1;
	if (current_selected_vca_array > 14) {
		current_selected_vca_array=0;
	}
	printf("current selected vca array is %d\n", current_selected_vca_array);
	
}

void handle_note_off(uint8_t note_number, uint8_t channel){
	//printf("handling note off: notenum %d, channel %d\n",note_number,channel);
	if (channel == 9){
		if (note_number == 36){
			set_vco_envelope_array();
		}
		if (note_number == 37){
			set_vca_envelope_array();
		}
	}
	int noteoff = get_note_i2c_val(note_number);
	//if it is not the current note number, ignore it
	if (noteoff != target_note_i2cval){
		printf("note number for wrong note, not shutting off\n");
		return;
	}
	//set the vca envelope to decay mode
	
	use_decay_array=true;
	//reset the vca envelope
	pthread_mutex_lock(&i2cbus_lock);
	vca_envelope_runner(true);
	pthread_mutex_unlock(&i2cbus_lock);
}

void envelope_updater(void){
	//printf("envelope updater ran. Current note val=%hu, target note val=%hu\n", current_note_i2cval,target_note_i2cval);
	pthread_mutex_lock(&i2cbus_lock);
	vca_envelope_runner(false);
	vco_envelope_runner(false);
	pthread_mutex_unlock(&i2cbus_lock);
}



int main(){
	//open midi port
	midi_input_fd = open_midi_input();
	if (midi_input_fd <=0 ){
		perror("COULD NOT OPEN MIDI PORT\n");
		return(1);
	}
	
	//open i2c bus
	i2cbus_fd = open_i2cbus();
	if (i2cbus_fd <=0 ){
		perror("COULD NOT OPEN I2CBUS\n");
		return(1);
	}
	
	void* timer_thread(void* arg) {
		struct timespec req, rem;
		req.tv_sec = 0;
		req.tv_nsec = update_milliseconds * 1000000;
		while (1) {
			if (nanosleep(&req, &rem) == -1) {
				if (errno == EINTR) { // Interrupted by signal, try again
				   req = rem;
				   continue;
				} else {
				  perror("nanosleep");
				  break;
				}
			}
	
			envelope_updater();
			
		}
		pthread_exit(NULL);
		}

    pthread_t timer;
	pthread_create(&timer, NULL, timer_thread, NULL);
	
	//read midi and react!
	
	
	
	while(1){
				int ret;
				ret = read(midi_input_fd, buffer, sizeof(buffer));
				uint8_t inbyte = buffer[0];
				uint8_t high, low;
				split_byte(inbyte, &high, &low);
				
				if (is_command_byte(buffer[0])){
					reset_cached_values();//this is a new note, we will get the values now
					channel=low;
					
					if (high == 0x8){
						curr_command_value = NOTE_OFF;
						continue;
						}
					if (high == 0x9){
						reset_cached_values();
						curr_command_value = NOTE_ON;
						//////printf("NOTE ON RECEIVED\n");
						continue;
						}
					
				}
				else{
					if (curr_command_value == NOTE_ON && (! note_num_set)){ //Note on received, didn't get note num yet
						note_num = buffer[0];
						note_num_set = true;
						continue;
					}
					if (curr_command_value == NOTE_ON && note_num_set && (! velocity_set)){
						velocity = buffer[0];
						velocity_set = true;
						//printf("aRUNNING NOTE ON: note#: %d, velocity:%d, channel:%d\n",note_num,velocity,channel);
						handle_note_on(note_num,velocity,channel);
						continue;	
					}
					if (curr_command_value == NOTE_OFF && (! note_num_set)){ //Note on received, didn't get note num yet
						note_num = buffer[0];
						printf("bRUNNING NOTE OFF WITH NOTE # %d, channel %d\n",note_num,channel);
						if (note_num != -1){
							handle_note_off(note_num, channel);
						}
						continue;
					}
					if (curr_command_value == NOTE_OFF && velocity== -1){
						velocity = buffer[0];
						velocity_set = true;
						//printf("RUNNING NOTE OFF: note #%d velocity:%d, channel %d\n",note_num,velocity,channel);
						if (note_num != -1){
							handle_note_off(note_num, channel);
						}
						continue;	
					}
					if (curr_command_value == NOTE_ON && velocity_set){
						//printf("UNHANDLED NOTE ON WITH note number = %d, velocity = %d, channel = %d\n",note_num,velocity,channel);
					}
						
				}
			} 

}








