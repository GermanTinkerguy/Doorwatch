/*
** Name		: Doorwatch
** Version	: TESTING v2.3.2
**
** Created	: 2024
** Updated	: 2025
** Author	: Oliver
**
** uC		: AT90USB162
** F_CPU	: 8MHz
** Pullups	: Yes (external)
** Function	: Using a salvaged pcb with an AT90USB162, external crystal oscillator and external pullups.
**			  There is a reed-contact, so there is no use of a debounce routine.
**			  Reed-contacts are "bounce free".
**			  Program for a door watch, which indicates its functionality with a green power on led (PB5).
**			  If the door contact (PB4) is open, the alarming red led (PB6) will blink.
**			  The uC goes asleep (power down) if the door is closed.
**			  In power down, the green led (PB5) is off.
**			  Using a pin change interrupt (PCINT4) will wake up the uC.
**
** Extras	: Statemachine :
**				As base there is a state machine.
**
**			  Millis-function :
**				Using the timer0, the 8 bit timer copying a millis function like in arduino.
**
**				16-bit Timer Calculation
**
**													1
**				Timer Resolution	=	--------------------------
**										Input Frequency / Prescale
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
**				Target Timer Count	=	----------------		:	---------------------	-  1
**									  	Target Frequency			Timer Clock Frequency
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
**				64						124999					124				-> usable		-> only value fitting for 8 bit
**				256						31249					30,25			-> uunsable
**				1024					7811,5					5,8125			-> unusable
**
**				>>> 8 bit Timer
**				1 - 255
**
**				>>> 16 bit Timer
**				1 - 65535
**
**				Using Prescaler Value 64 for 8-bit counter, but 1 and 64 is usable for 16-bit, too!
*/


#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


#define CONFIG_INPUTS	{ DDRB = 0x00; DDRC = 0x00; DDRD = 0x00; }		// Configure inputs (0) including all unused pins against floating
#define BUTTON_RELEASED	( PINB & (1 << PB4) )							// PB4 - button released - PCINT4

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
	TCCR0B |= (1 << WGM02);          									// Configure timer0 for CTC mode
	TIMSK0 |= (1 << OCIE0A);        									// Enable CTC interrupt
	sei ();                  											// Enable global interrupts
	OCR0A = 124;              											// Set CTC compare value to 1000 Hz at 8 MHz AVR clock , with a prescaler of 64
	TCCR0B |= (1 << CS01) | (1 << CS00);          						// Start timer at F_CPU /64

	// Part of blink led
	const unsigned long interval = 500;									// Alarm-loop interval
	unsigned long start_millis = 0;

	// Part of statemachine
	enum statemachine state = start;
	
	// Main loop
	while (1)
	{
		// Part of millis function
		cli ();
		unsigned long current_millis = millis;							// Updates frequently
		sei ();

		// Statemachine
		switch (state)
		{
			case start:
				LED_GN_ON;												// Power on led

				if (BUTTON_RELEASED)
				{
					state = alarm;
				}				
				else
				{
					state = sleep;
				}
				break;

			case alarm:
				LED_GN_ON;												// Power on led
				for (uint8_t i = 0; i < 2; i++)							// Toggle led 2 times for a full on/off cycle
				{
					if (current_millis - start_millis >= interval)
					{
						start_millis = current_millis;
						LED_RD_TOGGLE;
					}
					state = start;
				}
				break;

			case sleep:
				LED_GN_OFF;												// Power on led off for sleeping
				LED_RD_OFF;												// Turn led off

				// Pin change interrupt setup
				cli ();													// Disable status register global interrupt - for programming
				PCICR |= (1 << PCIE0);									// Enable pin change interrupt PB4 - PCINT4
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


ISR (TIMER0_COMPA_vect)
{
	millis++;
}
