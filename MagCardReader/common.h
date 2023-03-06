///////////////////////////////////////////////////////////////////////////
// Name: common.h
// Author: B. Gian James  <gian@BattleDroids.net>
// Description: All the common stuff I include in all my applications.
//
// $Id: common.h,v 1.1.1.1 2009/08/19 06:09:42 gian Exp $
///////////////////////////////////////////////////////////////////////////


#pragma once
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>

// This just makes my life easier
#define BIT16(b) ((unsigned long)0x00000001 << (b))
#define	BIT8(b) (0x01 << (b))


// from AVR035: Efficient C Coding for AVR 
#define BSET(ADDRESS,BIT) (ADDRESS |= (unsigned char)(1<<BIT)) 
#define BCLR(ADDRESS,BIT) (ADDRESS &= (unsigned char)~(1<<BIT)) 
#define BTOG(ADDRESS,BIT) (ADDRESS ^= (unsigned char)(1<<BIT)) 
#define BCHK(ADDRESS,BIT) (ADDRESS &  (unsigned char)(1<<BIT)) 

#define BMSET(x,y) (x |= (y)) 
#define BMCLR(x,y) (x &= (~y)) 
#define BMTOG(x,y) (x ^= (y)) 
#define BMCHK(x,y) (x & (y)) 

#define IO_IN	0
#define IO_OUT	1

// Generic timer stuff

#define PRESCALER		1024
#define CHECK_TIME		( (F_CPU / PRESCALER) / 4 )				// 250 ms
#define StartTimer()	BSET(TCCR1B,CS10), BSET(TCCR1B,CS12)	// 1024 prescaler
#define StopTimer()		BCLR(TCCR1B,CS10), BCLR(TCCR1B,CS12)
#define ClearTimer()	(TCNT1 = 0)

