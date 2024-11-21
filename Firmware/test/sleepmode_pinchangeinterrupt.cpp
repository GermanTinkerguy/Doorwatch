/*
 *
 *  Program for sleep mode with pin change interrupt
 *  Oliver - 2024
 *
 */


//#include <stdint.h>		// Integer variable
#include <avr/io.h>			// I/O ports
#include <util/delay.h>		// Delay function
#include <avr/sleep.h>		// Sleep mode
#include <avr/interrupt.h>	// Interrupt implemention
//#include <avr/wdt.h>		// Watchdog timer

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

	while (1){
		DDRB &= ~(1 << PB4);	// PB4/Button as input
		DDRB |= (1 << PB5);		// PB5/LED as output

		cli();
		PCICR |= (1<<PCIE0);	// Turn on port b
		PCMSK0 |= (1 << PB4);	// Turn on pin PB4, which is PCINT4
		sei();

		PORTB |= (1<<PB5);		// PB5/LED on - LOW -> HIGH
		_delay_ms(500);			// Delay
		PORTB &= ~(1<<PB5);		// PB5/LED off - HIGH -> LOW
		_delay_ms(500);			// Delay

		// Sleep mode list
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//		set_sleep_mode(SLEEP_MODE_PWR_SAVE);
//		set_sleep_mode(SLEEP_MODE_STANDBY);
//		set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
//		set_sleep_mode(SLEEP_MODE_IDLE);
//		set_sleep_mode(SLEEP_MODE_ADC);

		sleep_mode();		// Start sleep mode
	}
}
