/*
 * port_assignments.h
 *
 * Created: 5/19/2013 1:28:48 AM
 *  Author: George
 */ 

#include <avr/io.h>

#ifndef PORT_ASSIGNMENTS_H_
#define PORT_ASSIGNMENTS_H_

// Ports and pins for shift registers
//Storage Latch pin for all shift registers
extern unsigned char* const shift_reg_all_port;
extern const unsigned char shift_reg_all_st;

// Field LED Matrix
//Green Column
extern unsigned char* const shift_reg_col_green_port;
extern const unsigned char shift_reg_col_green_ds;
extern const unsigned char shift_reg_col_green_sh;
//Red Column
extern unsigned char* const shift_reg_col_red_port;
extern const unsigned char shift_reg_col_red_ds;
extern const unsigned char shift_reg_col_red_sh;
//Row
extern unsigned char* const shift_reg_row_port;
extern const unsigned char shift_reg_row_ds;
extern const unsigned char shift_reg_row_sh;

//Strike Zone LED Matrix
//Green Column
extern unsigned char* const zone_col_green_port;
extern const unsigned char zone_col_green_ds;
extern const unsigned char zone_col_green_sh;
//Red Column
extern unsigned char* const zone_col_red_port;
extern const unsigned char zone_col_red_ds;
extern const unsigned char zone_col_red_sh;
//Row
extern unsigned char* const zone_row_port;
extern const unsigned char zone_row_ds;
extern const unsigned char zone_row_sh;

void configure_ports();

#endif /* PORT_ASSIGNMENTS_H_ */