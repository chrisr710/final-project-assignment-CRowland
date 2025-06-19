#include<ncurses.h>
#include<stdio.h>
#include <unistd.h>
struct params {
	int note_number;
	int note_value;
	
	
};


struct params curr_params;


void display_values(void){
	printf("\033[3J\033[H\033[2J");
	printf("\033[%d;%dHNote_Number:%d\n", 1, 0,curr_params.note_number);
	printf("\033[%d;%dHNote_Loudness: %d\n",2, 0,curr_params.note_value);
	
}

int main(void){
display_values();
sleep(1);
curr_params.note_number = 1;
display_values();
sleep(1);
curr_params.note_number = 2;
display_values();
sleep(1);
curr_params.note_number = 3;
display_values();
sleep(1);
curr_params.note_number = 2;
display_values();
sleep(1);
curr_params.note_number = 0;
display_values();


}