/*
 * game_controller.c
 *
 * Created: 5/24/2013 11:37:52 AM
 *  Author: George
 */ 

#include "game_controller.h"

// Variable for seeding rand_r
static unsigned short seed_r = 0;

// Length of path to follow for different hits
const unsigned char single_length[] = { 8, 6, 10, 6, 8};
const unsigned char double_triple_length[] = { 10, 8, 12, 8, 10};
const unsigned char homerun_length = 15;

// Scoring Variables
unsigned char player_score = 0;
unsigned char cpu_score = 0;
unsigned char strikes = 0;
unsigned char balls = 0;
unsigned char outs = 0;
unsigned char inning = 1;
unsigned char top_bot_innning = 0;
unsigned char num_innings = 1;
unsigned char last_play_result = 0;

// Pitch Variables
signed char selected_pitch = 0;
signed char pitch_location_h = 0;
signed char pitch_location_v = 0;
signed char vertical_pitchmeter_result = 0;
signed char horizontal_pitchmeter_result = 0;
unsigned char pitch_difficulty = 0;
unsigned char last_zone_write = 0xFF; //Init to non-existent location

// Batting Variables
unsigned char swing_attempt = 0;
unsigned char hit_success = 0;
unsigned char swing_path_location = 0;
unsigned char swing_zone_location_h = 0;
unsigned char swing_zone_location_v = 0;

// Ball Variables
unsigned char ball_location;
unsigned char hit_path = 0;
unsigned char hit_length = 0;

// AI varibles
unsigned char pitch_confidence[] = {0, 0, 0, 0};

// Counters;
unsigned long wait_counter = 0;

// Temporary Variables
unsigned char tmp_char = 0;
unsigned short tmp_short = 0;
unsigned long tmp_long = 0;

// Utility Functions
void set_pitch_difficulty(){
	pitch_difficulty = pitches_difficulty[selected_pitch];
	if( (vertical_pitchmeter_result == 1) || (vertical_pitchmeter_result == 4) ){
		pitch_difficulty++;
	}
	if( (horizontal_pitchmeter_result == 1) || (horizontal_pitchmeter_result == 4) ){
		pitch_difficulty++;
	}
}

void game_adjust_count(){
	switch(last_play_result){
		case last_ball:
			balls++;
			if(balls > 3){
				last_play_result = last_walk;
				balls = 0;
				strikes = 0;
			}
			break;
		case last_strike_looking:
			strikes++;
			if(strikes > 2){
				balls = 0;
				strikes = 0;
				outs++;
				pitch_confidence[0] = 0;
				pitch_confidence[1] = 0;
				pitch_confidence[2] = 0;
				pitch_confidence[3] = 0;		
			}
			break;
		case last_strike_swinging:
			strikes++;
			if(strikes > 2){
				balls = 0;
				strikes = 0;
				outs++;
				pitch_confidence[0] = 0;
				pitch_confidence[1] = 0;
				pitch_confidence[2] = 0;
				pitch_confidence[3] = 0;
			}
			break;
		case last_foul:
			if(strikes < 2){
				strikes++;
			}
			break;
		case last_single:
			strikes = 0;
			balls = 0;
			break;
		case last_double:
			strikes = 0;
			balls = 0;
			break;
		case last_triple:
			strikes = 0;
			balls = 0;
			break;
		case last_homerun:
			strikes = 0;
			balls = 0;
			break;
		case last_groundout:
			outs++;
			strikes = 0;
			balls = 0;
			pitch_confidence[0] = 0;
			pitch_confidence[1] = 0;
			pitch_confidence[2] = 0;
			pitch_confidence[3] = 0;
			break;
		case last_flyout:
			outs++;
			strikes = 0;
			balls = 0;
			pitch_confidence[0] = 0;
			pitch_confidence[1] = 0;
			pitch_confidence[2] = 0;
			pitch_confidence[3] = 0;
			break;
	}
}

