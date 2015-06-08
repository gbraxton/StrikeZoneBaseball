/*
 * field_matrix.h
 *
 * Created: 5/19/2013 12:07:53 AM
 *  Author: George
 */ 


#ifndef FIELD_MATRIX_H_
#define FIELD_MATRIX_H_

#include "port_assignments.h"
#include "sound_controller.h"

enum Field_Matrix_States{Field_matrix_boot, 
						Field_Matrix_Initialize, 
						Render_Red,
						Render_Red2,
						Render_Green} field_matrix_state;
	
unsigned char field_matrix_tick(unsigned char);
void update_position(unsigned char, unsigned short);

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
	};



#endif /* FIELD_MATRIX_H_ */