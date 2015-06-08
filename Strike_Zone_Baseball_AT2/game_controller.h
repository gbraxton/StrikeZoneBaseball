/*
 * game_status.h
 *
 * Created: 5/24/2013 7:05:34 AM
 *  Author: George
 */ 


#ifndef GAME_CONTROLLER_H_
#define GAME_CONTROLLER_H_

#include <stdlib.h>

#include "lcd_functions.h"
#include "keypad_functions.h"
#include "runner_controller.h"
#include "ball_controller.h"
#include "uart_functions2.h"
#include "pitchmeter.h"
#include "pitches.h"

extern unsigned char player_score;
extern unsigned char cpu_score;
extern unsigned char inning;
extern unsigned char top_bot_innning;
extern unsigned char strikes;
extern unsigned char balls;
extern unsigned char outs;
extern unsigned char last_play_result;
extern signed char selected_pitch;
extern signed char vertical_pitchmeter_result;
extern signed char horizontal_pitchmeter_result;
extern unsigned char pitch_difficulty;
static unsigned short seed_r;

enum Field_Matrix_Positions{
	field_matrix_pitcher = 0,
	field_matrix_catcher,
	field_matrix_first_base,
	field_matrix_second_base,
	field_matrix_third_base,
	field_matrix_short_stop,
	field_matrix_left_fielder,
	field_matrix_center_fielder,
	field_matrix_right_fielder,
	field_matrix_batter,
	field_matrix_runner1,
	field_matrix_runner2,
	field_matrix_runner3,
	field_matrix_ball,
	zone_matrix_pitch_location,
	zone_matrix_swing_location,
	sound_state,
	sound_flight
} field_positions;

enum Sounds{sound_intro, sound_ball, sound_strike, sound_out, sound_hit, sound_homerun};

enum SM_Sound_Controller_States{sm_sound_boot, sm_sound_wait, sm_sound_play, sm_sound_flight} sm_sound_controller_state;

enum Last_Play{last_ball,
				last_walk,
				last_strike_looking,
				last_strike_swinging,
				last_foul,
				last_single,
				last_double,
				last_triple,
				last_homerun,
				last_groundout,
				last_flyout};

enum SM_Game_States{sm_game_boot,
					sm_game_wait_init,
					sm_game_intro,
					sm_game_sel_innings,
					sm_game_top_show_score,
					sm_game_top_show_inning,
					sm_game_top_show_count,
					sm_game_top_pitch_select,
					sm_game_top_pitch_select_wait,
					sm_game_top_pitchmeter_v,
					sm_game_top_pitchmeter_v_wait,
					sm_game_top_pitchmeter_h,
					sm_game_top_pitchmeter_h_wait,
					sm_game_top_pitch,
					sm_game_top_call_pitch,
					sm_game_top_call_play,
					sm_game_top_play_sequence,
					sm_game_top_wait_play,
					sm_game_bot_wait_show_score,
					sm_game_bot_wait_show_inning,
					sm_game_bot_pitch_select,
					sm_game_bot_pitch,
					sm_game_bot_call_pitch,
					sm_game_bot_call_play,
					sm_game_bot_play_sequence,
					sm_game_bot_wait_play,
					sm_game_game_over
					} sm_game_state;

signed char sm_tick_game(signed char);
void ball_push_message(unsigned long);

#endif /* GAME_CONTROLLER_H_ */