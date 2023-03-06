#pragma once
///////////////////////////////////////////////////////////////////////////
// Name: uart.h
// Author: B. Gian James  <gian@BattleDroids.net>
// Description:  A little bit of code to implement reading and writing 
//               from a hardware UART/USART.
//
// $Id: uart.h,v 1.1.1.1 2009/08/19 06:09:42 gian Exp $
///////////////////////////////////////////////////////////////////////////


#include "common.h"

//#define UART_BAUD 9600
#define BAUD_9600	1
#define BAUD_2400	2
#define BAUD_38400	3
#define BAUD_57600	4

#define INT_RX		0x01
#define INT_TX		0x02
#define INT_NONE	0x00

//Perform UART startup initialization.
void	USART_init(uint8_t b, uint8_t interrupt);

// Send one character to the UART.
unsigned char	USART_rx(void);

// Receive one character from the UART.  The actual reception is
// line-buffered, and one character is returned from the buffer at
// each invokation.
void	USART_tx(unsigned char);
void 	USART_tx_S(const char *);

#ifdef _USART_PGMSPACE_
void 	USART_tx_P(const char *) 
{
	while(pgm_read_byte(data) != 0x00)
		USART_tx(pgm_read_byte(data++));
}
#endif