signed char select_num_innings(){
	if( (current_key == 'A') ||
			(current_key == 'B') ||
			(current_key == 'C') ||
			(current_key == 'D') ||
			(current_key == '*') ||
			(current_key == '0') ||
			(current_key == '#') ||
			(current_key == '\0') ||
			(!LCD_rdy_g) ){
		return sm_game_sel_innings;
	} else {
		num_innings = ( ((unsigned char) current_key) - ((unsigned char) '0'));
		lcd_display_mode_request = lcd_count;
		ball_possession_request = ball_possession_pitcher;
		runner_state_request = sm_runner_batterup;
		wait_counter = 0;
		srand(seed_r);
		return sm_game_top_show_score;//////////sm_game_bot_wait_show_score for testing hitting/////////////////
	}
}

void ai_swing(){
	pitch_confidence[selected_pitch]++;
	if(pitch_confidence[(selected_pitch + 1) % 4] > 0){
		pitch_confidence[(selected_pitch + 1) % 4]--;
	}
	if(pitch_confidence[(selected_pitch + 2) % 4] > 0){
		pitch_confidence[(selected_pitch + 2) % 4]--;
	}
	if(pitch_confidence[(selected_pitch + 3) % 4] > 0){
		pitch_confidence[(selected_pitch + 3) % 4]--;
	}
	
	if(  (vertical_pitchmeter_result > 1) && (vertical_pitchmeter_result < 4) &&
			(horizontal_pitchmeter_result > 1) && (horizontal_pitchmeter_result < 4) ){
				
			tmp_char = rand() % 5;
			if( tmp_char > 0){
				swing_attempt = 1;
				if( (pitch_difficulty + (rand() % 4 ) - pitch_confidence[selected_pitch]) < 5 ){
					hit_success = 1;
				} else {
					hit_success = 0;
				}
			} else {
				swing_attempt = 0;
				hit_success = 0;
			}
	} else if( (vertical_pitchmeter_result > 0) && (vertical_pitchmeter_result < 5) &&
				(horizontal_pitchmeter_result > 0) && (horizontal_pitchmeter_result < 5) ){
		
		tmp_char = rand() % 3;
		if(tmp_char > 1){
			swing_attempt = 1;
			if( (pitch_difficulty + (rand() % 4 ) - pitch_confidence[selected_pitch]) < 5 ){
				hit_success = 1;
			} else {
				hit_success = 0;
			}
		} else {
			swing_attempt = 0;
			hit_success = 0;
		}
	} else {
		swing_attempt = 0;
		hit_success = 0;
	}
}

void ai_select_pitch(){
	tmp_char = rand() % 8;
	if(tmp_char < 1){
		selected_pitch = curveball;
	} else if(tmp_char < 3){
		selected_pitch = slider;
	} else if(tmp_char < 5){
		selected_pitch = changeup;
	} else {
		selected_pitch = fastball;
	}
	tmp_char = rand() % 6;
	if(tmp_char < 1){
		tmp_char = rand() % 2;
		pitch_location_h = (5 * tmp_char);
		if((selected_pitch == curveball) && (pitch_location_h < 2)){
			pitch_location_h = 2;
		}
		if((selected_pitch == slider) && (pitch_location_h < 1)){
			pitch_location_h = 1;
		}
	} else if(tmp_char < 3){
		tmp_char = rand() % 2;
		pitch_location_h = 1 + (3 * tmp_char);
		if((selected_pitch == curveball) && (pitch_location_h < 2)){
			pitch_location_h = 2;
		}
		if((selected_pitch == slider) && (pitch_location_h < 1)){
			pitch_location_h = 1;
		}
	} else {
		tmp_char = rand() % 2;
		pitch_location_h = 2 + tmp_char;
	}
	tmp_char = rand() % 6;
	if(tmp_char < 1){
		tmp_char = rand() % 2;
		pitch_location_v = (5 * tmp_char);
		if((selected_pitch == curveball) && (pitch_location_v < 2)){
			pitch_location_h = 2;
		}
		if((selected_pitch == slider) && (pitch_location_v < 1)){
			pitch_location_v = 1;
		}
	} else if(tmp_char < 3){
		tmp_char = rand() % 2;
		pitch_location_v = 1 + (3 * tmp_char);
		if((selected_pitch == curveball) && (pitch_location_v < 2)){
			pitch_location_v = 2;
		}
		if((selected_pitch == slider) && (pitch_location_v < 1)){
			pitch_location_v = 1;
		}
	} else {
		tmp_char = rand() % 2;
		pitch_location_v = 2 + tmp_char;
	}
}

