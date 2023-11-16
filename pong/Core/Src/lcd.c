#include<stdbool.h>
#include <math.h>
#include "main.h"
#include "lcd.h"
#include "uart.h"
#include "stm32f4xx_ll_gpio.h"
#include "timer.h"
#include "font/4x6_horizontal_LSB_2.h"

#define FONT_COLUMNS 4
#define FONT_ROWS 6

#define LCD_GPIO_Port GPIOC
#define DB0_Pin GPIO_PIN_0
#define DB1_Pin GPIO_PIN_1
#define DB2_Pin GPIO_PIN_2
#define DB3_Pin GPIO_PIN_3
#define DB4_Pin GPIO_PIN_4
#define DB5_Pin GPIO_PIN_5
#define DB6_Pin GPIO_PIN_6
#define DB7_Pin GPIO_PIN_7
#define CS1_Pin GPIO_PIN_8
#define CS2_Pin GPIO_PIN_9
#define RST_Pin GPIO_PIN_10
#define RW_Pin GPIO_PIN_11
#define DI_Pin GPIO_PIN_12
#define EN_Pin GPIO_PIN_13

extern TIM_HandleTypeDef htim1;

static bool lcd_dsp_buffer[LCD_COLUMNS][LCD_ROWS];

/**
 * refer to Microsecond/Nanoseconds delay in STM32 » ControllersTech
 * Link: https://controllerstech.com/create-1-microsecond-delay-stm32/
 */
static void delay_12_nanosec(int dlytime)
{
	__HAL_TIM_SetCounter(&htim1, 0);
	while(__HAL_TIM_GetCounter(&htim1) < dlytime);
}

static void lcd_reset(void)
{
	HAL_GPIO_WritePin(LCD_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LCD_GPIO_Port, RST_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

static void lcd_write(bool cs1, bool cs2, bool di, uint8_t data)
{
	HAL_GPIO_WritePin(LCD_GPIO_Port, EN_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_Port, RW_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_GPIO_Port, CS1_Pin, cs1);
	HAL_GPIO_WritePin(LCD_GPIO_Port, CS2_Pin, cs2);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DI_Pin, di);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB7_Pin, (data & 0b10000000) >> 7);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB6_Pin, (data & 0b01000000) >> 6);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB5_Pin, (data & 0b00100000) >> 5);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB4_Pin, (data & 0b00010000) >> 4);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB3_Pin, (data & 0b00001000) >> 3);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB2_Pin, (data & 0b00000100) >> 2);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB1_Pin, (data & 0b00000010) >> 1);
	HAL_GPIO_WritePin(LCD_GPIO_Port, DB0_Pin, (data & 0b00000001));

	delay_12_nanosec(26);//t_EWL - T_AS1 min 450 - 140 = 310ns

	HAL_GPIO_WritePin(LCD_GPIO_Port, RW_Pin, GPIO_PIN_RESET);

	delay_12_nanosec(12);//t_AS1 min 140ns

	HAL_GPIO_WritePin(LCD_GPIO_Port, EN_Pin, GPIO_PIN_SET);

	delay_12_nanosec(38);//t_EWH min 450ns

	HAL_GPIO_WritePin(LCD_GPIO_Port, EN_Pin, GPIO_PIN_RESET);

	delay_12_nanosec(1);//t_AH1 min 10ns

	HAL_GPIO_WritePin(LCD_GPIO_Port, RW_Pin, GPIO_PIN_SET);

	delay_12_nanosec(1);
}

static void lcd_dsp_on_off(bool cs1, bool cs2, bool on_off)
{
	lcd_write(cs1, cs2, 0, (0b00111110 | on_off));
}

static void lcd_write_addr(uint8_t page, uint8_t column, uint8_t data)
{
	bool cs;

	// Determine which chip select line to use based on the column number
	if (column < 64) cs = false;
	else cs = true;

	// Send commands and data to the LCD screen
	lcd_write(!cs, cs, 0, 0b10111000 | page);
	lcd_write(!cs, cs, 0, 0b01000000 | column);
	lcd_write(!cs, cs, 1, data);
}

void lcd_clear()
{
	for(uint8_t col = 0;col < LCD_COLUMNS;col++)
	{
		for(uint8_t row = 0;row < LCD_ROWS;row++)
		{
			lcd_dsp_buffer[col][row] = 0;
		}
	}
}

void lcd_draw_clear_rect(uint8_t posX, uint8_t posY, uint8_t length, uint8_t width, bool data)
{
	for(uint8_t col = posX;col < (width+posX);col++)
	{
		for(uint8_t row = posY;row < (length+posY);row++)
		{
			lcd_dsp_buffer[col][row] = data;
		}
	}
}

void lcd_write_number(uint8_t posX, uint8_t posY, uint8_t number)
{
	for(uint8_t x = 0;x < FONT_COLUMNS;x++)
	{
		for(uint8_t y = 0;y < FONT_ROWS;y++)
		{
			lcd_dsp_buffer[posX + x][posY + y] = (font[number+'0'][y]&(1 << x)) >> x;
		}
	}
}

uint8_t lcd_init(void)
{
	HAL_TIM_Base_Start(&htim1);

	lcd_reset();
	lcd_dsp_on_off(1,1,1);
	lcd_clear();

	return 0;
}


void lcd_loop(void)
{
	uint8_t data;

	for(uint8_t col = 0;col < LCD_COLUMNS;col++)
	{
		for(uint8_t page = 0;page < LCD_PAGES;page++)
		{
			data = 0;
			for(uint8_t i = 0;i < 8;i++)
			{
				uint8_t row = page * 8 + i;
				data += lcd_dsp_buffer[col][row] << i;
			}

			lcd_write_addr(page,col,data);
		}
	}
}
