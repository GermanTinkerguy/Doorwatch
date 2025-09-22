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
#define F_CPU 8000000UL		// Solution -> Properties -> AVR/GNU C Compiler -> Symbols -> Defined symbols
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

volatile unsigned long millis = 0;

int main (void)
{
	CONFIG_OUTPUTS;
	
	TCCR1B |= (1 << WGM12);					// Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A);				// Enable CTC interrupt
	sei ();									// Enable global interrupts
	OCR1A = 7999;							// Set CTC compare value to 1000 Hz = Millisecond at 8 MHz AVR clock , with a prescaler of 1
	TCCR1B |= (1 << CS10);					// Start timer at F_CPU /1
	
	const unsigned long eventInterval = 1000;
	unsigned long previousTime = 0;
	
	while (1)
	{
		cli (); // Disable interrupts
		// Updates frequently
		unsigned long currentTime = millis;
		sei ();	// Enable interrupts
			// This is the event
			if (currentTime - previousTime >= eventInterval) {
			// Event code
			LED_GN_TOGGLE;			  
			// Update the timing for the next time around
			previousTime = currentTime;			
			}
	}
}


ISR (TIMER1_COMPA_vect)
{
	millis++;
}