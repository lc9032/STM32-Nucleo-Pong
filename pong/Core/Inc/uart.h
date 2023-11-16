#ifndef INC_UART_H_
#define INC_UART_H_

#include "app.h"

#define UART_RX_BUF_SIZE (50)

extern volatile uint8_t uart_rx_rdy;
extern volatile uint8_t uart_rx_err;
extern volatile uint8_t uart_tx_rdy;
extern volatile uint8_t uart_tx_err;
extern volatile uint8_t uart_rx_buf[UART_RX_BUF_SIZE];


extern void uart_init(void);

extern void uart_readln(void);

extern void uart_write(char* text);

extern void usart2_IRQ(void);

#endif /* INC_UART_H_ */
