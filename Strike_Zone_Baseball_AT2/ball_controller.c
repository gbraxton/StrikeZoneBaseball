/*
 * ball_controller.c
 *
 * Created: 5/24/2013 7:38:30 AM
 *  Author: George
 */ 

#include "ball_controller.h"

const unsigned char MAX_PATH_LENGTH = 12;

unsigned char ball_possession_current = ball_possession_clear;
unsigned char ball_possession_request = ball_possession_clear;
unsigned char ball_max_path_request = 12;
unsigned char ball_speed_current = 0;
unsigned char ball_speed_request = 100;
unsigned short ball_throw_current = 0;
unsigned short ball_throw_request = 0;

const unsigned char ball_possession_locations[] = { 0x0C, 0x26, 0x2C, 0x44, 0x23, 0x04, 0x13, 0x01, 0x21, 0x41 };
const unsigned char ball_possession_locations_length = 10;
const unsigned char ball_pitch_path[] = {0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C};
const unsigned char ball_pitch_path_length = 7;
const unsigned char ball_hit_path0[] = {0x2B, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x0C};
const unsigned char ball_hit_path0_length = 10;
const unsigned char* ball_hit_path_flyout_left = ball_hit_path0;
const unsigned char ball_hit_path_flyout_left_length = 9;
const unsigned char ball_hit_path_groundout_third[] = {0x2B, 0x08, 0x07, 0x06, 0x05, 0x04};
const unsigned char ball_hit_path_groundout_third_length = 6;
const unsigned char ball_hit_pathfl[] = {0x2B, 0x08, 0x07, 0x06, 0x05, 0x0C};
const unsigned char ball_hit_pathfl_length = 6;
const unsigned char ball_hit_path1[] = {0x2B, 0x19, 0x18, 0x16, 0x13, 0x12, 0x11, 0x10, 0x0C};
const unsigned char ball_hit_path1_length = 8;
const unsigned char ball_hit_path_groundout_short[] = {0x2B, 0x19, 0x18, 0x16, 0x13};
const unsigned char ball_hit_path_groundout_short_length = 5;
const unsigned char ball_hit_path2[] = {0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x0C};
const unsigned char ball_hit_path2_length = 12;
const unsigned char* ball_hit_path_flyout_center = ball_hit_path2;
const unsigned char ball_hit_path_flyout_center_length = 11;
const unsigned char ball_hit_path_groundout_second[] = {0x2B, 0x2A, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23};
const unsigned char ball_hit_path_groundout_second_length = 9;
const unsigned char* ball_hit_path_groundout_pitcher = ball_hit_path_groundout_second;
const unsigned char ball_hit_path_groundout_pitcher_length = 6;
const unsigned char ball_hit_path3[] = {0x2B, 0x39, 0x38, 0x36, 0x33, 0x32, 0x31, 0x30, 0x0C};
const unsigned char ball_hit_path3_length = 8;
const unsigned char ball_hit_path4[] = {0x2B, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40, 0x0C};
const unsigned char ball_hit_path4_length = 10;
const unsigned char* ball_hit_path_flyout_right = ball_hit_path4;
const unsigned char ball_hit_path_flyout_right_length = 9;
const unsigned char ball_hit_path_groundout_first[] = {0x2B, 0x48, 0x47, 0x46, 0x45, 0x44};
const unsigned char ball_hit_path_groundout_first_length = 6;
const unsigned char ball_hit_pathfr[] = {0x2B, 0x48, 0x47, 0x46, 0x45, 0x0C};
const unsigned char ball_hit_pathfr_length = 6;

unsigned char *ball_path_current = '\0';
unsigned char ball_path_index_current = 0;
unsigned char ball_path_length = 0;
unsigned long ball_speed_wait_cnt = 0;

// flags for making ball flight sound effects
unsigned char ball_rising_path = 0;
unsigned char ball_falling_path = 0;

unsigned char ball_get_ball_location(){
	if(ball_possession_current < ball_possession_path_pitch){
		return ball_possession_locations[ball_possession_current];
	}
	return ball_path_current[ball_path_index_current];
}

void ball_send_data_to_uart(unsigned char location){
	uart_push_message( ((unsigned long) field_matrix_ball << 16) | ((unsigned short) location << 8) | 0xF0);
}

