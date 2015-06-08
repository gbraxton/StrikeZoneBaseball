/*
 * pitchmeter.h
 *
 * Created: 5/31/2013 12:34:27 PM
 *  Author: George
 */ 


#ifndef PITCHMETER_H_
#define PITCHMETER_H_

#include "game_controller.h"

extern unsigned char pitchmeter_state_request;
extern unsigned char pitchmeter_speed;
extern unsigned char pitchmeter_v_location;
extern unsigned char pitchmeter_h_location;

enum Pitchmeter_States{pitchmeter_boot, 
						pitchmeter_clear,
						pitchmeter_wait,
						pitchmeter_vertical,
						pitchmeter_horizontal,
						pitchmeter_hold} sm_pitchmeter_state;

signed char sm_pitchmeter_tick(signed char);

#endif /* PITCHMETER_H_ */