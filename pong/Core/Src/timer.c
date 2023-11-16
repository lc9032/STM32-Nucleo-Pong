/*
 * timer.c
 *
 */
#include "main.h"
#include "timer.h"
#include "stm32f4xx_hal_tim.h"
#include <string.h>
#include "uart.h"

typedef struct timerFunctionReg timerFunctionReg;
struct timerFunctionReg
{
	timer_fp_t funcPointer;
	uint16_t divider;
	uint16_t counter;
};

timerFunctionReg timerFuncReg[TIMER_MAX_TIMERS];

uint8_t funcNum;

int timer_init(void)
{
	for (int i = 0;i < TIMER_MAX_TIMERS;i++)
	{
		timerFuncReg[i].funcPointer = NULL;
		timerFuncReg[i].divider = 0;
		timerFuncReg[i].counter = 0;
	}
	return 0;
}

int timer_register(timer_fp_t timerfp, uint32_t div)
{
	if (funcNum < TIMER_MAX_TIMERS)
	{
		timerFuncReg[funcNum].funcPointer = timerfp;
		timerFuncReg[funcNum].divider = div;
		funcNum++;
		return 0;
	}
	else
	{
		return 1;
	}
}

int timer_func_divider_update(timer_fp_t timerfp, uint32_t div)
{
	for(int i = 0;i < funcNum;i++)
	{
		if(timerFuncReg[i].funcPointer == timerfp)
		{
			timerFuncReg[i].divider = div;
			return 0;
		}
	}
	return 1;
}

void timer_tick(void)
{
	// Iterate through the list of registered timer functions
	for(int i = 0;i < funcNum;i++)
	{
		// Increment the counter for this timer function
		timerFuncReg[i].counter++;

		// If the counter has reached the specified divider value, 
        // execute the timer function and reset the counter
		if((timerFuncReg[i].counter >= (timerFuncReg[i].divider)))
		{
			timerFuncReg[i].funcPointer();
			timerFuncReg[i].counter = 0;
		}
	}
}
