/*
 * Strike_Zone_Baseball_AT2_v005.c
 *
 * Created: 6/5/2013 9:09:06 PM
 *  Author: George
 */ 


#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "game_controller.h"

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR0
	TCCR0 = 0x0B;

	// AVR output compare register OCR0.
	OCR0 = 125;
	
	TIMSK = 0x02; // bit1: OCIE0 -- enables compare match interrupt

	//Initialize avr counter
	TCNT0=0;

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
	DDRA = 0xFF;
	DDRB = 0xF0; PORTB = 0x0F;
	DDRC = 0xFF;
	DDRD = 0xFF;
	initialize_uart();
	
	// Tasks
	// Period for the tasks
	unsigned long int SMTick_Uart_calc = 2;
	unsigned long int SMTick_ball_calc = 20;
	unsigned long int SMTick_keypad_calc = 20;
	unsigned long int SMTick_game_calc = 20;
	unsigned long int SMTick_lcd_calc = 20;
	unsigned long int SMTick_runner_calc = 20;
	unsigned long int SMTick_pitchmeter_calc = 20;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick_ball_calc, SMTick_Uart_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_keypad_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_game_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_lcd_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_runner_calc);
	tmpGCD = findGCD(tmpGCD, SMTick_pitchmeter_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick_Uart_period = SMTick_Uart_calc/GCD;
	unsigned long int SMTick_ball_period = SMTick_ball_calc/GCD;
	unsigned long int SMTick_keypad_period = SMTick_keypad_calc/GCD;
	unsigned long int SMTick_game_period = SMTick_game_calc/GCD;
	unsigned long int SMTick_lcd_period = SMTick_lcd_calc/GCD;
	unsigned long int SMTick_runner_period = SMTick_runner_calc/GCD;
	unsigned long int SMTick_pitchmeter_period = SMTick_pitchmeter_calc/GCD;

	//Declare an array of tasks
	static task task_Uart, task_ball, task_keypad, task_game, task_lcd, task_runner, task_pitchmeter;
	task *tasks[] = { &task_game, &task_ball, &task_runner, &task_lcd, &task_Uart, &task_keypad, &task_pitchmeter};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task LCD
task_lcd.state = -1;
task_lcd.period = SMTick_lcd_period;
task_lcd.elapsedTime = SMTick_lcd_period;
task_lcd.TickFct = &LI_Tick;
// Task Pitchmeter
task_pitchmeter.state = pitchmeter_boot;
task_pitchmeter.period = SMTick_pitchmeter_period;
task_pitchmeter.elapsedTime = SMTick_pitchmeter_period;
task_pitchmeter.TickFct = &sm_pitchmeter_tick;
// Task Game Controller
task_game.state = sm_game_boot;
task_game.period = SMTick_game_period;
task_game.elapsedTime = SMTick_game_period;
task_game.TickFct = &sm_tick_game;
// Task Uart
task_Uart.state = uart_boot;//Task initial state.
task_Uart.period = SMTick_Uart_period;//Task Period.
task_Uart.elapsedTime = SMTick_Uart_period;//Task current elapsed time.
task_Uart.TickFct = &uart_tick;//Function pointer for the tick.
// Task Ball
task_ball.state = sm_ball_boot;//Task initial state.
task_ball.period = SMTick_ball_period;//Task Period.
task_ball.elapsedTime = SMTick_ball_period;//Task current elapsed time.
task_ball.TickFct = &sm_tick_ball;//Function pointer for the tick.

// Task Keypad
task_keypad.state = sm_keypad_wait_for_press;//Task initial state.
task_keypad.period = SMTick_keypad_period;//Task Period.
task_keypad.elapsedTime = SMTick_keypad_period;//Task current elapsed time.
task_keypad.TickFct = &sm_keypad_tick;//Function pointer for the tick.

// Task Runner
task_runner.state = sm_runner_boot;
task_runner.period = SMTick_runner_period;
task_runner.elapsedTime = SMTick_runner_period;
task_runner.TickFct = &sm_runner_tick;

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