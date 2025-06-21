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

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

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

void append_file(uint16_t value){
	int fd = open("./note_array.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
	char data[10];
	sprintf(data,"%hu",value);
	write(fd,data,strlen(data));
	char comma = ',';
	write(fd,&comma,1);
	close(fd);
	printf("written!");
}

int main(){
	int position = 0;
	char change = 'n';
	
	printf("change position? (y /n )?\n");
	scanf("%c", &change);
	if (change == 'y'){
		printf("enter new position:\n");
		scanf("%d",&position);
		
		
		
	}
	long sleep_time=2000;
	while (1){
		while (getchar() != '\n');
		printf("position: %d\n",position);
		uint8_t values_to_send[2];
		uint16_t input_value;
		printf("INPUT, UP TO 65535:\n");
		scanf("%hu",&input_value);
		get_bytes_to_send(values_to_send,input_value);
		
		
		int i2c_fd;
		i2c_fd = open("/dev/i2c-2",O_RDWR);
		if (i2c_fd < 1){
			printf("error opening i2c bus\n");
			return(1);
		}
		int vca_addr=0x60;
		unsigned char outbuf[2];
		struct i2c_rdwr_ioctl_data packets; //the big struct
		struct i2c_msg messages[1]; //messages go in here
		messages[0].addr  = vca_addr;
		messages[0].flags = 0;
		messages[0].len   = sizeof(outbuf);
		messages[0].buf  = outbuf;
		outbuf[0] = values_to_send[0];
		outbuf[1] = values_to_send[1];

		packets.msgs  = messages;
		packets.nmsgs = 1;
		if(ioctl(i2c_fd, I2C_RDWR, &packets) < 0) {
			printf("sent\n");
		}
		char dowrite = 'n';
		printf("write to array at position %d ?\n(y or n or x)",position);
		while (getchar() != '\n');
		scanf("%c",&dowrite);
		if (dowrite == 'y'){
			append_file(input_value);
			printf("appended to file at position %d \n", position);
			position=position + 1;
		}
		if (dowrite == 'x'){
			
		uint16_t note_array[49]={100,157,255,375,577,830,1190,1635,2200,2850,3680,4670,5680,6950,8450,9975,11600,13300,15050,16825,18660,20450,22225,23825,25420,26940,28360,29650,30900,32050,33050,34020,34980,35790,36500,37175,37825,38410,38920,39450,39900,40300,40700,41050,41390,41700,41975,42250,42510};
		
		int i=0;
		while(1){
			int i2c_fd;
			i2c_fd = open("/dev/i2c-2",O_RDWR);
			if (i2c_fd < 1){
				printf("error opening i2c bus\n");
			return(1);
			}
		uint8_t values_to_send[2];
		uint16_t input_value=note_array[i];
		get_bytes_to_send(values_to_send,input_value);
		int vca_addr=0x60;
		unsigned char outbuf[2];
		struct i2c_rdwr_ioctl_data packets; //the big struct
		struct i2c_msg messages[1]; //messages go in here
		messages[0].addr  = vca_addr;
		messages[0].flags = 0;
		messages[0].len   = sizeof(outbuf);
		messages[0].buf  = outbuf;
		outbuf[0] = values_to_send[0];
		outbuf[1] = values_to_send[1];

		packets.msgs  = messages;
		packets.nmsgs = 1;
		ioctl(i2c_fd, I2C_RDWR, &packets);
			
		
		if (i==0){i=49;}
		i=i-1;	
		msleep(sleep_time);
		if (sleep_time > 0){
			sleep_time=sleep_time-10;
		}
		}
		
		
		}
}

}	
		
   

	
	
	
	
	
	