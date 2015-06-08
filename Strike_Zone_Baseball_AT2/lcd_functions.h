/*
 * lcd_functions.h
 *
 * Created: 5/24/2013 5:08:03 PM
 *  Author: George
 */ 


#ifndef LCD_FUNCTIONS_H_
#define LCD_FUNCTIONS_H_

#include <avr/io.h>
#include "game_controller.h"

enum LI_States { LI_Init1, LI_Init2, LI_Init3, LI_Init4, LI_Init5, LI_Init6,
	LI_WaitDisplayString, LI_Clr, LI_PositionCursor, LI_DisplayChar } LI_State;

signed char LI_Tick(signed char);

enum lcd_display_mode{ lcd_intro, 
						lcd_select_innings, 
						lcd_score, lcd_count, 
						lcd_last_play, 
						lcd_game_over, 
						lcd_inning, 
						lcd_special,
						lcd_clear} display_mode;
extern unsigned char lcd_display_mode_request;
extern unsigned char LCD_rdy_g;
extern unsigned char lcd_str_special[];

#endif /* LCD_FUNCTIONS_H_ */