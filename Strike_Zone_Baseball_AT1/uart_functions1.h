/*
 * uart_functions.h
 *
 * Created: 5/19/2013 6:37:55 PM
 *  Author: George
 */ 


#ifndef UART_FUNCTIONS_H_
#define UART_FUNCTIONS_H_

#include <avr/io.h>

enum Uart_SM_States{uart_boot,
					uart_init,
					uart_ready_data1,
					uart_wait_for_data1,
					uart_ready_data2,
					uart_wait_for_data2,
					uart_ready_data3,
					uart_wait_for_data3,
					uart_update_matrices} uart_sm_state;

unsigned char uart_tick(unsigned char);

#endif /* UART_FUNCTIONS_H_ */