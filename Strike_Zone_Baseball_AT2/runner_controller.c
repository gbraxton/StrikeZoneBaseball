/*
 * runner_controller.c
 *
 * Created: 5/27/2013 3:04:07 PM
 *  Author: George
 */ 

#include "runner_controller.h"

unsigned char runner_state_request = sm_runner_clear;
unsigned long runner_action_last = 0;
unsigned long runner_action_request = 0;

unsigned char runner_on_1B = 0;
unsigned char runner_on_2B = 0;
unsigned char runner_on_3B = 0;

unsigned char runner_runner_pos_count = 0;
unsigned long runner_pace_count = 0;

unsigned char run_length = 0;

unsigned char run_path[] = { 0x2B, 0x48, 0x47, 0x46, 0x45, 0x44, 0x37, 0x36, 0x35, 0x34, 0x23, 0x14, 0x15, 0x16, 0x17, 0x04, 0x05, 0x06, 0x07, 0x08, 0x2B, 0x0C }; 
							
unsigned long tmp_long;

void clear_runners(){
	uart_push_message( ((unsigned long) field_matrix_batter << 16) | 0x0000);
	uart_push_message( ((unsigned long) field_matrix_runner1 << 16) | 0x0000);
	uart_push_message( ((unsigned long) field_matrix_runner2 << 16) | 0x0000);
	uart_push_message( ((unsigned long) field_matrix_runner3 << 16) | 0x0000);
	runner_on_1B = 0;
	runner_on_2B = 0;
	runner_on_3B = 0;
}

signed char check_request(){
	if((runner_action_request != runner_action_last) || (runner_state_request != sm_runner_state)){
		runner_action_last = runner_action_request;
		if(runner_state_request == sm_runner_sequence){
			return sm_runner_prepare0;
		}
		if(runner_state_request == sm_runner_batterup) {
			tmp_long =  ((unsigned long) field_matrix_batter << 16) | ((unsigned short) 0x2B << 8) | 0xF0;
			uart_push_message(tmp_long);
		}
	}
	return runner_state_request;
}

signed char sm_runner_tick(signed char state){
	sm_runner_state = state;
	switch(sm_runner_state){// Transitions
		case sm_runner_boot:
			sm_runner_state = sm_runner_clear;
			break;
		case sm_runner_clear:
			sm_runner_state = check_request();
			break;
		case sm_runner_batterup:
			sm_runner_state = check_request();
			break;
		case sm_runner_wait:
			sm_runner_state = check_request();
			break;
		case sm_runner_prepare0:
			sm_runner_state = sm_runner_prepare1;
			break;
		case sm_runner_prepare1:
			sm_runner_state = sm_runner_prepare2;
			break;
		case sm_runner_prepare2:
			sm_runner_state = sm_runner_prepare3;
			break;
		case sm_runner_prepare3:
			sm_runner_state = sm_runner_prepare4;
			break;
		case sm_runner_prepare4:
			sm_runner_state = sm_runner_update1;
			break;
		case sm_runner_update1:
			sm_runner_state = sm_runner_update2;
			break;
		case sm_runner_update2:
			sm_runner_state = sm_runner_update3;
			break;
		case sm_runner_update3:
			sm_runner_state = sm_runner_update4;
			break;
		case sm_runner_update4:
			sm_runner_state = sm_runner_wait_pace;
			break;
		case sm_runner_wait_pace:
			if(runner_runner_pos_count < run_length){
				if(runner_pace_count < 40){
					sm_runner_state = sm_runner_wait_pace;
					runner_pace_count++;
				} else {
					sm_runner_state = sm_runner_update1;
					runner_pace_count = 0;
					runner_runner_pos_count++;
				}				
			} else {
				sm_runner_state = sm_runner_sequence;
			}
			break;
		case sm_runner_sequence:
			sm_runner_state = check_request();
			break;
	}
	switch(sm_runner_state){// Actions
		case sm_runner_clear:
			break;
		case sm_runner_batterup:
			break;
		case sm_runner_wait:
			break;
		case sm_runner_prepare0:
			if( (((runner_action_request & 0xFF000000) >> 24) == runner_base_index_1B) ){
				runner_on_1B = 1;
			} else {
				runner_on_1B = 0;
			}
			if(		(((runner_action_request & 0xFF000000) >> 24) == runner_base_index_2B) ||
			(((runner_action_request & 0x00FF0000) >> 16) == runner_base_index_2B) ){
				runner_on_2B = 1;
			} else {
				runner_on_2B = 0;
			}
			if(		(((runner_action_request & 0xFF000000) >> 24) == runner_base_index_3B) ||
			(((runner_action_request & 0x00FF0000) >> 16) == runner_base_index_3B) ||
			(((runner_action_request & 0x0000FF00) >> 8) == runner_base_index_3B) ){
				runner_on_3B = 1;
			} else {
				runner_on_3B = 0;
			}
			run_length = (runner_action_request & 0xFF000000) >> 24;
			runner_runner_pos_count = 0;
			runner_pace_count = 0;
			break;
		case sm_runner_prepare1:
			uart_push_message( ((unsigned long) field_matrix_runner1 << 16) | ((unsigned short) 0x44 << 8) | 0x00);
			break;
		case sm_runner_prepare2:
			uart_push_message( ((unsigned long) field_matrix_runner2 << 16) | ((unsigned short) 0x23 << 8) | 0x00);
			break;
		case sm_runner_prepare3:
			uart_push_message( ((unsigned long) field_matrix_runner3 << 16) | ((unsigned short) 0x04 << 8) | 0x00);
			break;
		case sm_runner_prepare4:
			uart_push_message( ((unsigned long) field_matrix_batter << 16) | ((unsigned short) 0x2B << 8) | 0x00);
			break;
		case sm_runner_update1:
			if(((runner_runner_pos_count) < 22) && (runner_action_request & 0xFF000000)){
				uart_push_message( ((unsigned long) field_matrix_runner1 << 16) | ((unsigned short) run_path[runner_runner_pos_count] << 8) | 0xF0);
			}
			break;
		case sm_runner_update2:
			if(((runner_runner_pos_count + runner_base_index_1B) < 22) && (runner_action_request & 0x00FF0000)){
				uart_push_message( ((unsigned long) field_matrix_runner2 << 16) | ((unsigned short) run_path[runner_runner_pos_count + runner_base_index_1B] << 8) | 0xF0);
			}
			break;
		case sm_runner_update3:
			if(((runner_runner_pos_count + runner_base_index_2B) < 22) && (runner_action_request & 0x0000FF00)){
				uart_push_message( ((unsigned long) field_matrix_runner3 << 16) | ((unsigned short) run_path[runner_runner_pos_count + runner_base_index_2B] << 8) | 0xF0);
			}
			break;
		case sm_runner_update4:
			if(((runner_runner_pos_count + runner_base_index_3B) < 21) && (runner_action_request & 0x000000FF)){
				uart_push_message( ((unsigned long) field_matrix_batter << 16) | ((unsigned short) run_path[runner_runner_pos_count + runner_base_index_3B] << 8) | 0xF0);
			}
			break;
		case sm_runner_wait_pace:
			break;
		case sm_runner_sequence:
			break;
	}
	return sm_runner_state;
}