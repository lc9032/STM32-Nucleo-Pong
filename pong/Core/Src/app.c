#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "app.h"
#include "main.h"
#include "uart.h"
#include "timer.h"
#include "rotary_enc.h"
#include "lcd.h"

#define UPDATE_PADDLE_DLY 50
#define UPDATE_BALL_DLY 50
#define AI_DLY 70

#define INFO_AREA_SIZE 7
#define PLAYFIELD_COLUMNS LCD_COLUMNS-2
#define PLAYFIELD_ROWS LCD_ROWS-INFO_AREA_SIZE-3

#define PADDLE_LENGTH 20
#define PADDLE_WIDTH 5
#define PADDLE_INITIAL_POS (PLAYFIELD_ROWS - PADDLE_LENGTH)/2 + INFO_AREA_SIZE + 1

#define BALL_SIZE 4
#define BALL_INITIAL_POS_X LCD_COLUMNS/2
#define BALL_INITIAL_POS_Y (PLAYFIELD_ROWS - BALL_SIZE)/2 + INFO_AREA_SIZE + 1

static int32_t enc1_old;
static enum btnState btn1_old;

static int32_t enc2_old;
static enum btnState btn2_old;

static int8_t ball_x;
static int8_t ball_y;
static int8_t ball_dx;
static int8_t ball_dy;

static uint8_t player1_score;
static uint8_t player2_score;

static bool player1_AI_mode;
static bool player2_AI_mode;

static void game_init();

static void draw_border_line_on_lcd()
{
	lcd_draw_clear_rect(0,0,LCD_ROWS,1,1);
	lcd_draw_clear_rect(LCD_COLUMNS-1,0,LCD_ROWS,1,1);

	lcd_draw_clear_rect(0,0,1,LCD_COLUMNS,1);
	lcd_draw_clear_rect(0,LCD_ROWS-1,1,LCD_COLUMNS,1);

	lcd_draw_clear_rect(0,INFO_AREA_SIZE+1,1,LCD_COLUMNS,1);
}

static void update_score_on_lcd(void)
{
	lcd_write_number(2, 2, player1_score/10);
	lcd_write_number(7, 2, player1_score%10);

	lcd_write_number(LCD_COLUMNS-11, 2, player2_score/10);
	lcd_write_number(LCD_COLUMNS-6, 2, player2_score%10);
}

static void update_ball_position()
{
	lcd_draw_clear_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE, 0);

	if(ball_x <= PADDLE_WIDTH + 1)
	{
		if((ball_y + BALL_SIZE > enc1_count) && (ball_y < enc1_count + PADDLE_LENGTH))
		{
			// The ball hits the paddle 1
			ball_dx = 1;
			if(ball_dy == 0) ball_dy = 1;
		}
		else
		{
			//paddle 1 miss
			if (player2_score < 99) player2_score++;
			game_init();
		}
	}
	else if(ball_x >= LCD_COLUMNS - PADDLE_WIDTH - BALL_SIZE - 1)
	{
		if((ball_y + BALL_SIZE > enc2_count) && (ball_y < enc2_count + PADDLE_LENGTH))
		{
			// The ball hits the paddle 2
			ball_dx = -1;
			if(ball_dy == 0) ball_dy = 1;
		}
		else
		{
			//paddle 2 miss
			if (player1_score < 99) player1_score++;
			game_init();
		}
	}
	else if((ball_y <= INFO_AREA_SIZE + 2) || (ball_y+BALL_SIZE >= LCD_ROWS - 1))
	{
		// the ball hit the top or bottom wall
		ball_dy *= -1;
	}

	// Update the ball's position
	ball_x += ball_dx;
	ball_y += ball_dy;
	lcd_draw_clear_rect(ball_x, ball_y, BALL_SIZE, BALL_SIZE, 1);
}

