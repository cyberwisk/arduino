///////////////////////////////////////////////////////////////////////////
// Name: MagCardReader.c
// Author: B. Gian James  <gian@BattleDroids.net>
// Description: The main file implementation of reading a magnetic 
//              card from a dual-head magnetic card reader.
//
// $Id: MagCardReader.c,v 1.1.1.1 2009/08/19 06:09:42 gian Exp $
///////////////////////////////////////////////////////////////////////////

#include <avr/interrupt.h>
#include "common.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>

#define CARD_STROBE		PIND2	
#define CARD_PRESENT 	PIND3
#define	CARD_DATA1		PIND4

// These depend on the type of card and the card format
// but are probably the most common. 
#define StartSentinel	'%'
#define StopSentinel	'?'
#define FieldSeparator	'^'
// Track two are different
#define T2StartSentinel	';'
#define T2StopSentinel	'?'
#define T2FieldSeparator '='

// Our buffer and mask index
#define MAX_BUFF_SZ1	256	// power of 2
#define	MAX_BUFF_SZ2	64


volatile char				buff[MAX_BUFF_SZ1];
volatile int8_t				bit;
volatile uint8_t			idx;
volatile uint8_t			bDataPresent;

static void InitInterrupt(void);
static void ReadData(void);
static void ProcessData(void);
static void InitData(void);
static void debug(char const *);
static void WriteString(const char *);

int main()
{
	char cbuff[5];	// general translation buffer

	// setup STATUS_LED for output
	BSET(DDRB,DDB5);
	BSET(PORTB,CARD_STROBE);	// pullup
	BSET(PORTB,CARD_DATA1);		
	BSET(PORTB,CARD_PRESENT);	// card_present goes high 150ms after last tx

	USART_init(BAUD_57600, INT_NONE);
	InitData();
	InitInterrupt();

	debug("System initialized!\r");
	debug("Checking for data after ");
	debug(itoa((CHECK_TIME/1000),cbuff,10));
	debug("ms\r");

	for (;;)
	{
		if( TCNT1 >= CHECK_TIME)
		{	
			StopTimer();
			ClearTimer();

			ProcessData();
			ReadData();			

			idx = 0;
			bit = 6;
			bDataPresent = 0;
			memset(&buff,0,MAX_BUFF_SZ1);

		}	
	}
}

void
ProcessData(void)
{
	uint8_t i;
	for (i = 0; i < (idx-1); i++)
	{
		// Contains a parity bit
		// TODO check parity
		BCLR(buff[i],6);

		// and is 0x20 from ASCII
		buff[i] += 0x20;
	}

	char cbuff[3];
	WriteString("\r\rMagnetic card digits: ");
	WriteString(itoa(idx,cbuff,10));
	USART_tx('\r');

}

void
ReadData(void)
{
	uint8_t i = (idx - 1);
	char cbuff[5];

	while (buff[i] != StartSentinel && i != 0) --i;
	if (i == 0) {
		debug("Invalid card format. Try swiping again.\r");
		return;
	}
	debug("Card Format: ");
	USART_tx(buff[--i]);
	debug("\r");
	if (buff[i] == 'B') {

		debug("Account: ");
		while(buff[--i] != FieldSeparator) {
			USART_tx(buff[i]);
		}
		debug("\r");
		
		debug("Name: ");
		while(buff[--i] != FieldSeparator) {
			USART_tx(buff[i]);
		}
		debug("\r");

		cbuff[0] = buff[--i]; cbuff[1] = buff[--i]; cbuff[2] = buff[--i]; cbuff[3] = buff[--i]; cbuff[4] = '\0';
		debug("Expiration: ");
		debug(cbuff); debug("\r");
		cbuff[0] = buff[--i]; cbuff[1] = buff[--i]; cbuff[2] = buff[--i]; cbuff[3] = '\0';
		debug("Service code: ");
		debug(cbuff); debug("\r");
		debug("PIN Verification Value: ");
		cbuff[0] = buff[--i]; cbuff[1] = buff[--i]; cbuff[2] = buff[--i]; cbuff[3] = buff[--i]; cbuff[4] = '\0';
		debug(cbuff); debug("\r");
		debug("Discretionary Data: ");
		while(buff[i] != FieldSeparator && buff[i] != StopSentinel) {
			USART_tx(buff[i]);
			--i;
		}
		debug("\r");
	}
	else {	// Process other formats here


	}
}

void
InitInterrupt(void)
{
	// Setup interrupt
	BSET(EIMSK,INT0);	// external interrupt mask
	BSET(EICRA,ISC01);	// falling edge
	BCLR(EICRA,ISC00);	// falling edge

	BSET(SREG,7);		// I-bit in SREG
}

#include <avr/interrupt.h>
ISR(INT0_vect)
{
	StopTimer();
	ClearTimer();

	if ( !BCHK(PIND,CARD_DATA1) )	// inverse low = 1
	{
		BSET(GPIOR0,bit);
		--bit;
		bDataPresent = 1;
	} else if (bDataPresent) {
		BCLR(GPIOR0,bit);
		--bit;
	}

	if (bit < 0) {
		buff[idx] = (char)GPIOR0;
		++idx;
		bit = 6;
	}
		
	StartTimer();
}

static void
InitData(void)
{
	memset(&buff,0,MAX_BUFF_SZ1);
	bit = 6; idx = 0;
	bDataPresent = 0;
	GPIOR0 = 0x00;
}


static void
debug(char const * msg)
{
	while(*msg != 0x00)
		USART_tx(*(msg++));
}

void WriteString(const char * msg)
{
	while (*msg != 0x00)
		USART_tx(*(msg++));
}

