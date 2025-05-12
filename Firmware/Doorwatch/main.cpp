/*
 * Program for a door watch with sleep mode and pin change interrupt wake up
 *
 * Created: 2024
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


#define BUTTON_PRESSED	(PINB & (1 << PB4))		// PB4 - Button pressed
#define LED_GN_ON		(PORTB |= (1 << PB5))	// PB5 - LED green on
#define LED_GN_OFF		(PORTB &= ~(1 << PB5))	// PB5 - LED green off
#define LED_RD_ON		(PORTB |= (1 << PB6))	// PB6 - LED red on
#define LED_RD_OFF		(PORTB &= ~(1 << PB6))	// PB6 - LED red off


// Interrupt service routine
ISR (PCINT0_vect)		// Port B, PCINT0 - PCINT7
{
}


// Status indication
void status ()
{
	LED_GN_ON;
	_delay_ms (1000);
	LED_GN_OFF;
	_delay_ms (1000);
}


// Indication for door is open
void open ()
{
	LED_RD_ON;
	_delay_ms (1000);
	LED_RD_OFF;
	_delay_ms (1000);
}


// Alarm
void alarm ()
{
	LED_GN_ON;
	LED_RD_ON;
	_delay_ms (1000);
	LED_GN_OFF;
	LED_RD_OFF;
	_delay_ms (1000);
}


// Main program
int main (void)
{
	// I/O Pin setup
	DDRB &= ~(1 << PB4);	// PB4 input  0 - Button
	DDRB |= (1 << PB5);		// PB5 output 1 - LED green
	DDRB |= (1 << PB6);		// PB6 output 1 - LED red

	// Variables
	uint8_t counter = 0;	// Variable for counting the clock cycles how long the door was open

	// Main loop
	while (1)
	{
		status ();	// Status blink

		if (BUTTON_PRESSED && (counter < 2))	// If button PB4 pressed = door open (HIGH -> LOW)
		{
			for (uint8_t i = 0; i <= 2; i++)	// Loop until it checks input again (2 times)
			{
				open();							// Door open indication
			}
			counter += 1;						// Counts up for how many times is input "door open" checked
		}

		if (BUTTON_PRESSED && (counter == 2))	// If button PB4 still pressed = door still open (HIGH -> LOW)
		{
			for (uint8_t i = 0; i <= 2; i++)	// Loop until it checks input again (2 times)
			{
				alarm();						// Door open too long -> alarm
			}
		}

		if ((!(BUTTON_PRESSED)) && (counter <= 2))	// If button PB4 released = door closed and wasn't open before or closed for a while (LOW -> HIGH)
		{
			counter = 0;							// Reset counter

			// Pin change interrupt setup
			cli ();					// Disable status register global interrupt - for programming
			PCICR |= (1<<PCIE0);	// Turn on port b
			PCMSK0 |= (1 << PB4);	// Turn on pin PB4, which is PCINT4
			sei ();					// Enable status register global interrupt

			// Sleep mode list
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set sleep mode: power down
			sleep_mode ();							// Activate sleep mode
		}
	}
}