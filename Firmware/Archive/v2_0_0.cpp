/*
 * Name		: Door watch
 *
 * Created	: 2024
 * Author	: Oliver
 *
 * uC		: AT90USB162
 * F_CPU	: 8MHz
 * Pull up	: Yes (external)
 * Function	: Using a salvaged PCB with an AT90USB162, external crystal and external pull ups.
 *			  Program for a door watch.
 *			  If the door contact (PB4) is open, the red LED (PB6) will blink with 2Hz.
 *			  After some time, the alarm will show a 1Hz fast blinking red LED (PB6).
 *			  The uC goes asleep (power down) if the door is closed for a while.
 *			  Using a pin change interrupt (PCINT4) will wake up the uC.
 */


#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


#define CONFIG_INPUTS	{ DDRB &= ~(1 << PB4); }
//#define CONFIG_INPUTS	{ DDRB = 0x00; DDRC = 0x00; DDRD = 0x00; }		// Configure inputs (0) including all unused pins against floating
//#define CONFIG_PULLUPS	{ PORTB = 0x8F; PORTC = 0xF7; PORTD = 0xFF; }	// Enable internal pull ups for all unused pins (including PB4 & except PB4, PB5 and PB6 & missing PIN PC3) against floating
#define BUTTON_PRESSED	(PINB & (1 << PB4))								// PB4 - Button pressed - PCINT4

#define CONFIG_OUTPUTS	{ DDRB |= (1 << PB5) + (1 << PB6); }			// Configure outputs (1)
#define LED_GN_ON		{ PORTB |= (1 << PB5); }						// PB5 - LED green on
#define LED_GN_OFF		{ PORTB &= ~(1 << PB5); }						// PB5 - LED green off
#define LED_GN_TOGGLE	{ PORTB ^= (1 << PB5); }						// PB5 - LED green toggle
#define LED_RD_ON		{ PORTB |= (1 << PB6); }						// PB6 - LED red on
#define LED_RD_OFF		{ PORTB &= ~(1 << PB6); }						// PB6 - LED red off
#define LED_RD_TOGGLE	{ PORTB ^= (1 << PB6); }						// PB6 - LED red toggle


// Interrupt service routine
ISR (PCINT0_vect)		// Port B, PCINT0 - PCINT7
{
}


// Indication for door is open
void open (void)
{
	LED_RD_ON;
	_delay_ms (1000);
	LED_RD_OFF;
	_delay_ms (1000);
}


// Alarm
void alarm (void)
{
	LED_RD_ON;
	_delay_ms (250);
	LED_RD_OFF;
	_delay_ms (250);
}


// Main program
int main (void)
{
	// I/O Pin setup
	CONFIG_INPUTS;
//	CONFIG_PULLUPS;
	CONFIG_OUTPUTS;
	
	// Variables
	uint8_t counter = 0;			// Variable for counting the clock cycles how long the door was open

	// Main loop
	while (1)
	{
		if (BUTTON_PRESSED && (counter < 5))		// If button PB4 pressed = door open (HIGH -> LOW)
		{
			open ();								// Door open indication
			counter += 1;							// Counts up for how many times is input "door open" checked
		}

		if (BUTTON_PRESSED && (counter == 5))		// If button PB4 still pressed = door still open (HIGH -> LOW)
		{
			alarm ();								// Door open too long -> alarm
		}

		if ((!(BUTTON_PRESSED)) && (counter <= 5))	// If button PB4 released = door closed and wasn't open before or closed for a while (LOW -> HIGH)
		{
			counter = 0;							// Reset counter

			// Pin change interrupt setup
			cli ();									// Disable status register global interrupt - for programming
			PCICR |= (1<<PCIE0);					// Enable pin change interrupt PB4 - PCINT4
			PCMSK0 |= (1 << PCINT4);				// Enable pin change interrupt mask PB4 - PCINT4
			sei ();									// Enable status register global interrupt

			// Sleep mode list
			set_sleep_mode (SLEEP_MODE_PWR_DOWN);	// Set sleep mode: power down
			sleep_mode ();							// Activate sleep mode
		}
	}
}
