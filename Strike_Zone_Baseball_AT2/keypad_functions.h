/*
 * keypad_functions.h
 *
 * Created: 5/24/2013 5:00:30 PM
 *  Author: George
 */ 


#ifndef KEYPAD_FUNCTIONS_H_
#define KEYPAD_FUNCTIONS_H_

#include <avr/io.h>


enum SM_Keypad_States{sm_keypad_wait_for_press, sm_keypad_current_key} sm_keypad_state;
extern unsigned char current_key;

int sm_keypad_tick(int);


#endif /* KEYPAD_FUNCTIONS_H_ */