/*
 * uart_functions.c
 *
 * Created: 5/24/2013 8:09:55 AM
 *  Author: George
 */ 

#include "uart_functions2.h"


unsigned long uart_send_queue[40];
unsigned char uart_send_queue_cnt = 0;
unsigned char data_byte1 = 0;
unsigned char data_byte2 = 0;
unsigned char data_byte3 = 0;
unsigned char discard_byte = 0;


// Clk frequency and baudrate constants
const unsigned long CLK_FREQUENCY = 8000000;
const unsigned long BAUDRATE = 14400;

unsigned char queue_full(){
	return (uart_send_queue_cnt == 80);
}

unsigned char queue_empty(){
	return (uart_send_queue_cnt == 0);
}

void uart_push_message(unsigned long data){
	if(!queue_full()){
		uart_send_queue[uart_send_queue_cnt] = data;
		uart_send_queue_cnt++;
	}
}

unsigned long pop_message(){
	if(!queue_empty()){
		unsigned long tmp_message = uart_send_queue[0];
		uart_send_queue_cnt--;
		unsigned char i;
		for(i = 0; i < uart_send_queue_cnt; i++){
			uart_send_queue[i] = uart_send_queue[i + 1];
		}
		return tmp_message;
	}
	return 0;
}

void initialize_uart(){
	// Set baudrate
	unsigned short tmp_ubrr = (CLK_FREQUENCY/(16 * BAUDRATE)) - 1;
	UBRRH = (unsigned char) tmp_ubrr >> 8;
	UBRRL = (unsigned char) tmp_ubrr;
	// Enable tx register
	UCSRB = (1 << TXEN) | (1 << RXEN);
	// Set frame format
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

unsigned char uart_tick(unsigned char state){
	uart_sm_state = state;
	unsigned long tmp_data;
	// uart sm transitions
	switch(uart_sm_state){
		case uart_boot:
			uart_sm_state = uart_init;
			break;
		case uart_init:
			uart_sm_state = uart_ready;
			break;
		case uart_ready:
			if( (!queue_empty()) && (UCSRA & (1 << RXC))){
				discard_byte = UDR;
				uart_sm_state = uart_wait_to_send1;
			} else {
				uart_sm_state = uart_ready;
			}
			break;
		case uart_wait_to_send1:
			if(UCSRA & (1 << UDRE)){
				uart_sm_state = uart_sending1;
			} else {
				uart_sm_state = uart_wait_to_send1;
			}
			break;
		case uart_sending1:
			uart_sm_state = uart_wait_to_send2;
			break;
		case uart_wait_to_send2:
			if( (UCSRA & (1 << UDRE)) && (UCSRA & (1 << RXC)) ){
				discard_byte = UDR;
				uart_sm_state = uart_sending2;
			} else {
				uart_sm_state = uart_wait_to_send2;
			}
			break;
		case uart_sending2:
			uart_sm_state = uart_wait_to_send3;
			break;
		case uart_wait_to_send3:
			if( (UCSRA & (1 << UDRE)) && (UCSRA & (1 << RXC)) ){
				discard_byte = UDR;
				uart_sm_state = uart_sending3;
			} else {
				uart_sm_state = uart_wait_to_send3;
			}
			break;
		case uart_sending3:
			uart_sm_state = uart_ready;
			break;
	}
	// uart sm actions
	switch(uart_sm_state){
		case uart_init:
			initialize_uart();
			break;
		case uart_ready:
			break;
		case uart_wait_to_send1:
			break;
		case uart_sending1:
			tmp_data = pop_message();
			data_byte1 = (tmp_data & 0x00FF0000) >> 16;
			data_byte2 = (tmp_data & 0x0000FF00) >> 8;
			data_byte3 = tmp_data & 0x000000FF;
			UDR = data_byte1;
			break;
		case uart_wait_to_send2:
			break;
		case uart_sending2:
			UDR = data_byte2;
			break;
		case uart_wait_to_send3:
			break;
		case uart_sending3:
			UDR = data_byte3;
			break;
	}
	return uart_sm_state;
}