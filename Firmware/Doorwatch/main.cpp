/*
** Name		: Doorwatch
** Version	: v.1.0.0
**
** Created	: 2024
** Updated	: 2025
** Author	: Oliver
**
** uC		: AT90USB162
** F_CPU	: 8MHz
** Fuses	:
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
**			  I am using the structure of a state machine.
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
#include <util/delay.h>


#define CONFIG_INPUTS	{ DDRC = 0b00000000; }							// Configure inputs (0)				<<< ORIGINAL DDRB
#define ENABLE_PULLUPS	{ PORTC = 0b00000100; }							// Enable pullups (1)				<<< ORIGINAL PORTB
#define BUTTON_RELEASED	( PINC & (1 << PC2)	)							// PB4 - button released - PCINT4	<<< ORIGINAL PINB & (1 << PB4)
#define BUTTON_PRESSED	( !(PINC & (1 << PC2)) )						// PB4 - button pressed - PCINT4	<<< ORIGINAL 

#define CONFIG_OUTPUTS	{ DDRC |= (1 << PC5) + (1 << PC6); }			// Configure outputs (1)			<<< ORIGINAL
#define LED_GN_ON		{ PORTC |= (1 << PC5); }						// PB5 - led green on				<<< ORIGINAL
#define LED_GN_OFF		{ PORTC &= ~(1 << PC5); }						// PB5 - led green off				<<< ORIGINAL
#define LED_RD_ON		{ PORTC |= (1 << PC6); }						// PB6 - led red on					<<< ORIGINAL 
#define LED_RD_OFF		{ PORTC &= ~(1 << PC6); }						// PB6 - led red off				<<< ORIGINAL
#define LED_RD_TOGGLE	{ PORTC ^= (1 << PC6); }						// PB6 - led red toggle				<<< ORIGINAL


// Interrupt service routine for Port B, PCINT0 - PCINT7
ISR (PCINT0_vect)
{	
}


// Main routine
int main (void)
{
	// Config i/o pins
	CONFIG_INPUTS;
	ENABLE_PULLUPS;
	CONFIG_OUTPUTS;

	// Variables
	static enum {STANDBY, DOOROPEN, ALARM} state = STANDBY;		// Part of statemachine
	uint8_t counter = 0;										// Variable for counting the clock cycles how long the door was open

	// Main loop
	while (1)
	{
		switch (state)
			{
				case STANDBY:
					if (BUTTON_RELEASED)
					{
						LED_GN_ON;
						LED_RD_ON;
						state = DOOROPEN;
					}
					break;

				case DOOROPEN:
					if (counter < 5 )
					{
						counter = counter + 1;
						state = DOOROPEN;
					}
					else if (counter >= 5 )
					{
						counter = 0;
						state = ALARM;
					}
					else if (BUTTON_PRESSED)
					{
						LED_GN_OFF;
						LED_RD_OFF;
						state = STANDBY;
					}
					break;

				case ALARM:
					if (BUTTON_PRESSED)
					{
						LED_GN_OFF;
						LED_RD_OFF;

						// Pin change interrupt setup
						cli ();									// Disable interrupt for programming
						PCICR |= (1<<PCIE0);					// Turn on port b
						PCMSK1 |= (1 << PC2);					// Turn on pin PB4, which is PCINT4			<<< ORIGINAL PCMSK0 |= (1 << PB4)
						sei ();									// Enable interrupt

						// Sleep mode
						set_sleep_mode (SLEEP_MODE_PWR_DOWN);

						sleep_mode ();							// Start sleep mode

						state = STANDBY;
					}
					break;
			}


		/*
		LED_GN_ON;												// Status led also for debugging
		
		if ((BUTTON_RELEASED) && (counter < 2))					// If door is open, then...
		{
			LED_RD_ON;											// Door open indication			
			counter = counter + 1;								// Counts up for how many times is input "door open" checked
		}

		else if ((BUTTON_RELEASED) && (counter >= 2))			// If door PB4 is still open, then...
		{		
			for (uint8_t i = 0; i <= 2; i++)					// Loop until it checks input again (2 times)
			{
				LED_RD_ON;										// Door to long open -> alarm
				_delay_ms (500);
				LED_RD_OFF;
				_delay_ms (500);
			}
		}

		if (!(BUTTON_RELEASED)) && (counter <= 2))				// If the door is closed and wasn't opened before or closed for a while, then...
		{
			counter = 0;										// Reset counter

			// Pin change interrupt setup
			cli ();												// Disable interrupt for programming
			PCICR |= (1<<PCIE0);								// Turn on port b
			PCMSK1 |= (1 << PC2);								// Turn on pin PB4, which is PCINT4
			sei ();												// Enable interrupt

			// Sleep mode
			set_sleep_mode (SLEEP_MODE_PWR_DOWN);

			sleep_mode ();										// Start sleep mode
		}*/
	}
}
