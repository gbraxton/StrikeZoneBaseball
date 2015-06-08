/*
 * keypad_functions.c
 *
 * Created: 5/24/2013 5:00:49 PM
 *  Author: George
 */ 


#include "keypad_functions.h"
#include "bit.h"

unsigned char current_key = '\0';

unsigned char GetKeypadKey() {

	PORTB = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTB to stabilize before checking
if (GetBit(PINB,0)==0) { return('1'); }
	if (GetBit(PINB,1)==0) { return('4'); }
	if (GetBit(PINB,2)==0) { return('7'); }
	if (GetBit(PINB,3)==0) { return('*'); }

	// Check keys in col 2
	PORTB = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTB to stabilize before checking
	if (GetBit(PINB,0)==0) { return('2'); }
	if (GetBit(PINB,1)==0) { return('5'); }
	if (GetBit(PINB,2)==0) { return('8'); }
	if (GetBit(PINB,3)==0) { return('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTB = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTB to stabilize before checking
	if (GetBit(PINB,0)==0) { return('3'); }
	if (GetBit(PINB,1)==0) { return('6'); }
	if (GetBit(PINB,2)==0) { return('9'); }
	if (GetBit(PINB,3)==0) { return('#'); }
	// ... *****FINISH*****

	// Check keys in col 4	
	PORTB = 0x7F;
	asm("nop");
	if (GetBit(PINB,0)==0) { return('A'); }
	if (GetBit(PINB,1)==0) { return('B'); }
	if (GetBit(PINB,2)==0) { return('C'); }
	if (GetBit(PINB,3)==0) { return('D'); }
	// ... *****FINISH*****

	return '\0'; // default value

}

unsigned long current_key_on_count = 0;

int sm_keypad_tick(int state){
	sm_keypad_state = state;
	switch(sm_keypad_state){//transitions
		case sm_keypad_wait_for_press:
			if(GetKeypadKey() == '\0'){
				sm_keypad_state = sm_keypad_wait_for_press;
				current_key = '\0';
			} else {
				sm_keypad_state = sm_keypad_current_key;
				current_key_on_count = 0;
				current_key = GetKeypadKey();
			}			
			break;
		case sm_keypad_current_key:
			if(current_key_on_count < 10){
				sm_keypad_state = sm_keypad_current_key;
			} else {
				sm_keypad_state = sm_keypad_wait_for_press;
			}
			break;
	}
	switch(sm_keypad_state){//actions
		case sm_keypad_wait_for_press:
			break;
		case sm_keypad_current_key:
			current_key_on_count++;
			break;
	}
	return sm_keypad_state;
}