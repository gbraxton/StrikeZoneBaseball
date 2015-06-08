/*
 * ball_controller.h
 *
 * Created: 5/24/2013 7:36:06 AM
 *  Author: George
 */ 


#ifndef BALL_CONTROLLER_H_
#define BALL_CONTROLLER_H_

#include "uart_functions2.h"
#include "game_controller.h"

extern const unsigned char MAX_PATH_LENGTH;

extern unsigned char ball_possession_request;
extern unsigned char ball_max_path_request;
extern unsigned char ball_speed_request;
extern unsigned short ball_throw_request;

enum Ball_Possession_Status{ ball_possession_clear,
							ball_possession_pitcher,
							ball_possession_catcher,
							ball_possession_first,
							ball_possession_second,
							ball_possession_third,
							ball_possession_short,
							ball_possession_left,
							ball_possession_center,
							ball_possession_right,
							ball_possession_path_pitch,
							ball_possession_path0,
							ball_possession_path1,
							ball_possession_path2,
							ball_possession_path3,
							ball_possession_path4,
							ball_possession_pathfl,
							ball_possession_pathfr,
							ball_possession_path_groundout_third,
							ball_possession_path_groundout_short,
							ball_possession_path_groundout_second,
							ball_possession_path_groundout_first,
							ball_possession_path_groundout_pitcher,
							ball_possession_path_flyout_left,
							ball_possession_path_flyout_center,
							ball_possession_path_flyout_right,
							ball_possession_throw } ball_possession_status;
							
enum SM_Ball_States{ sm_ball_boot, sm_ball_clear, sm_ball_possessed, sm_ball_path } sm_ball_state;
signed char sm_tick_ball(signed char);

unsigned char ball_get_ball_location();

#endif /* BALL_CONTROLLER_H_ */