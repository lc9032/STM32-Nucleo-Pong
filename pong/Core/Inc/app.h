/**
 * The App module contains the core of the game's logic,
 * such as collision detection, ball and paddles movement, and score keeping.
 * It communicates with the other modules to receive input from the user
 * and update the game's display on the LCD screen.
 * This module also manages the game state and transitions between game states,
 * such as starting a new game or switching to the AI mode.
 */ 

#ifndef SRC_APP_H_
#define SRC_APP_H_

#include "stdint.h"

extern void app_init(void);

extern void app_loop(void);

#endif /* SRC_APP_H_ */
