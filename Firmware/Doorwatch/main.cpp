/*
** Name		: Doorwatch
** Version	: v.1.2.1
**
** Created	: 2024
** Updated	: 13.02.2026
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
**			 IMPORTANT NOTE: This is not the productivity code for the salvaged PCB. It is for the diy development board
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


#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/sleep.h>													// Power down mode
#include <avr/interrupt.h>												// Pin change interrupt
#include <util/delay.h>													// Trying to avoid this library with millis in the future

#define CONFIG_INPUTS	{ DDRC = 0x00; }								// Configure inputs (0)				<<< ORIGINAL DDRB
#define ENABLE_PULLUPS	{ PORTC |= (1 << PC2); }						// Enable pullups (1)				<<< ORIGINAL NO NEED for internal pullups!
#define CONFIG_OUTPUTS	{ DDRC |= (1 << PC5) + (1 << PC6); }			// Configure outputs (1)			<<< ORIGINAL

#define DOOR_OPEN		( PINC & (1 << PC2)	)							// PB4 - button released - PCINT4	<<< ORIGINAL PINB & (1 << PB4)
#define DOOR_CLOSE		( !(PINC & (1 << PC2)) )						// PB4 - button pressed - PCINT4	<<< ORIGINAL 
#define LED_GN_ON		{ PORTC |= (1 << PC5); }						// PB5 - led green on				<<< ORIGINAL
#define LED_GN_OFF		{ PORTC &= ~(1 << PC5); }						// PB5 - led green off				<<< ORIGINAL
#define LED_RD_ON		{ PORTC |= (1 << PC6); }						// PB6 - led red on					<<< ORIGINAL 
#define LED_RD_OFF		{ PORTC &= ~(1 << PC6); }						// PB6 - led red off				<<< ORIGINAL
#define LED_RD_TOGGLE	{ PORTC ^= (1 << PC6); }						// PB6 - led red toggle				<<< ORIGINAL


volatile unsigned long millis = 0;										// Part of millis function


// Interrupt service routine for Port B, PCINT0 - PCINT7
ISR (PCINT1_vect)														//									<<< ORIGINAL ISR (PCINT0_vect)
{	
}


// Main routine
int main (void)
{
	// Config i/o pins
	CONFIG_INPUTS;
	ENABLE_PULLUPS;
	CONFIG_OUTPUTS;
	
	// Part of millis function
	TCCR1B |= (1 << WGM12);          									// Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A);        									// Enable CTC interrupt
	sei ();                  											// Enable global interrupts
	OCR1A = 7999;              											// Set CTC compare value to 1000 Hz at 8 MHz AVR clock , with a prescaler of 1
	TCCR1B |= (1 << CS10);          									// Start timer at F_CPU /1

	// Variables	
	const unsigned long interval = 500;									// Part of millis function
	unsigned long millis_start = 0;										// Part of millis function
	uint8_t counter = 0;												// Variable for counting the clock cycles how long the door was open
	static enum															// Part of statemachine
	{
		STANDBY,
		OPEN,
		ALARM
	}
	state = STANDBY;
	
	// Init pin change interrupt
	cli ();																// Disable interrupt for programming
	PCICR |= (1 << PCIE1);												// Turn on port b							<<< ORIGINAL PCICR |= (1 << PCIE0);
	PCMSK1 |= (1 << PCINT11);											// Turn on pin PB4, which is PCINT4			<<< ORIGINAL PCMSK0 |= (1 << PB4)
	sei ();																// Enable interrupt

	// Main loop
	while (1)
	{		
		// Part of millis function
		cli ();
		unsigned long millis_current = millis;							// Updates frequently
		sei ();
				
		switch (state)
			{
				case STANDBY:
					if (DOOR_OPEN)
					{
						LED_GN_ON;										// Status and debug led
						LED_RD_ON;

						state = OPEN;
					}
					break;

				case OPEN:
					if ((DOOR_OPEN) && (counter <= 5))					// Counting the time until...
					{
// 						_delay_ms (1000);
// 						counter = counter + 1;
						if (millis_current - millis_start >= (interval * 2))
						{
							counter++;
							millis_start = millis_current;
						}
						
						state = OPEN;
					}
					else if ((DOOR_OPEN) && (counter > 5))				// ...5 seconds left, then change to state alarm
					{
						state = ALARM;
					}
					else if (DOOR_CLOSE)
					{
						LED_GN_OFF;
						LED_RD_OFF;
						counter = 0;

						// Init sleep mode
						set_sleep_mode (SLEEP_MODE_PWR_DOWN);
						sleep_mode ();									// Start sleep mode

						state = STANDBY;
					}
					break;

 				case ALARM:
					if ((DOOR_CLOSE) || (counter == 255))				// ... and if maximum of "uint8_t counter" is reached, securely go sleepmode instead of any kind of error
					{
						LED_GN_OFF;
						LED_RD_OFF;
						counter = 0;

						// Init sleep mode
						set_sleep_mode (SLEEP_MODE_PWR_DOWN);
						sleep_mode ();									// Start sleep mode

						state = STANDBY;
					}
					else
					{
// 						LED_RD_TOGGLE;									// Blink routine for alarm-state
// 						_delay_ms (500);
						if (millis_current - millis_start >= interval)
						{
							LED_RD_TOGGLE;
							millis_start = millis_current;
						}

						state = ALARM;
					}
					break;
					
				default:
					break;
			}
	}
	return 0;
}


ISR (TIMER1_COMPA_vect)
{
	millis++;
}