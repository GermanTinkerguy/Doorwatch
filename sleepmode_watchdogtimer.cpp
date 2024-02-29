/*
 *
 *	Program for sleep mode examples with watchdog timer wake up
 *	Oliver - 2024.
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
 *  Name:        watchdogSetup
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog settings.
 *
 ***************************************************/
void watchdogSetup(void){
	cli();													// Disable all interrupts for no problems while setup
	wdt_reset();											// Watchdog reset
	WDTCSR |= (1<<WDCE) | (1<<WDE);							// Watchdog initialize. WDCE: Watchdog Change Enable. WDE: Watchdog System Reset Enable
	//Timer setup	
//	WDTCSR = (0<<WDIE) | (0<<WDE) | (0<<WDP3) | (0<<WDP0);	// 16 ms / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (0<<WDE) | (0<<WDP3) | (1<<WDP0);	// 32 ms / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (0<<WDE) | (1<<WDP3) | (0<<WDP0);	// 64 ms / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (0<<WDE) | (1<<WDP3) | (1<<WDP0);	// 0.125 s / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (1<<WDE) | (0<<WDP3) | (0<<WDP0);	// 0.25 s / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (1<<WDE) | (0<<WDP3) | (1<<WDP0);	// 0.5 s / interrupt, no system reset
	WDTCSR = (0<<WDIE) | (1<<WDE) | (1<<WDP3) | (0<<WDP0);	// 1.0 s / interrupt, no system reset
//	WDTCSR = (0<<WDIE) | (1<<WDE) | (1<<WDP3) | (1<<WDP0);	// 2.0 s / interrupt, no system reset						
//	WDTCSR = (1<<WDIE) | (0<<WDE) | (0<<WDP3) | (0<<WDP0);	// 4.0 s / interrupt, no system reset
//	WDTCSR = (1<<WDIE) | (0<<WDE) | (0<<WDP3) | (1<<WDP0);	// 8.0 s / interrupt, no system reset
	sei();													// Interrupts acivated
}


/***************************************************
 *  Name:        ISR(WDT_vect)
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: 
 *
 ***************************************************/
ISR(WDT_vect){
	//put in additional code here
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
	
	DDRB |= (1 << PB5);		// PB5/LED as output -> LOW
	watchdogSetup();		// Watchdog setup
	
	PORTB |= (1<<PB5);		// --Awake -- PB5/LED on - LOW -> HIGH
	_delay_ms(500);			// --Awake -- Delay
	PORTB &= ~(1<<PB5);		// --Awake -- PB5/LED off - HIGH -> LOW
	_delay_ms(500);			// --Awake -- Delay
	wdt_reset();			// Reset Watchdog

	// Sleep mode list
	  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // choose power down mode
	//  set_sleep_mode(SLEEP_MODE_PWR_SAVE); // choose power save mode
	//  set_sleep_mode(SLEEP_MODE_STANDBY); // choose external standby power mode
	//  set_sleep_mode(SLEEP_MODE_EXT_STANDBY); // choose external standby power mode
	//  set_sleep_mode(SLEEP_MODE_IDLE); // did not work like this!
	//  set_sleep_mode(SLEEP_MODE_ADC); // choose ADC noise reduction mode
	//  sleep_bod_disable();  // optional brown-out detection switch off

	sleep_mode();		// --Asleep-- Start sleep mode
}