static void update_paddle_position()
{
	if((enc1_old != enc1_count))
	{
		//update the paddle1's position
		lcd_draw_clear_rect(1,(uint8_t)enc1_old,PADDLE_LENGTH,PADDLE_WIDTH,0);
		lcd_draw_clear_rect(1,(uint8_t)enc1_count,PADDLE_LENGTH,PADDLE_WIDTH,1);

		enc1_old = enc1_count;
	}

	if((enc2_old != enc2_count))
	{
		//update the paddle2's position
		lcd_draw_clear_rect(LCD_COLUMNS-PADDLE_WIDTH-1,(uint8_t)enc2_old,PADDLE_LENGTH,PADDLE_WIDTH,0);
		lcd_draw_clear_rect(LCD_COLUMNS-PADDLE_WIDTH-1,(uint8_t)enc2_count,PADDLE_LENGTH,PADDLE_WIDTH,1);

		enc2_old = enc2_count;
	}
}

static void get_enc_btn_state()
{
	if(btn1_old != btn1)
	{
		//read the btn1's state
		if(btn1 == PRESSED)
			player1_AI_mode = !player1_AI_mode;

		btn1_old = btn1;
	}

	if(btn2_old != btn2)
	{
		//read the btn2's state
		if(btn2 == PRESSED)
			player2_AI_mode = !player2_AI_mode;

		btn2_old = btn2;
	}
}

static void ai_mode()
{
	get_enc_btn_state();

	if((player1_AI_mode) && (btn1 != PRESSED))
	{
		if((enc1_count + (PADDLE_LENGTH/2)) > (ball_y+(BALL_SIZE/2)))
		{
			set_enc1_val(enc1_count - 1); // Move the paddle 1 up
		}
		else if((enc1_count + (PADDLE_LENGTH/2)) < (ball_y+(BALL_SIZE/2)))
		{
			set_enc1_val(enc1_count + 1); // Move the paddle 1 down
		}
	}
	if((player2_AI_mode) && (btn2 != PRESSED))
	{
		if((enc2_count + (PADDLE_LENGTH/2)) > (ball_y+(BALL_SIZE/2)))
		{
			set_enc2_val(enc2_count - 1); // Move the paddle 2 up
		}
		else if((enc2_count + (PADDLE_LENGTH/2)) < (ball_y+(BALL_SIZE/2)))
		{
			set_enc2_val(enc2_count + 1); // Move the paddle 2 down
		}
	}
}

static void game_init()
{
	//clear the display
	lcd_clear();

	//draw the border line and update the score
	draw_border_line_on_lcd();
	update_score_on_lcd();

	//init the paddles' position
	set_enc_max_min(LCD_ROWS - PADDLE_LENGTH - 1, INFO_AREA_SIZE + 2);
	set_enc1_val(PADDLE_INITIAL_POS);
	set_enc2_val(PADDLE_INITIAL_POS);
	enc1_old = enc1_count;
	enc2_old = enc2_count;

	//draw the paddles on lcd
	lcd_draw_clear_rect(1,(uint8_t)enc1_count,PADDLE_LENGTH,PADDLE_WIDTH,1);
	lcd_draw_clear_rect(LCD_COLUMNS-PADDLE_WIDTH-1,(uint8_t)enc2_count,PADDLE_LENGTH,PADDLE_WIDTH,1);

	//init the ball's position
	ball_x = BALL_INITIAL_POS_X;
	ball_y = BALL_INITIAL_POS_Y;

	//init the ball's moving direction
	if((player1_score+player2_score)%2 == 0)
		ball_dx = -1;
	else
		ball_dx = 1;
	
	ball_dy = 0;

	//set the ball moving speed and update the AI speed
	if((player1_score+player2_score) < 10)
	{
		timer_func_divider_update(update_ball_position, UPDATE_BALL_DLY-(player1_score+player2_score)*3);
		timer_func_divider_update(ai_mode, AI_DLY-(player1_score+player2_score)*4);
	}
}

void app_init(void)
{
	player1_score = 0;
	player2_score = 0;

	timer_init();
	rotary_enc_init();
	lcd_init();

	game_init();

	timer_register(update_paddle_position, UPDATE_PADDLE_DLY);
	timer_register(update_ball_position, UPDATE_BALL_DLY);
	timer_register(ai_mode, AI_DLY);
}

void app_loop(void)
{
	lcd_loop();
}
