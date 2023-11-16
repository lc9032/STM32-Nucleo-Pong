#include "uart.h"
#include "app.h"
#include "main.h"
#include <string.h>

volatile uint8_t uart_rx_rdy;
volatile uint8_t uart_rx_err;
volatile uint8_t uart_tx_rdy;
volatile uint8_t uart_tx_err;
volatile uint8_t uart_rx_buf[UART_RX_BUF_SIZE];

typedef struct
{
	uint8_t *ptr;
	uint16_t size;
	uint16_t counter;
} bufferInfo;

bufferInfo bufInfo;

uint8_t receive_counter = 0;

void usart2_receive()
{
	if (LL_USART_IsActiveFlag_ORE(USART2) || LL_USART_IsActiveFlag_FE(USART2))
	{
		LL_USART_ReceiveData8(USART2);
		receive_counter = 0;
		uart_rx_err = 1;
	}
	else if (LL_USART_IsActiveFlag_RXNE(USART2))
	{
		uart_rx_buf[receive_counter] = (uint8_t)LL_USART_ReceiveData8(USART2);
		if (receive_counter >= UART_RX_BUF_SIZE)
		{
			uart_write("Command too long\n");
			receive_counter = 0;
		}
		else if (uart_rx_buf[receive_counter] == '\n')
		{
			uart_rx_buf[receive_counter] = '\0';
			receive_counter = 0;
			uart_rx_rdy = 1;
		}
		else
		{
			receive_counter++;
		}
	}
}

void usart2_transmit()
{
	uint8_t dataToTransmit;

	if (LL_USART_IsActiveFlag_TXE(USART2))
	{
		bufInfo.counter++;
		if (bufInfo.counter > bufInfo.size)
		{
			LL_USART_DisableIT_TXE(USART2);
			uart_tx_rdy = 1;
			return;
		}
		dataToTransmit = *(bufInfo.ptr++);
		LL_USART_TransmitData8(USART2, (uint8_t)dataToTransmit);
	}
}

void uart_init()
{
	uart_rx_rdy = 1;
	uart_rx_err = 0;
	uart_tx_rdy = 1;
	uart_tx_err = 0;
}

void uart_readln()
{
	if(uart_rx_rdy == 1)
	{
		uart_rx_rdy = 0;
		LL_USART_EnableIT_RXNE(USART2);
	}
	else
	{
		uart_rx_err = 1;
	}
}

void uart_write(char* text)
{
	if(uart_tx_rdy == 1)
	{
		uart_tx_rdy = 0;
		bufInfo.ptr = (uint8_t*)text;
		bufInfo.size = strlen(text);
		bufInfo.counter = 0;
		LL_USART_EnableIT_TXE(USART2);
	}
	else
	{
		uart_tx_err = 1;
	}

}