unsigned char ball_check_request(){
	if(ball_possession_current == ball_possession_request){
		return sm_ball_state;
	}
	ball_possession_current = ball_possession_request;
	ball_speed_current = ball_speed_request;
	ball_throw_current = ball_throw_request;

	if( ball_possession_request <= ball_possession_right){
		ball_send_data_to_uart(ball_possession_locations[ball_possession_request]);
		return sm_ball_possessed;
	}
	
	switch(ball_possession_request){
		case ball_possession_clear:
			ball_send_data_to_uart(0x00);
			return sm_ball_clear;
			break;
		case ball_possession_path_pitch:
			ball_path_current = ball_pitch_path;
			ball_path_length = ball_pitch_path_length;
			break;
		case ball_possession_path0:
			ball_path_current = ball_hit_path0;
			ball_path_length = (ball_max_path_request > ball_hit_path0_length) ? ball_hit_path0_length + 1 : ball_max_path_request;
			break;
		case ball_possession_path1:
			ball_path_current = ball_hit_path1;
			ball_path_length = (ball_max_path_request > ball_hit_path1_length) ? ball_hit_path1_length + 1 : ball_max_path_request;
			break;
		case ball_possession_path2:
			ball_path_current = ball_hit_path2;
			ball_path_length = (ball_max_path_request > ball_hit_path2_length) ? ball_hit_path2_length + 1 : ball_max_path_request;
			break;
		case ball_possession_path3:
			ball_path_current = ball_hit_path3;
			ball_path_length = (ball_max_path_request > ball_hit_path3_length) ? ball_hit_path3_length + 1 : ball_max_path_request;
			break;
		case ball_possession_path4:
			ball_path_current = ball_hit_path4;
			ball_path_length = (ball_max_path_request > ball_hit_path4_length) ? ball_hit_path4_length + 1 : ball_max_path_request;
			break;
		case ball_possession_pathfl:
			ball_path_current = ball_hit_pathfl;
			ball_path_length = ball_hit_pathfl_length;
			break;
		case ball_possession_pathfr:
			ball_path_current = ball_hit_pathfr;
			ball_path_length = ball_hit_pathfr_length;
			break;
		case ball_possession_path_groundout_third:
			ball_path_current = ball_hit_path_groundout_third;
			ball_path_length = ball_hit_path_groundout_third_length;
			break;
		case ball_possession_path_groundout_second:
			ball_path_current = ball_hit_path_groundout_second;
			ball_path_length = ball_hit_path_groundout_second_length;
			break;
		case ball_possession_path_groundout_short:
			ball_path_current = ball_hit_path_groundout_short;
			ball_path_length = ball_hit_path_groundout_short_length;
			break;
		case ball_possession_path_groundout_pitcher:
			ball_path_current = ball_hit_path_groundout_pitcher;
			ball_path_length = ball_hit_path_groundout_pitcher_length;
			break;
		case ball_possession_path_groundout_first:
			ball_path_current = ball_hit_path_groundout_first;
			ball_path_length = ball_hit_path_groundout_first_length;
			break;
		case ball_possession_path_flyout_left:
			ball_path_current = ball_hit_path_flyout_left;
			ball_path_length = ball_hit_path_flyout_left_length;
			break;
		case ball_possession_path_flyout_center:
			ball_path_current = ball_hit_path_flyout_center;
			ball_path_length = ball_hit_path_flyout_center_length;
			break;
		case ball_possession_path_flyout_right:
			ball_path_current = ball_hit_path_flyout_right;
			ball_path_length = ball_hit_path_flyout_right_length;
			break;
		case ball_possession_throw:
			//TODO Throwing
			break;
	}
	ball_path_index_current = 0;
	ball_speed_wait_cnt = 0;
	return sm_ball_path;
}

signed char sm_tick_ball(signed char state){
	sm_ball_state = state;
	switch(sm_ball_state){ // Transitions
		case sm_ball_boot:
			sm_ball_state = sm_ball_clear;
			break;
		case sm_ball_clear:
			sm_ball_state = ball_check_request();
			break;
		case sm_ball_possessed:
			sm_ball_state = ball_check_request();
			break;
		case sm_ball_path:
			sm_ball_state = ball_check_request();
			break;
	}
	switch(sm_ball_state){ // Actions
		case sm_ball_clear:
			ball_possession_current = ball_possession_clear;
			break;
		case sm_ball_possessed:
			break;
		case sm_ball_path:
			if(ball_speed_wait_cnt >= ball_speed_current){
				if(ball_path_index_current < (ball_path_length - 1)){ 
					ball_path_index_current++;
					ball_send_data_to_uart( ball_path_current[ball_path_index_current]);
				}					
					ball_speed_wait_cnt = 0;
			} else {
				ball_speed_wait_cnt++;
			}				
			break;
	}
	return sm_ball_state;
}