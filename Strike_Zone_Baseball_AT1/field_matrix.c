/*
 * field_matrix.c
 *
 * Created: 5/19/2013 12:14:48 AM
 *  Author: George
 */ 

#include "field_matrix.h"

// Dimensions of Matrix
const unsigned char num_columns = 5;
const unsigned char num_rows = 13;
// Masks
const unsigned short red_positions_mask =   0x01FF; //bits 8:0 represent the fielder positions - Red
const unsigned short green_positions_mask = 0x3E00; //bits 13:9 represent the batter/runners/ball - Green
// Array of field LED locations
unsigned short field_array[5][13] = {0};
// Current location of position saved so previous position can be unset when location updated
unsigned char positions_current_location[14] = {0};
// Column Scan Render
static unsigned char current_render_column = 0;
// Zone Matrix
unsigned short zone_pitch_led_value = 0x0000;
unsigned short zone_swing_led_value = 0x0000;
// Temporary Variables
signed char i;
unsigned short tmp_col_write = 0;

//value------Format [column location:4bits][row location:4bits][green duration:4bits][red duration:4bits]
void update_position(unsigned char position, unsigned short value){
	switch(position){
		case field_matrix_pitcher:
			if(positions_current_location[field_matrix_pitcher]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_pitcher] >> 4)][positions_current_location[field_matrix_pitcher] & 0x0F]
					&= ~(1 << field_matrix_pitcher);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_pitcher);
				}				
				positions_current_location[field_matrix_pitcher] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_catcher:
			if(positions_current_location[field_matrix_catcher]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_catcher] >> 4)][positions_current_location[field_matrix_catcher] & 0x0F]
				&= ~(1 << field_matrix_catcher);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_catcher);
				}				
				positions_current_location[field_matrix_catcher] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_first_base:
			if(positions_current_location[field_matrix_first_base]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_first_base] >> 4)][positions_current_location[field_matrix_first_base] & 0x0F]
				&= ~(1 << field_matrix_first_base);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_first_base);
				}				
				positions_current_location[field_matrix_first_base] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_second_base:
			if(positions_current_location[field_matrix_second_base]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_second_base] >> 4)][positions_current_location[field_matrix_second_base] & 0x0F]
				&= ~(1 << field_matrix_second_base);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_second_base);
				}				
				positions_current_location[field_matrix_second_base] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_third_base:
			if(positions_current_location[field_matrix_third_base]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_third_base] >> 4)][positions_current_location[field_matrix_third_base] & 0x0F]
				&= ~(1 << field_matrix_third_base);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_third_base);
				}				
				positions_current_location[field_matrix_third_base] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_short_stop:
			if(positions_current_location[field_matrix_short_stop]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_short_stop] >> 4)][positions_current_location[field_matrix_short_stop] & 0x0F]
				&= ~(1 << field_matrix_short_stop);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_short_stop);
				}				
				positions_current_location[field_matrix_short_stop] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_left_fielder:
			if(positions_current_location[field_matrix_left_fielder]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_left_fielder] >> 4)][positions_current_location[field_matrix_left_fielder] & 0x0F]
				&= ~(1 << field_matrix_left_fielder);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_left_fielder);
				}				
				positions_current_location[field_matrix_left_fielder] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_center_fielder:
			if(positions_current_location[field_matrix_center_fielder]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_center_fielder] >> 4)][positions_current_location[field_matrix_center_fielder] & 0x0F]
				&= ~(1 << field_matrix_center_fielder);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_center_fielder);
				}				
				positions_current_location[field_matrix_center_fielder] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_right_fielder:
			if(positions_current_location[field_matrix_right_fielder]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_right_fielder] >> 4)][positions_current_location[field_matrix_right_fielder] & 0x0F]
				&= ~(1 << field_matrix_right_fielder);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_right_fielder);
				}				
				positions_current_location[field_matrix_right_fielder] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_batter:
			if(positions_current_location[field_matrix_batter]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_batter] >> 4)][positions_current_location[field_matrix_batter] & 0x0F]
				&= ~(1 << field_matrix_batter);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_batter);
				}				
				positions_current_location[field_matrix_batter] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_runner1:
			if(positions_current_location[field_matrix_runner1]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_runner1] >> 4)][positions_current_location[field_matrix_runner1] & 0x0F]
				&= ~(1 << field_matrix_runner1);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_runner1);
				}				
				positions_current_location[field_matrix_runner1] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_runner2:
			if(positions_current_location[field_matrix_runner2]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_runner2] >> 4)][positions_current_location[field_matrix_runner2] & 0x0F]
				&= ~(1 << field_matrix_runner2);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_runner2);
				}				
				positions_current_location[field_matrix_runner2] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_runner3:
			if(positions_current_location[field_matrix_runner3]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_runner3] >> 4)][positions_current_location[field_matrix_runner3] & 0x0F]
				&= ~(1 << field_matrix_runner3);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_runner3);
				}				
				positions_current_location[field_matrix_runner3] = (unsigned char) (value >> 8);
			}
			break;
		case field_matrix_ball:
			if(positions_current_location[field_matrix_ball]  != (value >> 8)){
				field_array[(positions_current_location[field_matrix_ball] >> 4)][positions_current_location[field_matrix_ball] & 0x0F]
				&= ~(1 << field_matrix_ball);
				if(value & 0x00FF){
					field_array[value >> 12][(value & 0x0F00) >> 8] |= (1 << field_matrix_ball);
				}				
				positions_current_location[field_matrix_ball] = (unsigned char) (value >> 8);
			}
			break;
		case zone_matrix_pitch_location:
			zone_pitch_led_value = value;
			break;
		case zone_matrix_swing_location:
			
			break;
		case sound_state:
			sound_state_request = value >> 8;
			sound_request = value & 0x0F;
			break;
		case sound_flight:
			sound_state_request = sm_sound_flight;
			sound_flight_time_request = value;
			break;
	}
}

