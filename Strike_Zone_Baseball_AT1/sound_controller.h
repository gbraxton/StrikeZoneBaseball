/*
 * sound_controller.h
 *
 * Created: 6/6/2013 10:19:22 PM
 *  Author: George
 */ 


#ifndef SOUND_CONTROLLER_H_
#define SOUND_CONTROLLER_H_

#include <avr/io.h>

enum Sounds{sound_intro, sound_ball, sound_strike, sound_out, sound_hit, sound_homerun};

enum SM_Sound_Controller_States{sm_sound_boot, sm_sound_wait, sm_sound_play, sm_sound_flight} sm_sound_controller_state;
	
extern unsigned char sound_request;
extern unsigned char sound_state_request;
extern unsigned short sound_flight_time_request;
signed char sm_sound_controller_tick(signed char);

#endif /* SOUND_CONTROLLER_H_ */