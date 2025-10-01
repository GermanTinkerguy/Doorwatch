/*
** Name	: Doorwatch
**
** Created	: 2024
** Updated	: 2025
** Author	: Oliver
**
** uC		: AT90USB162
** F_CPU	: 8MHz
** Pullups	: Yes (external)
** Function	: Using a salvaged pcb with an AT90USB162, external crystal and external pullups.
**		  Program for a door watch, which indicates its functionality with a green power on led (PB5).
**		  If the door contact (PB4) is open, the alarming red led (PB6) will blink.
**		  The uC goes asleep (power down) if the door is closed.
**		  In power down, the green led (PB5) is off.
**		  Using a pin change interrupt (PCINT4) will wake up the uC.
**
**
** Debounce
**
**	 (0)   (1)				   (3)				(0)
**	________   _   _			 _   _   ____________
**			| | | |	|			| | | | |
**			| | | |	|			| | | | |
**			| | | |	|			| | | | |
**		 	| | | |	|			| | | | |
**			|_| |_| |___________| |_| |_|
**						(2)
**
**	(0)	Sleep
**	(1) Button pressed
**	(2) Button hold
**	(3) Button released
*/


#ifndef F_CPU
#define F_CPU 8000000UL													// Solution -> Properties -> AVR/GNU C Compiler -> Symbols -> Defined symbols
#endif


#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


#define CONFIG_INPUTS	{ DDRB = 0x00; DDRC = 0x00; DDRD = 0x00; }		// Configure inputs (0) including all unused pins against floating
#define BUTTON_PRESSED	( PINB & (1 << PB4) )							// PB4 - button pressed - PCINT4

#define CONFIG_OUTPUTS	{ DDRB |= (1 << PB5) + (1 << PB6); }			// Configure outputs (1)
#define LED_GN_ON		{ PORTB |= (1 << PB5); }						// PB5 - led green on
#define LED_GN_OFF		{ PORTB &= ~(1 << PB5); }						// PB5 - led green off
#define LED_RD_OFF		{ PORTB &= ~(1 << PB6); }						// PB6 - led red off
#define LED_RD_TOGGLE	{ PORTB ^= (1 << PB6); }						// PB6 - led red toggle


volatile unsigned long millis = 0;										// Part of millis function

enum statemachine {start, alarm, sleep};								// Part of statemachine


int main (void)
{
	// Config i/o pins
	CONFIG_INPUTS;
	CONFIG_OUTPUTS;

	// Part of millis function
	TCCR1B |= (1 << WGM12);          									// Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A);        									// Enable CTC interrupt
	sei ();                  											// Enable global interrupts
	OCR1A = 7999;              											// Set CTC compare value to 1000 Hz at 8 MHz AVR clock , with a prescaler of 1
	TCCR1B |= (1 << CS10);          									// Start timer at F_CPU /1

	// Part of blink led
	const unsigned long interval_1 = 1000;								// Alarm-loop (1)
	unsigned long millis_start_1 = 0;

	// Part of statemachine
	statemachine state = start;
	
	// Main loop
	while (1)
	{
		// Part of millis function
		cli ();
		unsigned long millis_current = millis;							// Updates frequently
		sei ();

		// Part of debounce routine
		uint_8 button_state = 0;

		// Statemachine
		switch (state)
		{
			case start:
				LED_GN_ON;												// Power on led

				// Debounce routine
				if (button_state == 0 && (BUTTON_PRESSED))				// Button pressed
				{
					button_state = 1;
					return 1;
				}
				else if (button_state == 1 && (BUTTON_PRESSED))			// Button hold
				{
					button_state = 2;
					state = alarm;
					return 0;
				}
				else if (button_state == 2 && (!(BUTTON_PRESSED)))		// Button released
				{
					button_state = 3;
					return 0;
				}
				else if (button_state == 3 && (!(BUTTON_PRESSED)))		// Button untouched
				{
					button_state = 0;
					state = sleep;
					return 0;
				}
/*
 				if (BUTTON_PRESSED)
				{
					state = alarm;
				}
				else
				{
					state = sleep;
				}
				break;
*/

			case alarm:
				LED_GN_ON;												// Power on led
				if (millis_current - millis_start_1 >= interval_1)
				{
					millis_start_1 = millis_current;
					LED_RD_TOGGLE;
				}
				state = start;
				break;

			case sleep:
				LED_GN_OFF;												// Power on led off for sleeping
				LED_RD_OFF;												// Turn led off

				// Pin change interrupt setup
				cli ();													// Disable status register global interrupt - for programming
				PCICR |= (1<<PCIE0);									// Enable pin change interrupt PB4 - PCINT4
				PCMSK0 |= (1 << PCINT4);								// Enable pin change interrupt mask PB4 - PCINT4
				sei ();													// Enable status register global interrupt

				// Sleep mode list
				set_sleep_mode (SLEEP_MODE_PWR_DOWN);					// Set sleep mode: power down
				sleep_mode ();											// Activate sleep mode
			
				state = start;
				break;
		}
	}
}


ISR (TIMER1_COMPA_vect)
{
	millis++;
}
