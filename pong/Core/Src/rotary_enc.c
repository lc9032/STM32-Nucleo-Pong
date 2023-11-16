#include "main.h"
#include "rotary_enc.h"
#include "app.h"
#include "timer.h"
#include "stm32f4xx_hal_gpio_ex.h"
#include "stm32f4xx_hal.h"
#include <limits.h>

#define BTN1_PIN GPIO_PIN_8
#define ENC1_A_PIN GPIO_PIN_7
#define ENC1_B_PIN GPIO_PIN_6

#define BTN2_PIN GPIO_PIN_4
#define ENC2_A_PIN GPIO_PIN_0
#define ENC2_B_PIN GPIO_PIN_1

enum btnState btn1;
enum btnState btn2;

int32_t enc1_count;
static int32_t enc1;
static uint8_t enc1_old_state;
static uint8_t enc1_new_state;

int32_t enc2_count;
static int32_t enc2;
static uint8_t enc2_old_state;
static uint8_t enc2_new_state;

static int32_t enc_max;
static int32_t enc_min;


/**
 * refer to STM32 Rotary Encoder - Stm32World Wiki
 * Link: https://stm32world.com/wiki/STM32_Rotary_Encoder
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case BTN1_PIN:
			if (HAL_GPIO_ReadPin(GPIOA, BTN1_PIN)) btn1 = RELEASED;
			else btn1 = PRESSED;
			break;
		case BTN2_PIN:
			if (HAL_GPIO_ReadPin(GPIOA, BTN2_PIN)) btn2 = RELEASED;
			else btn2 = PRESSED;
			break;
		case ENC1_A_PIN:
		case ENC1_B_PIN:
			enc1_new_state = (uint8_t)((HAL_GPIO_ReadPin(GPIOA, ENC1_B_PIN) << 1)
										| (HAL_GPIO_ReadPin(GPIOA, ENC1_A_PIN)));

			if ((enc1_old_state == 3 && enc1_new_state == 2)
				|| (enc1_old_state == 2 && enc1_new_state == 0)
				|| (enc1_old_state == 0 && enc1_new_state == 1)
				|| (enc1_old_state == 1 && enc1_new_state == 3))
			{
				if (enc1 < enc_max*4) enc1++;
			}
			else if ((enc1_old_state == 3 && enc1_new_state == 1)
				|| (enc1_old_state == 1 && enc1_new_state == 0)
				|| (enc1_old_state == 0 && enc1_new_state == 2)
				|| (enc1_old_state == 2 && enc1_new_state == 3))
			{
				if (enc1 > enc_min*4) enc1--;
			}
			enc1_count = enc1/4;
			enc1_old_state = enc1_new_state;
			break;
		case ENC2_A_PIN:
		case ENC2_B_PIN:
			enc2_new_state = (uint8_t)((HAL_GPIO_ReadPin(GPIOA, ENC2_B_PIN) << 1)
										| (HAL_GPIO_ReadPin(GPIOA, ENC2_A_PIN)));

			if ((enc2_old_state == 3 && enc2_new_state == 2)
				|| (enc2_old_state == 2 && enc2_new_state == 0)
				|| (enc2_old_state == 0 && enc2_new_state == 1)
				|| (enc2_old_state == 1 && enc2_new_state == 3))
			{
				if (enc2 < enc_max*4) enc2++;
			}
			else if ((enc2_old_state == 3 && enc2_new_state == 1)
				|| (enc2_old_state == 1 && enc2_new_state == 0)
				|| (enc2_old_state == 0 && enc2_new_state == 2)
				|| (enc2_old_state == 2 && enc2_new_state == 3))
			{
				if (enc2 > enc_min*4) enc2--;
			}
			enc2_count = enc2/4;
			enc2_old_state = enc2_new_state;
			break;
		default:
			break;
	}
}

void set_enc_max_min(int32_t max, int32_t min){
	enc_max = max;
	enc_min = min;
}

uint8_t set_enc1_val(int32_t value)
{
	if ((value <= (enc_max)) && (value >= (enc_min)))
	{
		enc1 = value*4;
		enc1_count = value;
		return 0;
	}
	else
	{
		return 1;
	}
}

uint8_t set_enc2_val(int32_t value)
{
	if ((value <= (enc_max)) && (value >= (enc_min)))
	{
		enc2 = value*4;
		enc2_count = value;
		return 0;
	}
	else
	{
		return 1;
	}
}

void rotary_enc_init()
{
	enc1 = 0;
	enc1_count = 0;
	enc_max = INT_MAX;
	enc_min = INT_MIN;
	btn1 = RELEASED;
	btn2 = RELEASED;
	enc1_old_state = 3;
	enc1_new_state = 3;
}