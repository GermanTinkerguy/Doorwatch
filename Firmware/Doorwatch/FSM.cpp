/*
 * Program for a door watch with sleep mode and pin change interrupt wake up using a finite state machine
 *
 * Created: 19.05.2025
 * Author : Oliver
 */ 

#ifndef F_CPU				// Include guards
#define F_CPU 8000000UL		// AT90USB162 runs at 8 MHz clock speed - unsigned long
#endif						// Include guards

#include <stdint.h>			// Integer variable
#include <avr/io.h>			// I/O ports
#include <util/delay.h>		// Delay function
#include <avr/sleep.h>		// Sleep mode
#include <avr/interrupt.h>	// Interrupt implementation

#define BUTTON_PRESSED	(PINB & (1 << PB4))		// PB4 - Button pressed - PCINT4
#define LED_GN_ON		(PORTB |= (1 << PB5))	// PB5 - LED green on
#define LED_GN_OFF		(PORTB &= ~(1 << PB5))	// PB5 - LED green off
#define LED_GN_TOGGLE	(PORTB ^= (1 << PB5))	// PB5 - LED green toggle
#define LED_RD_ON		(PORTB |= (1 << PB6))	// PB6 - LED red on
#define LED_RD_OFF		(PORTB &= ~(1 << PB6))	// PB6 - LED red off
#define LED_RD_TOGGLE	(PORTB ^= (1 << PB6))	// PB6 - LED red toggle

enum State
{
	IDLE,
	OPEN,
	ALARM,
	SLEEP
};
State currentState = IDLE;

int setup (void)
{
	DDRB &= ~(1 << PB4);	// PB4 input  0 - Button
	DDRB |= (1 << PB5);		// PB5 output 1 - LED green
	DDRB |= (1 << PB6);		// PB6 output 1 - LED red
}

int loop (void)
{
	switch (currentState)
	{
		case IDLE:
		break;
		case OPEN:
		break;
		case ALARM:
		break;
		case SLEEP:
		break;
	}	
}

int main (void)
{
	setup ();
	
	while (1)
	{
		loop ();
	}
}