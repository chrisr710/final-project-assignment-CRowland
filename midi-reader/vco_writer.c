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

void printBinary16(uint16_t num) {
    // Iterate from the most significant bit to the least significant bit
    // sizeof(int) * 8 gives the total number of bits in an int
    for (int i = (sizeof(uint16_t) * 8) - 1; i >= 0; i--) {
        // Use bitwise right shift (>>) to move the current bit to the LSB position
        // Use bitwise AND (&) with 1 to check if the LSB is 1 or 0
        printf("%d", (num >> i) & 1);
    }
    printf("\n"); // Print a newline after the binary representation
}

void printBinary(char num) {
    // Iterate from the most significant bit to the least significant bit
    // sizeof(int) * 8 gives the total number of bits in an int
    for (int i = (sizeof(char) * 8) - 1; i >= 0; i--) {
        // Use bitwise right shift (>>) to move the current bit to the LSB position
        // Use bitwise AND (&) with 1 to check if the LSB is 1 or 0
        printf("%d", (num >> i) & 1);
    }
    printf("\n"); // Print a newline after the binary representation
}

void get_bytes_to_send(uint8_t input_array[],uint16_t value){
		input_array[0]=(uint8_t)(value>>12);
		//printf("This should be an 8 bit number always starting with 0000 and ending in top 4 bits of value\n");
		input_array[1]=(uint8_t)(value>>4);
		//printf("this should be bits, from the left, 12-4\n");
}

int main(){
	uint8_t values_to_send[2];
	uint16_t input_value;
	printf("INPUT, UP TO 65535:\n");
	scanf("%hu",&input_value);
	get_bytes_to_send(values_to_send,input_value);
	printf("Input in binary:\n");
	printBinary16(input_value);
	printf("should start with 4 zeros, end in the 4 MSB of the input value\n");
	printBinary(values_to_send[0]);
	printf("should contain bits 12->4 of input value\n");
	printBinary(values_to_send[1]);
	//uint8_t reg=15; //0000+msb bits of value
	//uint8_t value=255; //8 lsb of value
	
	int i2c_fd;
	i2c_fd = open("/dev/i2c-2",O_RDWR);
	if (i2c_fd < 1){
		printf("error opening i2c bus\n");
		return(1);
	}
	int vco_addr=0x60;
	unsigned char outbuf[2];
    struct i2c_rdwr_ioctl_data packets; //the big struct
    struct i2c_msg messages[1]; //messages go in here
	messages[0].addr  = vco_addr;
    messages[0].flags = 0;
    messages[0].len   = sizeof(outbuf);
    messages[0].buf   = outbuf;

    outbuf[0] = values_to_send[0];
    outbuf[1] = values_to_send[1];

    packets.msgs  = messages;
    packets.nmsgs = 1;
    if(ioctl(i2c_fd, I2C_RDWR, &packets) < 0) {
        return 0;
    }

    return 1;
}
	
	
	
	
	
	