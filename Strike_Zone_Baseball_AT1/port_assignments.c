/*
 * port_assignments.c
 *
 * Created: 5/19/2013 6:22:36 PM
 *  Author: George
 */ 

#include "port_assignments.h"

// Ports and pins for Field Matrix shift registers
// Storage Latch pin
unsigned char* const shift_reg_all_port = &PORTB;
const unsigned char shift_reg_all_st = 1;
//Field LED Matrix
//Green Column
unsigned char* const shift_reg_col_green_port = &PORTB;
const unsigned char shift_reg_col_green_ds = 0;
const unsigned char shift_reg_col_green_sh = 2;
//Red Column
unsigned char* const shift_reg_col_red_port = &PORTB;
const unsigned char shift_reg_col_red_ds = 3;
const unsigned char shift_reg_col_red_sh = 4;
//Row
unsigned char* const shift_reg_row_port = &PORTB;
const unsigned char shift_reg_row_ds = 5;
const unsigned char shift_reg_row_sh = 6;
//Strike Zone LED Matrix
//Row
unsigned char* const zone_row_port  = &PORTD;
const unsigned char zone_row_ds = 2;
const unsigned char zone_row_sh = 3;
//Green Column
unsigned char* const zone_col_green_port  = &PORTD;
const unsigned char zone_col_green_ds = 4;
const unsigned char zone_col_green_sh = 5;
// Red Column
unsigned char* const zone_col_red_port = &PORTD;
const unsigned char zone_col_red_ds = 0;
const unsigned char zone_col_red_sh = 1;

void configure_ports(){
	DDRA = 0xFF;
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
}