/*
 * lcd_functions.c
 *
 * Created: 5/24/2013 5:08:27 PM
 *  Author: George
 */ 

#include "lcd_functions.h"

unsigned char lcd_display_mode_request = 0;
unsigned char current_display_mode = 255;

unsigned char lcd_str_special[] = "special---------";
const char str_sz_baseball[] =    "SZ Baseball     ";
const char str_select_innings[] = "# of Innings    ";
const char str_count[] =          "B:  S:  O:      ";
const char str_score[] =          "Player:   CPU:  ";
const char str_strike_looking[] = "Strike Looking  ";
const char str_strike_swinging[]= "Strike Swinging ";
const char str_foul[] =           "Foul Ball       ";
const char str_ball[] =           "Ball            ";
const char str_walk[] =           "Walk            ";
const char str_single[] =         "Single          ";
const char str_double[] =         "Double          ";
const char str_triple[] =         "Triple          ";
const char str_homerun[] =        "Home Run        ";
const char str_groundout[] =      "Groundout       ";
const char str_flyout[] =         "Flyout          ";
const char str_player_wins[] =    "You Won    -    ";
const char str_player_loses[] =   "You Lost   -    ";
const char str_top_inning[] =     "Top     Inning  ";
const char str_bot_inning[] =     "Bot     Inning  ";

//********* LCD interface synchSM *********************************************

// Define LCD port assignments here so easier to change than if hardcoded below
unsigned char *LCD_Data = &PORTA;	// LCD 8-bit data bus
unsigned char *LCD_Ctrl = &PORTC;	// LCD needs 2-bits for control, use port B
const unsigned char LCD_RS = 7;		// LCD Reset pin is PB4
const unsigned char LCD_E = 6;		// LCD Enable pin is PB5

unsigned char LCD_rdy_g = 0; // Set by LCD interface synchSM, ready to display new string
unsigned char LCD_go_g = 0; // Set by user synchSM wishing to display string in LCD_string_g
unsigned char LCD_string_g[17]; // Filled by user synchSM, 16 chars plus end-of-string char

void LCD_WriteCmdStart(unsigned char cmd) {
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_RS, 0);
	*LCD_Data = cmd;
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 1);
}
void LCD_WriteCmdEnd() {
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 0);
}
void LCD_WriteDataStart(unsigned char Data) {
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_RS,1);
	*LCD_Data = Data;
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 1);
}
void LCD_WriteDataEnd() {
	*LCD_Ctrl = SetBit(*LCD_Ctrl,LCD_E, 0);
}
void LCD_Cursor(unsigned char column ) {
	if ( column < 8 ) { // IEEE change this value to 16
		LCD_WriteCmdStart(0x80+column);
	}
	else {
		LCD_WriteCmdStart(0xB8+column); // IEEE change this value to 0xBF+column
	}
}

