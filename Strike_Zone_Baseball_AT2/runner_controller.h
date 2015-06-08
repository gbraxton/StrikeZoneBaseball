/*
 * runner_controller.h
 *
 * Created: 5/27/2013 3:03:47 PM
 *  Author: George
 */ 


#ifndef RUNNER_CONTROLLER_H_
#define RUNNER_CONTROLLER_H_

#include "game_controller.h"

extern unsigned char runner_on_1B;
extern unsigned char runner_on_2B;
extern unsigned char runner_on_3B;

extern unsigned char runner_state_request;
extern unsigned long runner_action_request; // Format for each runner [4 bits: to-base] !batter last

enum runner_bases_index{ runner_base_index_1B = 5, runner_base_index_2B = 10, runner_base_index_3B = 15, runner_base_index_H = 20, runner_base_index_clear = 21 };

enum Runners{runner_batter, runner_runner1, runner_runner2, runner_runner3} runners;

enum SM_Runner_States{ sm_runner_boot, 
						sm_runner_clear, 
						sm_runner_batterup, 
						sm_runner_wait,
						sm_runner_prepare0,
						sm_runner_prepare1, 
						sm_runner_prepare2, 
						sm_runner_prepare3, 
						sm_runner_prepare4,
						sm_runner_update1,
						sm_runner_update2,
						sm_runner_update3,
						sm_runner_update4,
						sm_runner_wait_pace, 
						sm_runner_sequence } sm_runner_state;

signed char sm_runner_tick(signed char);
void clear_runners();

#endif /* RUNNER_CONTROLLER_H_ */