/*
 * lcd.h - Header file for the LCD module.

 * This module provides a driver for the DEM 128o64B SYH-PY LCD display.
 * It is responsible for drawing the game screen on the LCD and updating
 * the screen with the current game state. This module receives commands
 * from the App module to draw or update specific elements of the game
 * screen, such as the ball, the paddles, and the score.
 *
 * Created on: 18.12.2022
 * Author: LiChieh Chou
 */
#ifndef LCD_H_
#define LCD_H_

#include<stdbool.h>

/***Constants******************************************************************/
#define LCD_PAGES 8
#define LCD_ROWS 64
#define LCD_COLUMNS 128
/******************************************************************************/

/***Functions******************************************************************/
/**
 * Initialize the LCD module.
 * Must be called before any other usage of the module.
 *
 * Return: 0 for success, others for failure
 */
extern uint8_t lcd_init(void);

/**
 * The lcd_loop function is responsible for continuously updating the display on the LCD screen
 * with the contents of the display buffer.
 */
extern void lcd_loop(void);

/**
 * To clear all the data on the LCD
 */
extern void lcd_clear();

/**
 * Draws or clears a rectangle on the LCD screen.
 *
 * This function draws a rectangle on the LCD screen with the specified
 * coordinates, width, height, and color. The x and y coordinates are
 * relative to the top-left corner of the screen, with (0,0) being the
 * top-left pixel.
 *
 * @param posX The x-coordinate of the top-left corner of the rectangle.
 * @param posY The y-coordinate of the top-left corner of the rectangle.
 * @param length The height of the rectangle.
 * @param width The width of the rectangle.
 * @param data: 1 to draw the rectangle, 0 to clear the area.
 */
extern void lcd_draw_clear_rect(uint8_t posX, uint8_t posY, uint8_t length, uint8_t width, bool data);

/**
 * This function writes a decimal number to the specified location on the LCD screen.
 * 
 * @param posX The x-coordinate of the top-left corner of the rectangle.
 * @param posY The y-coordinate of the top-left corner of the rectangle.
 * @param number The number to display.
 */
extern void lcd_write_number(uint8_t posX, uint8_t posY, uint8_t number);

/******************************************************************************/

#endif /* LCD_H_ */
