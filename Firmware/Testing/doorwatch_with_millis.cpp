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
** Function	: Using a salvaged PCB with an AT90USB162, external crystal and external pullups.
**		  Program for a door watch, which indicates its functionality with a green power on LED (PB5).
**		  If the door contact (PB4) is open, the red LED (PB6) will blink slow.
**		  After some time, the alarm will show fast blinking red LED (PB6).
**		  The uC goes asleep (power down) if the door is closed for a while.
**		  In power down, the green LED (PB5) ist off.
**		  Using a pin change interrupt (PCINT4) will wake up the uC.
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
#define CONFIG_PULLUPS	{ PORTB = 0x8F; PORTC = 0xF7; PORTD = 0xFF; }	// Enable internal pullups for all unused pins (including PB4 & except PB4, PB5 and PB6 & missing PIN PC3) against floating
#define BUTTON_PRESSED	{ PINB & (1 << PB4); }							// PB4 - Button pressed - PCINT4

#define CONFIG_OUTPUTS	{ DDRB |= (1 << PB5) + (1 << PB6); }			// Configure outputs (1)
#define LED_GN_ON		{ PORTB |= (1 << PB5); }						// PB5 - LED green on
#define LED_GN_OFF	{ PORTB &= ~(1 << PB5); }							// PB5 - LED green off
#define LED_GN_TOGGLE	{ PORTB ^= (1 << PB5); }						// PB5 - LED green toggle
#define LED_RD_ON		{ PORTB |= (1 << PB6); }						// PB6 - LED red on
#define LED_RD_OFF	{ PORTB &= ~(1 << PB6); }							// PB6 - LED red off
#define LED_RD_TOGGLE	{ PORTB ^= (1 << PB6); }						// PB6 - LED red toggle


volatile unsigned long millis = 0;									// Part of millis function

typedef enum statemachine {start, open, alarm, sleep};					// Part of statemachine


int main (void)
{
	// Config I/O
	CONFIG_INPUTS;
//	CONFIG_PULLUPS;
	CONFIG_OUTPUTS;

  	// Part of millis function
	TCCR1B |= (1 << WGM12);          									// Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A);        									// Enable CTC interrupt
	sei ();                  											// Enable global interrupts
	OCR1A = 7999;              											// Set CTC compare value to 1000 Hz at 8 MHz AVR clock , with a prescaler of 1
	TCCR1B |= (1 << CS10);          									// Start timer at F_CPU /1

	// Part of blink LED
	const unsigned long interval_1 = 1000;								// Open-loop (1)
	const unsigned long interval_2 = 250;								// Alarm-loop (2)
	unsigned long millis_start_1 = 0;
	unsigned long millis_start_2 = 0;

	// Part of statemachine
	uint8_t counter = 0;												// Variable for counting the clock cycles how long the door was open
	statemachine state = start;
  
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
				LED_GN_ON;												// Power on LED

				if (BUTTON_PRESSED && (counter < 2))					// If button PB4 pressed = door open (HIGH -> LOW)
				{
					state = open;
				}
				else if (BUTTON_PRESSED && (counter == 2))				// If button PB4 still pressed = door still open (HIGH -> LOW)
				{
					state = alarm;
				}				
				else													// If button PB4 released = door closed and wasn't open before or closed for a while (LOW -> HIGH)
				{
					state = sleep;
				}
				break;

			case open:
				LED_GN_ON;												// Power on LED
				counter++;												// Counts up for every round

				if (millis_current - millis_start_1 >= interval_1)
				{
					LED_RD_TOGGLE;
					millis_start_1 = millis_current;
				}
				state = start;
				break;

			case alarm:
				LED_GN_ON;

				if (millis_current - millis_start_2 >= interval_2)
				{
					LED_RD_TOGGLE;
					millis_start_2 = millis_current;
				}
				state = start;
				break;

			case sleep:
				LED_GN_OFF;												// Power on LED off for sleeping
				LED_RD_OFF;												// Turn LED off
				counter = 0;											// Reset counter

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