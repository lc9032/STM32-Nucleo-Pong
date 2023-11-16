/*
 * timer.h
 * This timer module can be used to multiplex a single hardware timer into
 * multiple software timers (i.e. periodic function calls). The number of
 * available software timers is limited to TIMER_MAX_TIMERS.
 * This module must be initialized by calling timer_init() before any other
 * usage. Then, software timers (callbacks) can be registered with the function
 * timer_register(timerfp, div). The function timer_tick() must be called with
 * the frequency TIMER_TICKRATE_HZ from a hardware timer or any other reliable timing
 * source.
 *
 *  Created on: 08.11.2021
 *      Author: Timm Bostelmann
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/***Constants******************************************************************/
#define TIMER_TICKRATE_HZ (1000)
#define TIMER_MAX_TIMERS (8)
/******************************************************************************/

/***Types**********************************************************************/
typedef void (*timer_fp_t)(void);
/******************************************************************************/

/***Functions******************************************************************/

/*
 * Initialize the timer module.
 * Must be called before any other usage of the module.
 * All previously registered timers are deleted!
 *
 * Return: 0 for success, others for failure
 */
extern int timer_init(void);

/*
 * Register a new timer function.
 * The new timer function "timerfp" is called with a frequency of
 * "TIMER_TICKRATE_HZ" / "div".
 * @param timerfp Pointer to the timer function that is to be called
 * @param div Clock divider f_out = f_in / div
 * Return: 0 for success, others for failure
 */
extern int timer_register(timer_fp_t timerfp, uint32_t div);

/**
 * The timer_func_divider_update function updates the divider value for a registered timer function.
 * The divider value determines how many ticks the timer must count before calling the timer function.
 * 
 * @param timerfp: A function pointer to the timer function to update the divider value for.
 * @param div: The new divider value for the timer function.
 * Return: 0 if the divider value was successfully updated,
 *         1 if the timer function was not found in the list of registered timer functions.
 */
extern int timer_func_divider_update(timer_fp_t timerfp, uint32_t div);

/*
 * Process next tick.
 * Must be called periodically with a frequency of TIMER_TICKRATE_HZ
 */
extern void timer_tick(void);
/******************************************************************************/

#endif /* TIMER_H_ */