void player_swing(){
	switch(current_key){
		case '1':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_h = 1;
			swing_zone_location_v = 1;
			break;
		case '2':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 1;
			swing_zone_location_h = 2;
			break;
		case '3':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 1;
			swing_zone_location_h = 3;
			break;
		case 'A':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 1;
			swing_zone_location_h = 4;
			break;
		case '4':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 2;
			swing_zone_location_h = 1;
			break;
		case '5':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 2;
			swing_zone_location_h = 2;
			break;
		case '6':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 2;
			swing_zone_location_h = 3;
			break;
		case 'B':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 2;
			swing_zone_location_h = 4;
			break;
		case '7':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 3;
			swing_zone_location_h = 1;
			break;
		case '8':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 3;
			swing_zone_location_h = 2;
			break;
		case '9':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 3;
			swing_zone_location_h = 3;
			break;
		case 'C':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 3;
			swing_zone_location_h = 4;
			break;
		case '*':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 4;
			swing_zone_location_h = 1;
			break;
		case '0':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 4;
			swing_zone_location_h = 2;
			break;
		case '#':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 4;
			swing_zone_location_h = 3;
			break;
		case 'D':
			swing_path_location = ball_get_ball_location();
			swing_zone_location_v = 4;
			swing_zone_location_h = 4;
			break;
		default:
			break;
	}
}

void player_select_pitch(){
	if(current_key == '1'){
		selected_pitch = ((selected_pitch + 3) % num_pitches);
		sm_game_state = sm_game_top_pitch_select_wait;
		wait_counter = 0;
		if(lcd_display_mode_request == lcd_special){
			lcd_display_mode_request = lcd_clear;
		} else {
			strcpy(lcd_str_special, str_pitches[selected_pitch]);
			lcd_display_mode_request = lcd_special;
		}
	} else if(current_key == '3'){
		selected_pitch = ((selected_pitch + 1) % num_pitches);
		sm_game_state = sm_game_top_pitch_select_wait;
		wait_counter = 0;
		if(lcd_display_mode_request == lcd_special){
			lcd_display_mode_request = lcd_clear;
		} else {
			strcpy(lcd_str_special, str_pitches[selected_pitch]);
			lcd_display_mode_request = lcd_special;
		}
	} else if((current_key == '2') && LCD_rdy_g){
		sm_game_state = sm_game_top_pitchmeter_v;
		pitchmeter_speed = (10 - (pitches_difficulty[selected_pitch] * 2));
		pitchmeter_state_request = pitchmeter_vertical;
		lcd_display_mode_request = lcd_count;
		wait_counter = 0;
	} else {
		sm_game_state = sm_game_top_pitch_select;
		if(lcd_display_mode_request == lcd_clear){
			strcpy(lcd_str_special, str_pitches[selected_pitch]);
			lcd_display_mode_request = lcd_special;
		}
	}
}

void bot_pitch(){
	player_swing();
	ball_location = ball_get_ball_location();
	switch(ball_location){
		case 0x27:
			if(ball_location != last_zone_write){
				uart_push_message( ((unsigned long) zone_matrix_pitch_location << 16) |
									((unsigned short) (pitch_location_h - pitch_path_modifier[selected_pitch]) << 12) |
									((unsigned short) (pitch_location_v  - pitch_path_modifier[selected_pitch]) << 8) | (0xF0) );
				last_zone_write = ball_location;
			}
			break;
		case 0x28:
			break;
		case 0x29:
			break;
		case 0x2A:
			if(ball_location != last_zone_write){
				uart_push_message( ((unsigned long) zone_matrix_pitch_location << 16) |
									((unsigned short) pitch_location_h << 12) |
									((unsigned short) pitch_location_v << 8) |
									(0xF0) );
				last_zone_write = ball_location;
			}
			break;
		case 0x2B:
			break;
		case 0x2C:
			uart_push_message( ((unsigned long) zone_matrix_pitch_location << 16) | 0x0000 );
			last_zone_write = 0xFF;
		default:
			break;
	}
	if( ball_location == 0x2C){
		if( (swing_path_location == 0x2B) &&
			(swing_zone_location_h == pitch_location_h) &&
			(swing_zone_location_v == pitch_location_v) ){

			sm_game_state = sm_game_bot_call_play;
		} else {
			sm_game_state = sm_game_bot_call_pitch;
		}
	} else {
		sm_game_state = sm_game_bot_pitch;
	}
}

