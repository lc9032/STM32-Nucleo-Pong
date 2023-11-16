/*
 * rotary_enc.h
 *
 * Module for handling rotary encoder input.
 *
 * Created on: 27.11.2022
 * Author: LiChieh Chou
 */

#ifndef INC_ENC_H_
#define INC_ENC_H_

enum btnState {PRESSED, RELEASED};

extern enum btnState btn1;
extern int32_t enc1_count;

extern enum btnState btn2;
extern int32_t enc2_count;

/**
 * To set the counter value of the rotary encoder 1
 * 
 * @param value		the value to be set to the rotary encoder 1
 * Return: 0 for success, others for failure
 */
extern uint8_t set_enc1_val(int32_t value);

/**
 * To set the counter value of the rotary encoder 2
 * 
 * @param value     the value to be set to the rotary encoder 2
 * Return: 0 for success, others for failure
 */
extern uint8_t set_enc2_val(int32_t value);

/**
 * To set the max and min value of enc1_count and enc2_count
 * 
 * @param max     the max value to be set
 * @param min     the min value to be set
 */
extern void set_enc_max_min(int32_t max, int32_t min);

/**
 * Initialize the rotary encoder module.
 */
extern void rotary_enc_init();

#endif /* INC_ENC_H_ */
