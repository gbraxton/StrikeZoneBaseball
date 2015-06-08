/*
 * Strike_Zone_Baseball_AT1_v005.c
 *
 * Created: 6/5/2013 9:03:05 PM
 *  Author: George
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "port_assignments.h"
#include "field_matrix.h"
#include "uart_functions1.h"
#include "sound_controller.h"

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR0
	TCCR0 = 0x0B;
	
	// AVR output compare register OCR0.
	OCR0 = 125;
	
	TIMSK = 1 << OCIE0; // bit1: OCIE0 -- enables compare match interrupt

	//Initialize avr counter
	TCNT0 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR0 = 0x00; // bit2bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER0_COMP_vect) {
	// CPU automatically calls when TCNT0 == OCR0 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b){
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

int main(void)
{
	configure_ports();
	init_PWM();
	
	// Tasks
	
	// Period for the tasks
	unsigned long int SMTick_Field_Matrix_calc = 1;
	unsigned long int SMTick_Uart_calc = 2;
	unsigned long int SMTick_Sound_calc = 15;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick_Field_Matrix_calc, SMTick_Uart_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_Sound_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick_Field_Matrix_period = SMTick_Field_Matrix_calc/GCD;
	unsigned long int SMTick_Uart_period = SMTick_Uart_calc/GCD;
	unsigned long int SMTick_Sound_period = SMTick_Sound_calc/GCD;

	//Declare an array of tasks
	static task task_Field_Matrix, task_Uart, task_Sound;
	task *tasks[] = { &task_Field_Matrix, &task_Uart, &task_Sound };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task Field_Matrix
	task_Field_Matrix.state = Field_matrix_boot;//Task initial state.
	task_Field_Matrix.period = SMTick_Field_Matrix_period;//Task Period.
	task_Field_Matrix.elapsedTime = SMTick_Field_Matrix_period;//Task current elapsed time.
	task_Field_Matrix.TickFct = &field_matrix_tick;//Function pointer for the tick.
	// Task Uart
	task_Uart.state = uart_boot;//Task initial state.
	task_Uart.period = SMTick_Uart_period;//Task Period.
	task_Uart.elapsedTime = SMTick_Uart_period;//Task current elapsed time.
	task_Uart.TickFct = &uart_tick;//Function pointer for the tick.
	// Task Sound
	task_Sound.state = sm_sound_boot;
	task_Sound.period = SMTick_Sound_period;
	task_Sound.elapsedTime = SMTick_Sound_period;
	task_Sound.TickFct = &sm_sound_controller_tick;
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
    while(1)
    {
		unsigned char i;
        // Scheduler code
        for ( i = 0; i < numTasks; i++ ) {
	        // Task is ready to tick
	        if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
		        // Setting next state for task
		        tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
		        // Reset the elapsed time for next tick.
		        tasks[i]->elapsedTime = 0;
	        }
	        tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
}