void top_call_pitch(){
	if(swing_attempt){
		last_play_result = last_strike_swinging;
		uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
	} else {
		if(  ((vertical_pitchmeter_result + pitch_path_modifier[selected_pitch]) > 0) && ((vertical_pitchmeter_result + pitch_path_modifier[selected_pitch]) < 5) &&
			((horizontal_pitchmeter_result + pitch_path_modifier[selected_pitch]) > 0) && ((horizontal_pitchmeter_result + pitch_path_modifier[selected_pitch]) < 5)  ){
				
			last_play_result = last_strike_looking;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
		} else {
			last_play_result = last_ball;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_ball) );
		}
	}
	game_adjust_count();
	if(last_play_result == last_walk){
		runner_action_request = ( (unsigned long) (runner_base_index_1B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_2B) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_3B) << 8) |
									( (runner_on_3B * runner_base_index_H) );
		runner_state_request = sm_runner_sequence;
		cpu_score += runner_on_3B;
	}
}

void top_call_play(){
	last_play_result = last_foul + (rand() % 7);
	game_adjust_count();
	switch(last_play_result){
		case last_groundout:
			hit_path = (ball_possession_path_groundout_third + (rand() % 5));
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_out) );
			break;
		case last_flyout:
			hit_path = (ball_possession_path_flyout_left +(rand() % 3));
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_out) );
			break;
		case last_foul:
			hit_path = (ball_possession_pathfl + (rand() % 2));		
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
			break;
		case last_single:
			hit_path = (rand() % 5) + 11;
			hit_length = single_length[hit_path - 11];
			runner_action_request = ( (unsigned long) (runner_base_index_1B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_2B) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_3B) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			cpu_score += runner_on_3B;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_double:
			hit_path = (rand() % 5) + 11;
			hit_length = double_triple_length[hit_path - 11];
			runner_action_request = ( (unsigned long) (runner_base_index_2B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_3B) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			cpu_score += (runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_triple:
			hit_path = (rand() % 5) + 11;
			hit_length = double_triple_length[hit_path - 11];
			runner_action_request = ( (unsigned long) (runner_base_index_3B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_H) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			cpu_score += (runner_on_1B + runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_homerun:
			hit_path = (rand() % 5) + 11;
			hit_length = homerun_length;
			runner_action_request = ( (unsigned long) (runner_base_index_H) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_H) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			cpu_score += (1 + runner_on_1B + runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_homerun) );
			break;
	}
}

void bot_call_pitch(){
	if(swing_path_location){
		last_play_result = last_strike_swinging;
		uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
	} else {
		if(  ((pitch_location_v > 0) && (pitch_location_v < 5)) &&
			((pitch_location_h > 0) && (pitch_location_h < 5))  ){

			last_play_result = last_strike_looking;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
		} else {
			last_play_result = last_ball;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_ball) );
		}
	}
	game_adjust_count();
	if(last_play_result == last_walk){
		runner_action_request = ( (unsigned long) (runner_base_index_1B) << 24 ) |
								( (unsigned long) (runner_on_1B * runner_base_index_2B) << 16  ) |
								( (unsigned short) (runner_on_2B * runner_base_index_3B) << 8) |
								( (runner_on_3B * runner_base_index_H) );
		runner_state_request = sm_runner_sequence;
		player_score += runner_on_3B;
	}
}