void turn_off_pin(unsigned char* const port, const unsigned char pin){
	*port &= ~(1 << pin);
}

void turn_on_pin(unsigned char* const port, const unsigned char pin){
	*port |= (1 << pin);
}

void pulse_pin(unsigned char* const port, const unsigned char pin){
	turn_on_pin(port, pin);
	turn_off_pin(port, pin);
}

void field_write_green_col(unsigned char data){
	unsigned char i;
	for(i=0; i < 8; i++){
		if( data & 0x80 ){
			turn_on_pin(shift_reg_col_green_port, shift_reg_col_green_ds);
		} else {
			turn_off_pin(shift_reg_col_green_port, shift_reg_col_green_ds);
		}
		data = data << 1;
		pulse_pin(shift_reg_col_green_port, shift_reg_col_green_sh);
	}	
}

void field_write_red_col(unsigned char data){
	unsigned char i;
	for(i=0; i < 8; i++){
		if( data & 0x80 ){
			turn_on_pin(shift_reg_col_red_port, shift_reg_col_red_ds);
		} else {
			turn_off_pin(shift_reg_col_red_port, shift_reg_col_red_ds);
		}
		data = data << 1;
		pulse_pin(shift_reg_col_red_port, shift_reg_col_red_sh);
	}
}

void field_write_row(unsigned short data){
	unsigned char i;
	for(i=0; i < 16; i++){
		if( data & 0x8000 ){
			turn_on_pin(shift_reg_row_port, shift_reg_row_ds);
		} else {
			turn_off_pin(shift_reg_row_port, shift_reg_row_ds);
		}
		data = data << 1;
		pulse_pin(shift_reg_row_port, shift_reg_row_sh);
	}
}

void zone_write_green_col(unsigned char data){
	unsigned char i;
	for(i=0; i < 8; i++){
		if( data & 0x80 ){
			turn_on_pin(zone_col_green_port, zone_col_green_ds);
		} else {
			turn_off_pin(zone_col_green_port, zone_col_green_ds);
		}
		data = data << 1;
		pulse_pin(zone_col_green_port, zone_col_green_sh);
	}
}

