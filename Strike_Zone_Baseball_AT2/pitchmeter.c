/*
 * pitchmeter.c
 *
 * Created: 5/31/2013 12:34:46 PM
 *  Author: George
 */ 

#include "pitchmeter.h"

unsigned char pitchmeter_state_request = pitchmeter_wait;
unsigned char pitchmeter_speed = 0;
unsigned char pitchmeter_v_location = 0;
unsigned char pitchmeter_h_location = 0;
unsigned long pitchmeter_speed_wait_count = 0;

unsigned long tmp_value;
unsigned char tmp_v;
unsigned char tmp_h;

signed char sm_pitchmeter_tick(signed char state){
	sm_pitchmeter_state = state;
	
	switch(sm_pitchmeter_state){
		case pitchmeter_boot:
			sm_pitchmeter_state = pitchmeter_clear;
			break;
		case pitchmeter_clear:
			sm_pitchmeter_state = pitchmeter_wait;
			break;
		case pitchmeter_wait:
			if(pitchmeter_state_request == pitchmeter_clear){
				sm_pitchmeter_state = pitchmeter_wait;
			} else {
				sm_pitchmeter_state = pitchmeter_state_request;
			}			
			break;
		case pitchmeter_vertical:
			sm_pitchmeter_state = pitchmeter_state_request;
			break;
		case pitchmeter_horizontal:
			sm_pitchmeter_state = pitchmeter_state_request;
			break;
		case pitchmeter_hold:
			sm_pitchmeter_state = pitchmeter_state_request;
			break;
	}
	
	switch(sm_pitchmeter_state){
		case pitchmeter_boot:
			break;
		case pitchmeter_clear:
			tmp_value = ((unsigned long) zone_matrix_pitch_location << 16);
			uart_push_message(tmp_value);
			break;
		case pitchmeter_wait:
			break;
		case pitchmeter_vertical:
			if(pitchmeter_speed_wait_count < pitchmeter_speed){
				pitchmeter_speed_wait_count++;
			} else {
				tmp_value = ((unsigned long) zone_matrix_pitch_location << 16) |
							((unsigned short) pitchmeter_h_location << 12) |
							((unsigned short) pitchmeter_v_location << 8) | (0xF0);
				uart_push_message(tmp_value);
				pitchmeter_v_location = ((pitchmeter_v_location + 1) % 6);
				pitchmeter_speed_wait_count = 0;
			}			
			break;
		case pitchmeter_horizontal:
			if(pitchmeter_speed_wait_count < pitchmeter_speed){
				pitchmeter_speed_wait_count++;
			} else {
				
				tmp_value = ((unsigned long) zone_matrix_pitch_location << 16) |
				((unsigned short) pitchmeter_h_location << 12) |
				((unsigned short) pitchmeter_v_location << 8) | (0xF0);
				uart_push_message(tmp_value);
				pitchmeter_h_location = ((pitchmeter_h_location + 1) % 6);
				pitchmeter_speed_wait_count = 0;
			}
			break;
		case pitchmeter_hold:
			break;
	}
	
	return sm_pitchmeter_state;
}