void bot_call_play(){
	last_play_result = last_foul + (rand() % 7);
	game_adjust_count();
	switch(last_play_result){
		case last_groundout:
			hit_path = (ball_possession_path_groundout_third + (rand() % 5));
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_out) );
			break;
		case last_flyout:
			hit_path = (ball_possession_path_flyout_left +(rand() % 3));
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_out) );
			break;
		case last_foul:
			hit_path = (ball_possession_pathfl + (rand() % 2));
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_strike) );
			break;
		case last_single:
			hit_path = (rand()%5) + 11;
			hit_length = single_length[hit_path - 11];
			runner_action_request = ( (unsigned long) (runner_base_index_1B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_2B) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_3B) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			player_score += runner_on_3B;
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_double:
			hit_path = (rand()%5) + 11;
			hit_length =double_triple_length[hit_path -11];
			runner_action_request = ( (unsigned long) (runner_base_index_2B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_3B) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			player_score += (runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_triple:
			hit_path = (rand()%5) + 11;
			hit_length = double_triple_length[hit_path -11];
			runner_action_request = ( (unsigned long) (runner_base_index_3B) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_H) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			player_score += (runner_on_1B + runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_hit) );
			break;
		case last_homerun:
			hit_path = (rand()%5) + 11;
			hit_length = homerun_length;
			runner_action_request = ( (unsigned long) (runner_base_index_H) << 24 ) |
									( (unsigned long) (runner_on_1B * runner_base_index_H) << 16  ) |
									( (unsigned short) (runner_on_2B * runner_base_index_H) << 8) |
									( (runner_on_3B * runner_base_index_H) );
			runner_state_request = sm_runner_sequence;
			player_score += (1 + runner_on_1B + runner_on_2B + runner_on_3B);
			uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_homerun) );
			break;
	}
}