signed char LI_Tick(signed char state) {
	LI_State = state;
	static unsigned char i;
	switch(LI_State) { // Transitions
		case -1:
			LI_State = LI_Init1;
			break;
		case LI_Init1:
			LI_State = LI_Init2;
			i=0;
			break;
		case LI_Init2:
			if (i<100) { // Wait 100 ms after power up
				LI_State = LI_Init2;
			}
			else {
				LI_State = LI_Init3;
			}
			break;
		case LI_Init3:
			LI_State = LI_Init4;
			LCD_WriteCmdEnd();
			break;
		case LI_Init4:
			LI_State = LI_Init5;
			LCD_WriteCmdEnd();
			break;
		case LI_Init5:
			LI_State = LI_Init6;
			LCD_WriteCmdEnd();
			break;
		case LI_Init6:
			LI_State = LI_WaitDisplayString;
			LCD_WriteCmdEnd();
			break;
		//////////////////////////////////////////////
		case LI_WaitDisplayString:
			if (current_display_mode == lcd_display_mode_request) {
				LI_State = LI_WaitDisplayString;
			}
			else if (current_display_mode != lcd_display_mode_request) {
			 LCD_rdy_g = 0;
				LI_State = LI_Clr;
			}
			break;
		case LI_Clr:
			LI_State = LI_PositionCursor;
			LCD_WriteCmdEnd();
			i=0;
			break;
		case LI_PositionCursor:
			LI_State = LI_DisplayChar;
			LCD_WriteCmdEnd();
			break;
		case LI_DisplayChar:
			if (i<16) {
				LI_State = LI_PositionCursor;
				LCD_WriteDataEnd();
			i++;
			}
			else {
				LI_State = LI_WaitDisplayString;
				LCD_WriteDataEnd();
			}
			break;
		default:
			LI_State = LI_Init1;
		} // Transitions
	unsigned char tmp_str[17];
	switch(LI_State) { // State actions
		case LI_Init1:
		 LCD_rdy_g = 0;
			break;
		case LI_Init2:
			i++; // Waiting after power up
			break;
		case LI_Init3:
			LCD_WriteCmdStart(0x38);
			break;
		case LI_Init4:
			LCD_WriteCmdStart(0x06);
			break;
		case LI_Init5:
			LCD_WriteCmdStart(0x0F);
			break;
		case LI_Init6:
			LCD_WriteCmdStart(0x01); // Clear
			break;
		//////////////////////////////////////////////
		case LI_WaitDisplayString:
			LCD_rdy_g = 1;
			break;
		case LI_Clr:
			LCD_WriteCmdStart(0x01);
			break;
		case LI_PositionCursor:
			LCD_Cursor(i);			
			break;
		case LI_DisplayChar:
			switch(lcd_display_mode_request){
				case lcd_intro:
					strcpy(LCD_string_g, str_sz_baseball);
					break;
				case lcd_select_innings:
					strcpy(LCD_string_g, str_select_innings);
					break;
				case lcd_count:
					strcpy(tmp_str, str_count);
					tmp_str[2] = (char) (balls + 48);
					tmp_str[6] = (char) (strikes + 48);
					tmp_str[10] = (char) (outs + 48);
					strcpy(LCD_string_g, tmp_str);
					break;
				case lcd_last_play:
					switch(last_play_result){
						case last_ball:
							strcpy(LCD_string_g, str_ball);
							break;
						case last_walk:
							strcpy(LCD_string_g, str_walk);
							break;
						case last_strike_looking:
							strcpy(LCD_string_g, str_strike_looking);
							break;
						case last_strike_swinging:
							strcpy(LCD_string_g, str_strike_swinging);
							break;
						case last_foul:
							strcpy(LCD_string_g, str_foul);
							break;
						case last_single:
							strcpy(LCD_string_g, str_single);
							break;
						case last_double:
							strcpy(LCD_string_g, str_double);
							break;
						case last_triple:
							strcpy(LCD_string_g, str_triple);
							break;
						case last_homerun:
							strcpy(LCD_string_g, str_homerun);
							break;
						case last_groundout:
							strcpy(LCD_string_g, str_groundout);
							break;
						case last_flyout:
							strcpy(LCD_string_g, str_flyout);
							break;
						default:
							strcpy(LCD_string_g, "ERROR           ");
							break;
					}
					break;
				case lcd_score:
					strcpy(tmp_str, str_score);
					tmp_str[7] = (char) (player_score + (int) '0');
					tmp_str[14] = (char) (cpu_score + (int) '0');
					strcpy(LCD_string_g, tmp_str);
					break;
				case lcd_game_over:
					if(player_score > cpu_score){
						strcpy(tmp_str, str_player_wins);
						tmp_str[9] = (char) (player_score + (int) '0');
						tmp_str[13] = (char) (cpu_score + (int) '0');
						strcpy(LCD_string_g, tmp_str);
						
					} else if(player_score < cpu_score){
						strcpy(tmp_str, str_player_loses);
						tmp_str[9] = (char) (player_score + (int) '0');
						tmp_str[13] = (char) (cpu_score + (int) '0');
						strcpy(LCD_string_g, tmp_str);
					} else {
						strcpy(LCD_string_g, "TIE             ");
					}
					break;
				case lcd_inning:
					if(top_bot_innning){
						strcpy(tmp_str, str_bot_inning);
						tmp_str[4] = (char) (inning + (int) '0');
						strcpy(LCD_string_g, tmp_str);
					} else {
						strcpy(tmp_str, str_top_inning);
						tmp_str[4] = (char) (inning + (int) '0');
						strcpy(LCD_string_g, tmp_str);
					}
					break;
				case lcd_special:
					strcpy(LCD_string_g, lcd_str_special);
					break;
				case lcd_clear:
					break;
				default:
					strcpy(LCD_string_g, "ERROR           ");
					break;
			}
			current_display_mode = lcd_display_mode_request;
			LCD_WriteDataStart(LCD_string_g[i]);
			break;
		default:
			break;
	} // State actions
	return LI_State;
}
//--------END LCD interface synchSM------------------------------------------------