void zone_write_red_col(unsigned char data){
	unsigned char i;
	for(i=0; i < 8; i++){
		if( data & 0x80 ){
			turn_on_pin(zone_col_red_port, zone_col_red_ds);
		} else {
			turn_off_pin(zone_col_red_port, zone_col_red_ds);
		}
		data = data << 1;
		pulse_pin(zone_col_red_port, zone_col_red_sh);
	}
}

void zone_write_row(unsigned char data){
	unsigned char i;
	for(i=0; i < 8; i++){
		if( data & 0x80 ){
			turn_on_pin(zone_row_port, zone_row_ds);
		} else {
			turn_off_pin(zone_row_port, zone_row_ds);
		}
		data = data << 1;
		pulse_pin(zone_row_port, zone_row_sh);
	}
}

void initialize_fielders(){
	//pitcher
	field_array[2][6] |= (1 << field_matrix_pitcher);
	//catcher
	field_array[2][12] |= (1 << field_matrix_catcher);
	//first base
	field_array[4][4] |= (1 << field_matrix_first_base);
	//second base
	field_array[2][3] |= (1 << field_matrix_second_base);
	//third base
	field_array[0][4] |= (1 << field_matrix_third_base);
	//short stop
	field_array[1][3] |= (1 << field_matrix_short_stop);
	//left field
	field_array[0][1] |= (1 << field_matrix_left_fielder);
	//center field
	field_array[2][1] |= (1 << field_matrix_center_fielder);
	//right field
	field_array[4][1] |= (1 << field_matrix_right_fielder);
}

unsigned char field_matrix_tick(unsigned char state){
	field_matrix_state = state;
	// FSM Transitions
	switch(field_matrix_state){
		case Field_matrix_boot:
			field_matrix_state = Field_Matrix_Initialize;
			break;
		case Field_Matrix_Initialize:
			field_matrix_state = Render_Red;
			break;
		case Render_Red:
			field_matrix_state = Render_Red2;
			break;
		case Render_Red2:
			field_matrix_state = Render_Green;
			break;
		case Render_Green:
			field_matrix_state = Render_Red;
			break;
	}
	
	//FSM Actions
	switch(field_matrix_state){
		case Field_Matrix_Initialize:
			initialize_fielders();
			zone_write_green_col(0);
			zone_write_red_col(0);
			zone_write_row(0xFF);
			pulse_pin(shift_reg_all_port, shift_reg_all_st);
			break;
		case Render_Red:
			field_write_green_col(0);
			field_write_red_col(1 << current_render_column);
			tmp_col_write = 0;
			for(i = 0; i < 13; i++){
				if(field_array[current_render_column][i] & red_positions_mask){
					tmp_col_write |= (1 << i);
				}
			}
			field_write_row( tmp_col_write );
			if(zone_pitch_led_value){
				zone_write_green_col(1 << ((zone_pitch_led_value & 0xF000) >> 12));
				zone_write_row(~(1 << ((zone_pitch_led_value & 0x0F00) >> 8)));
			} else {
				zone_write_green_col(0);
				zone_write_row(0xFF);
			}
			pulse_pin(shift_reg_all_port, shift_reg_all_st);
			break;
		case Render_Red2:
			break;
		case Render_Green:
			field_write_green_col(1 << (unsigned short) current_render_column);
			field_write_red_col(0);
			tmp_col_write = 0;
			for(i = 0; i < 13; i++){
				if(field_array[current_render_column][i] & green_positions_mask){
					tmp_col_write |= (1 << i);
				}
			}
			field_write_row( tmp_col_write );
			pulse_pin(shift_reg_all_port, shift_reg_all_st);
			current_render_column++;
			if(current_render_column >= 6){
				current_render_column = 0;
			}
			break;
	}
	return field_matrix_state;
}