// SM
signed char sm_tick_game(signed char state){
	sm_game_state = state;
	switch(sm_game_state){ // Transitions
		case sm_game_boot:
			sm_game_state = sm_game_wait_init;
			break;
		case sm_game_wait_init:
			if(wait_counter < 250){
				sm_game_state = sm_game_wait_init;
			} else {			
				sm_game_state = sm_game_intro;
				lcd_display_mode_request = lcd_intro;
				uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_play << 8) | (sound_intro) );
				wait_counter = 0;
			}
			break;			
		case sm_game_intro:
			if(wait_counter < 300){
				sm_game_state = sm_game_intro;
			} else {
				sm_game_state = sm_game_sel_innings;
				lcd_display_mode_request = lcd_select_innings;
				uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_wait << 8) | (sound_intro) );
			}
			break;
		case sm_game_sel_innings:
			sm_game_state = select_num_innings();
			break;
		case sm_game_top_show_score:
			if(wait_counter < 100){
				sm_game_state = sm_game_top_show_score;
			} else {
				sm_game_state = sm_game_top_show_inning;
				lcd_display_mode_request = lcd_inning;
				wait_counter = 0;
			}
			break;
		case sm_game_top_show_inning:
			if(wait_counter < 100){
				sm_game_state = sm_game_top_show_inning;
			} else {
				sm_game_state = sm_game_top_show_count;
				lcd_display_mode_request = lcd_count;
				wait_counter = 0;
			}
			break;
		case sm_game_top_show_count:
			if(wait_counter < 100){
				sm_game_state = sm_game_top_show_count;
			} else {
				sm_game_state = sm_game_top_pitch_select;
				strcpy(lcd_str_special, str_pitches[selected_pitch]);
				lcd_display_mode_request = lcd_special;
			}
			break;
		case sm_game_top_pitch_select:
			player_select_pitch();
			break;
		case sm_game_top_pitch_select_wait:
			if(wait_counter < 40){
				sm_game_state = sm_game_top_pitch_select_wait;
			} else {
				sm_game_state = sm_game_top_pitch_select;
			}
			break;
		case sm_game_top_pitchmeter_v:
			if(current_key == '5'){
				sm_game_state = sm_game_top_pitchmeter_v_wait;
				wait_counter = 0;
				vertical_pitchmeter_result = pitchmeter_v_location;
				pitchmeter_state_request = pitchmeter_horizontal;
			} else {
				sm_game_state = sm_game_top_pitchmeter_v;
			}			
			break;
		case sm_game_top_pitchmeter_v_wait:
			if(wait_counter < 20){
				sm_game_state = sm_game_top_pitchmeter_v_wait;
			} else {
				sm_game_state = sm_game_top_pitchmeter_h;
			}
			break;
		case sm_game_top_pitchmeter_h:
			if(current_key == '5'){
				sm_game_state = sm_game_top_pitchmeter_h_wait;
				horizontal_pitchmeter_result = (pitchmeter_h_location - 1) % 6;
				pitchmeter_state_request = pitchmeter_hold;
				set_pitch_difficulty();
				ai_swing();
				wait_counter = 0;
			} else {
				sm_game_state = sm_game_top_pitchmeter_h;
			}			
			break;
		case sm_game_top_pitchmeter_h_wait:
			if(wait_counter < 20){
				sm_game_state = sm_game_top_pitchmeter_h_wait;
			} else {
				sm_game_state = sm_game_top_pitch;
				ball_possession_request = ball_possession_path_pitch;
				ball_speed_request = pitch_speeds[selected_pitch];
				last_zone_write = 0;
			}
			break;
		case sm_game_top_pitch:
			ball_location = ball_get_ball_location();
			if((ball_location == 0x2A) && (last_zone_write == 0) ){
				uart_push_message( ((unsigned long) zone_matrix_pitch_location << 16) |
									((unsigned short) (horizontal_pitchmeter_result + pitch_path_modifier[selected_pitch]) << 12) |
									((unsigned short) (vertical_pitchmeter_result + pitch_path_modifier[selected_pitch]) << 8) |
									(0xF0) );
				last_zone_write = 0x2A;
			}			
			if( ball_location == 0x2C){
				if(hit_success == 1){
					sm_game_state = sm_game_top_call_play;
					
				} else {
					sm_game_state = sm_game_top_call_pitch;
				}
			} else {
				sm_game_state = sm_game_top_pitch;
			}
			break;
		case sm_game_top_call_pitch:
			if(last_play_result == last_walk){
				lcd_display_mode_request = lcd_last_play;
				sm_game_state = sm_game_top_play_sequence;
				wait_counter = 0;
			} else {
				sm_game_state = sm_game_top_wait_play;
				wait_counter = 0;
				lcd_display_mode_request = lcd_last_play;
			}
			break;
		case sm_game_top_call_play:
			sm_game_state = sm_game_top_play_sequence;
			wait_counter = 0;
			ball_speed_request = 35;
			ball_possession_request = hit_path;
			ball_max_path_request = hit_length;
			break;
		case sm_game_top_play_sequence:
			if(wait_counter < 250){
				sm_game_state = sm_game_top_play_sequence;
			} else {
				sm_game_state = sm_game_top_wait_play;
				wait_counter = 0;
				lcd_display_mode_request = lcd_last_play;
			}
			break;
		case sm_game_top_wait_play:
			if(wait_counter < 300){
				sm_game_state = sm_game_top_wait_play;
			} else if(outs > 2){
				sm_game_state = sm_game_bot_wait_show_score;
				outs = 0;
				strikes = 0;
				balls = 0;
				top_bot_innning = 1;
				lcd_display_mode_request = lcd_score;
				ball_possession_request = ball_possession_pitcher;
				pitchmeter_state_request = pitchmeter_clear;
				runner_on_1B = 0;
				runner_on_2B = 0;
				runner_on_3B = 0;
				runner_action_request = 0;
				clear_runners();
				runner_state_request = sm_runner_batterup;
				wait_counter = 0;
				uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_wait << 8) | (sound_hit) );
			} else {
				sm_game_state = sm_game_top_show_score;
				wait_counter = 0;
				lcd_display_mode_request = lcd_score;
				ball_possession_request = ball_possession_pitcher;
				pitchmeter_state_request = pitchmeter_clear;
				runner_state_request = sm_runner_batterup;
				uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_wait << 8) | (sound_hit) );
			}
			break;
		case sm_game_bot_wait_show_score:
			if(wait_counter < 100){
				sm_game_state = sm_game_bot_wait_show_score;
			} else {
				sm_game_state = sm_game_bot_wait_show_inning;
				lcd_display_mode_request = lcd_inning;
				wait_counter = 0;
			}
			break;
		case sm_game_bot_wait_show_inning:
			if(wait_counter < 100){
				sm_game_state = sm_game_bot_wait_show_inning;
			} else {
				sm_game_state = sm_game_bot_pitch_select;
				wait_counter = 0;
				lcd_display_mode_request = lcd_count;
				
			}
			break;
		case sm_game_bot_pitch_select:
			if(wait_counter < 200){
				sm_game_state = sm_game_bot_pitch_select;
				
			} else {
				ai_select_pitch();
				sm_game_state = sm_game_bot_pitch;
				swing_path_location = 0;
				ball_speed_request = pitch_speeds[selected_pitch] + 10;
				ball_possession_request = ball_possession_path_pitch;
			}
			break;
		case sm_game_bot_pitch:
			bot_pitch();
			break;
		case sm_game_bot_call_pitch:
			if(last_play_result == last_walk){
				sm_game_state = sm_game_bot_play_sequence;
				wait_counter = 0;
			} else {
				sm_game_state = sm_game_bot_wait_play;
				wait_counter = 0;
				lcd_display_mode_request = lcd_last_play;
			}			
			break;
		case sm_game_bot_call_play:
			sm_game_state = sm_game_bot_play_sequence;
			wait_counter = 0;
			ball_speed_request = 35;
			ball_possession_request = hit_path;
			ball_max_path_request = hit_length;
			break;
		case sm_game_bot_play_sequence:
			if(wait_counter < 250){
				sm_game_state = sm_game_bot_play_sequence;
			} else {
				sm_game_state = sm_game_bot_wait_play;
				wait_counter = 0;
				lcd_display_mode_request = lcd_last_play;
			}
			break;
		case sm_game_bot_wait_play:
			if(wait_counter < 350){
				sm_game_state = sm_game_bot_wait_play;
			} else if(outs > 2){
				if((inning >= num_innings) && (player_score != cpu_score)){
					sm_game_state = sm_game_game_over;
					lcd_display_mode_request = lcd_game_over;
				} else {
					sm_game_state = sm_game_top_show_score;
					inning++;
					outs = 0;
					balls = 0;
					strikes = 0;
					runner_on_1B = 0;
					runner_on_2B = 0;
					runner_on_3B = 0;
					runner_action_request = 0;
					clear_runners();
					top_bot_innning = 0;
					wait_counter = 0;
					lcd_display_mode_request = lcd_score;
					ball_possession_request = ball_possession_pitcher;
					runner_state_request = sm_runner_batterup;
					uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_wait << 8) | (sound_hit) );
				}
			} else {
				sm_game_state = sm_game_bot_wait_show_score;
				wait_counter = 0;
				lcd_display_mode_request = lcd_score;
				ball_possession_request = ball_possession_pitcher;
				runner_state_request = sm_runner_batterup;
				uart_push_message( ((unsigned long) sound_state << 16) | ((unsigned short) sm_sound_wait << 8) | (sound_hit) );
			}
			break;
		case sm_game_game_over:
			break;
	}

	switch(sm_game_state){	// Actions // TODO Add check queues
		case sm_game_wait_init:
			wait_counter++;
			break;
		case sm_game_intro:
			wait_counter++;
			break;
		case sm_game_sel_innings:
			seed_r++;
			break;
		case sm_game_top_show_score:
			wait_counter++;
			break;
		case sm_game_top_show_inning:
			wait_counter++;
			break;
		case sm_game_top_show_count:
			wait_counter++;
			break;
		case sm_game_top_pitch_select:
			break;
		case sm_game_top_pitch_select_wait:
			wait_counter++;
			break;
		case sm_game_top_pitchmeter_v:
			break;
		case sm_game_top_pitchmeter_v_wait:
			wait_counter++;
			break;
		case sm_game_top_pitchmeter_h:
			break;
		case sm_game_top_pitchmeter_h_wait:
			wait_counter++;
			break;
		case sm_game_top_pitch:
			break;
		case sm_game_top_call_pitch:
			top_call_pitch();
			break;
		case sm_game_top_call_play:
			top_call_play();
			break;
		case sm_game_top_play_sequence:
			wait_counter++;
			break;
		case sm_game_top_wait_play:
			wait_counter++;
			break;
		case sm_game_bot_wait_show_score:
			wait_counter++;
			break;
		case sm_game_bot_wait_show_inning:
			wait_counter++;
			break;
		case sm_game_bot_pitch_select:
			wait_counter++;
			break;
		case sm_game_bot_pitch:
			break;
		case sm_game_bot_call_pitch:
			bot_call_pitch();
			break;
		case sm_game_bot_call_play:
			bot_call_play();
			break;
		case sm_game_bot_play_sequence:
			wait_counter++;
			break;					
		case sm_game_bot_wait_play:
			wait_counter++;
			break;
		case sm_game_game_over:
			break;
	}
	return sm_game_state;
}