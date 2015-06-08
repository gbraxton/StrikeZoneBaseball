/*
 * sound_controller.c
 *
 * Created: 6/6/2013 10:19:38 PM
 *  Author: George
 */ 

#include "sound_controller.h"

unsigned char sound_request = 0;
unsigned char sound_state_request = sm_sound_wait;
unsigned char current_sound = 0;
unsigned char current_sound_position = 0;
unsigned char sound_wait_counter = 0;
unsigned short flight_sound_freq = 250;
unsigned short sound_flight_time_request = 0;

unsigned short sounds[6][12] = {
								{261, 261, 523, 440, 392, 329, 392, 392, 392, 293, 293, 293}, //Intro
								{ 523, 523 }, //Ball
								{ 200, 200 }, //Strike
								{ 320, 180, 180 }, //Out
								{ 261, 523, 523 }, //Hit
								{ 261, 329, 392, 523, 523, 392, 523, 523 } //Homerun
							};
							
unsigned char sound_lengths[] = {12, 2, 3, 3, 3, 8};
	
void init_PWM() {
	TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS22) | (1 << CS20);
}

void set_PWM(double frequency) {
	if (frequency < 1){
		OCR2 = 0;
	}else{
		OCR2 = (int)(8000000 / (256 * frequency)) - 1;
	}
}
	
signed char sm_sound_controller_tick(signed char state){
	sm_sound_controller_state = state;
	
	switch(sm_sound_controller_state){ //Transitions
		case sm_sound_boot:
			sm_sound_controller_state = sm_sound_wait;
			break;
		case sm_sound_wait:
			if(sound_state_request == sm_sound_play){
				current_sound = sound_request;
				sm_sound_controller_state = sm_sound_play;
				sound_wait_counter = 0;
				current_sound_position = 0;
			} else if(sound_state_request == sm_sound_flight){
				sound_wait_counter = 0;
				flight_sound_freq = 250;
				sm_sound_controller_state = sm_sound_flight;
			} else {
				sm_sound_controller_state = sm_sound_wait;
			}
			break;
		case sm_sound_play:
			if( (current_sound_position < sound_lengths[current_sound]) ){
				sm_sound_controller_state = sm_sound_play;
			} else {
				set_PWM(0);
				sm_sound_controller_state = sound_state_request;
			}
			break;
		case sm_sound_flight:
			if(sound_wait_counter < sound_flight_time_request){
				sm_sound_controller_state = sm_sound_flight;
			} else {
				set_PWM(0);
				sound_wait_counter = 0;
				current_sound_position = 0;
				sm_sound_controller_state = sm_sound_play;
			}
			break;
	}
	switch(sm_sound_controller_state){ //Actions
		case sm_sound_boot:
			break;
		case sm_sound_wait:
			
			break;
		case sm_sound_play:
			if( current_sound_position < sound_lengths[current_sound] ){
				if(sound_wait_counter < 16){
					sound_wait_counter++;
				} else {
					sound_wait_counter = 0;
					set_PWM(sounds[current_sound][current_sound_position]);
					current_sound_position++;
				}
			} else {
				set_PWM(0);
			}
			break;
		case sm_sound_flight:
			if(sound_wait_counter < sound_flight_time_request){
				sound_wait_counter++;
				set_PWM(flight_sound_freq);
				if(sound_wait_counter < (sound_flight_time_request / 2)){
					flight_sound_freq += 1;
				} else {
					flight_sound_freq -= 1;
				}				
			}
			break;
	}
	return sm_sound_controller_state;
}