/*
** Name		: Doorwatch
** Version	: v2.3.0
**
** Created	: 2024
** Updated	: 2025
** Author	: Oliver
**
** uC		: AT90USB162
** F_CPU	: 8MHz
** Pullups	: Yes (external)
** Function	: Using a salvaged pcb with an AT90USB162, external crystal and external pullups.
**			  Program for a door watch, which indicates its functionality with a green power on led (PB5).
**			  If the door contact (PB4) is open, the alarming red led (PB6) will blink.
**			  The uC goes asleep (power down) if the door is closed.
**			  In power down, the green led (PB5) is off.
**			  Using a pin change interrupt (PCINT4) will wake up the uC.
**
** Extras	: Millis-function :
**				Using the timer1 - 16-bit timer copying a millis function like in arduino.
**
**				16-bit Timer Calculation
**
**													1
**				Timer Resolution	=	-------------------------
**										Input Frquency / Prescale
**
**											Prescale
**									=	---------------
**										Input Frequency
**
**										    8
**									=	---------
**										8.000.000
**
**				Prescaler Value			Resolution @ 8MHz
**				1						0,125us
**				8						1us
**				64						8us
**				1024					128us
**
**											  1							  1
**				Target Timer Count	=	---------------		:	---------------------	-  1
**									  	Taret Frequency			Timer Clock Frequency
**
**													1					Prescale
**				Target Timer Count	=	( 	----------------	/	---------------	   )   -  1
**											Target Frequency		Input Frequency
**
**											      Input Frequency
**				Target Timer Count	=	( 	---------------------------	)   -  1
**											Prescale x Target Frequency		
**
**				Target Frequency:				1000 Hz -> 1000000000uSeconds
**				Input Frequency (AT90USB162):	8 MHz
**
**				8.000.000Hz
**				-----------		-	1
**				64 x 1000Hz
**
**				Prescaler Value			Target Timer Count
**				1						7999999					7999			-> usable
**				8						999999					999				-> usable
**				64						124999					124				-> usable
**				256						31249					30,25			-> uunsable
**				1024					7811,5					5,8125			-> unusable
**
**				>>> 8 bit Timer
**				1 - 255
**
**				>>> 16 bit Timer
**				1 - 65535
**
**				Using Prescaler Value 1, but 8 and 64 is usable too, also the 8-bit counter!
**
**
**			  Statemachine :
**				As base there is a state machine.
**
**
**			  Debounce-routine :
**
**									 (0)   (1)				   (3)				(0)
**									________   _   _			 _   _   ____________
**											| | | |	|			| | | | |
**											| | | |	|			| | | | |
**											| | | |	|			| | | | |
**										 	| | | |	|			| | | | |
**											|_| |_| |___________| |_| |_|
**														(2)
**
**								  (0) Sleep
**								  (1) Button pressed
**								  (2) Button hold
**								  (3) Button released
**
**								  Debounce interval = 100ms
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

enum statemachine {start, alarm, sleep};										// Part of statemachine

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

	// Part of a simple delay debounce routine
	const unsigned long interval_2 = 100;								// Debounce
	unsigned long millis_start_2 = 0;

	// Part of statemachine
	enum statemachine state = start;
	
	// Main loop
	while (1)
	{
		// Part of millis function
		cli ();
		unsigned long millis_current = millis;							// Updates frequently
		sei ();

		// Statemachine
		switch (state)
		{
			case start:
				LED_GN_ON;												// Power on led

				if (BUTTON_PRESSED && (millis_current - millis_start_2 >= interval_2))	// Debounce routine
				{
					millis_start_2 = millis_current;
					state = alarm,
				}
				else if (!(BUTTON_PRESSED))
				{
					state = sleep;
				}
				break;

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
