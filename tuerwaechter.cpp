/*
 *
 *  Program for a door watch with sleep mode and pin change interrupt wake up
 *  Oliver - 2024
 *
 */


#include <stdint.h>			// Integer variable
#include <avr/io.h>			// I/O ports
#include <util/delay.h>		// Delay function
#include <avr/sleep.h>		// Sleep mode
#include <avr/interrupt.h>	// Interrupt implemention
#include <avr/wdt.h>		// Watchdog timer

#ifndef F_CPU				// Include guards
#define F_CPU 8000000UL		// AT90USB162 runs at 8 MHz clock speed - unsigned long
#endif						// Include guards


/***************************************************
 *  Name:        ISR(PCINT0_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Interrupt service routine
 *
 ***************************************************/
ISR(PCINT0_vect){			// Port B, PCINT0 - PCINT7
}


/***************************************************
 *  Name:        status
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Status indication.
 *
 ***************************************************/
void status (){
	PORTB |= (1 << PB5);
	_delay_ms (1000);
	PORTB &= ~(1 << PB5);
	_delay_ms (1000);
}


/***************************************************
 *  Name:        open
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Indication for door is open.
 *
 ***************************************************/
void open (){
	PORTB |= (1 << PB6);
	_delay_ms (1000);
	PORTB &= ~(1 << PB6);
	_delay_ms (1000);
}


/***************************************************
 *  Name:        alarm
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Alarm.
 *
 ***************************************************/
void alarm (){
	PORTB |= (1 << PB5);
	PORTB |= (1 << PB6);
	_delay_ms (1000);
	PORTB &= ~(1 << PB5);
	PORTB &= ~(1 << PB6);
	_delay_ms (1000);
}


/***************************************************
 *  Name:        main(void)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Main program
 *
 ***************************************************/
int main(void){

		// I/O Pin setup
		DDRB &= ~(1 << PB4);		// Pin PB4 as input - Button
		DDRB |= (1 << PB5);			// Pin PB5 as output - green LED
		DDRB |= (1 << PB6);			// Pin PB6 as output - Buzzer/ red LED

		// Variables
		uint8_t counter = 0;		// Variable for counting the clock cycles how long the door was open

	// Main loop
	while (1){

		status();											// Status blink

		if (PINB & (1 << PB4) && (counter < 2)){			// If button PB4 pressed = door open (HIGH -> LOW)
			for (uint8_t i = 0; i <= 2; i++){				// Loop until it checks input again (2 times)
				open();										// Door open indication
			}
			counter += 1;									// Counts up for how many times is input "door open" checked
		}

		if (PINB & (1 << PB4) && (counter == 2)){			// If button PB4 still pressed = door still open (HIGH -> LOW)
			for (uint8_t i = 0; i <= 2; i++){				// Loop until it checks input again (2 times)
				alarm();									// Door to long open -> alarm
			}
		}

		if ((!(PINB & (1 << PB4))) && (counter <= 2)){		// If button PB4 released = door closed and wasn't open before or closed for a while (LOW -> HIGH)
			counter = 0;									// Reset counter
			
			// Pin change interrupt setup
			cli();											// Disable interrupt for programming
			PCICR |= (1<<PCIE0);							// Turn on port b
			PCMSK0 |= (1 << PB4);							// Turn on pin PB4, which is PCINT4
			sei();											// Enable interrupt

			// Sleep mode list
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//			set_sleep_mode(SLEEP_MODE_PWR_SAVE);
//			set_sleep_mode(SLEEP_MODE_STANDBY);
//			set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
//			set_sleep_mode(SLEEP_MODE_IDLE);
//			set_sleep_mode(SLEEP_MODE_ADC);

			sleep_mode();									// Start sleep mode
		}
	}
}
