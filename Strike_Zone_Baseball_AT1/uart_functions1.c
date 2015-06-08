/*
 * uart_functions.c
 *
 * Created: 5/19/2013 6:38:18 PM
 *  Author: George
 */ 

#include "uart_functions1.h"

// Clk frequency and baudrate constants
const unsigned long CLK_FREQUENCY = 8000000;
const unsigned long BAUDRATE = 14400;

unsigned char rx_data_byte1;
unsigned char rx_data_byte2;
unsigned char rx_data_byte3;

void initialize_uart(){
	// Set baudrate
	unsigned short tmp_ubrr = (CLK_FREQUENCY/((unsigned long) 16 * BAUDRATE)) - 1;
	UBRRH = (unsigned char) (tmp_ubrr >> 8);
	UBRRL = (unsigned char) tmp_ubrr;
	// Enable rx register
	UCSRB = (1 << RXEN) | (1 << TXEN);
	// Set frame format 
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

unsigned char uart_tick(unsigned char state){
	uart_sm_state = state;
	// uart sm transitions
	switch(uart_sm_state){
		case uart_boot:
			uart_sm_state = uart_init;
			break;
		case uart_init:
			uart_sm_state = uart_ready_data1;
			break;
		case uart_ready_data1:
			if(UCSRA & (1 << UDRE)){
				uart_sm_state = uart_wait_for_data1;
				UDR = 1;
			} else {
				uart_sm_state = uart_ready_data1;
			}
			break;
		case uart_wait_for_data1:
			if( UCSRA & (1 << RXC) ){
				uart_sm_state = uart_ready_data2;
				rx_data_byte1 = UDR;
			} else if( !(UCSRA & (1 << RXC)) ){
				uart_sm_state = uart_wait_for_data1;
			}
			break;
		case uart_ready_data2:
			if(UCSRA & (1 << UDRE)){
				uart_sm_state = uart_wait_for_data2;
				UDR = 1;
			} else {
				uart_sm_state = uart_ready_data2;
			}
			break;
		case uart_wait_for_data2:
			if(UCSRA & (1 << RXC)){
				uart_sm_state = uart_ready_data3;
				rx_data_byte2 = UDR;
			} else if( !(UCSRA & (1 << RXC)) ){
				uart_sm_state = uart_wait_for_data2;
			}
			break;
		case uart_ready_data3:
			if(UCSRA & (1 << UDRE)){
				uart_sm_state = uart_wait_for_data3;
				UDR = 1;
			} else {
				uart_sm_state = uart_ready_data3;
			}
			break;
		case uart_wait_for_data3:
			if(UCSRA & (1 << RXC)){
				uart_sm_state = uart_update_matrices;
				rx_data_byte3 = UDR;
			} else if( !(UCSRA & (1 << RXC)) ){
				uart_sm_state = uart_wait_for_data3;
			}
			break;
		case uart_update_matrices:
			uart_sm_state = uart_ready_data1;
			break;
	}
	// uart sm actions
	switch(uart_sm_state){
		case uart_init:
			initialize_uart();
			break;
		case uart_ready_data1:
			break;
		case uart_wait_for_data1:
			break;
		case uart_ready_data2:
			break;
		case uart_wait_for_data2:
			break;
		case uart_ready_data3:
			break;
		case uart_wait_for_data3:
			break;
		case uart_update_matrices:
			update_position( rx_data_byte1, (((unsigned short) rx_data_byte2 << 8) | (unsigned short) rx_data_byte3) );
			break;
	}
	return uart_sm_state;
}