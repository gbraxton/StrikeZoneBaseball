/*
 * uart_functions.h
 *
 * Created: 5/24/2013 8:07:08 AM
 *  Author: George
 */ 


#ifndef UART_FUNCTIONS_H_
#define UART_FUNCTIONS_H_

#include <avr/io.h>

#include "game_controller.h"
//Global Data
extern unsigned char uart_data_byte;

enum Uart_SM_States{uart_boot,
uart_init,
uart_ready,
uart_wait_to_send1,
uart_sending1,
uart_wait_to_send2,
uart_sending2,
uart_wait_to_send3,
uart_sending3 } uart_sm_state;

unsigned char uart_tick(unsigned char);
void initialize_uart();
void uart_push_message(unsigned long);

#endif /* UART_FUNCTIONS_H_ */