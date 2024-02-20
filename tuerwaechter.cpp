// Clock speed

#ifndef F_CPU
#define F_CPU 8000000
#endif


// Libraries

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>


// Functions

void status (){
	PORTB |= (1 << PB5);
	_delay_ms (100);
	PORTB &= ~(1 << PB5);
	_delay_ms (100);
}

void open (){
	PORTB |= (1 << PB5);
	_delay_ms (2000);
	PORTB &= (1 << PB5);
}

void alarm (){
	PORTB |= (1 << PB5);
	_delay_ms (1000);
	PORTB &= (1 << PB5);
}


// Main program

int main (void){

	DDRB &= ~(1 << PB4);		// Pin PB4 as input - Button
	DDRB |= (1 << PB5);			// Pin PB5 as output - LED
	DDRB |= (1 << PB6);			// Pin PB6 as output - Buzzer

	while (1){								// Infinite loop
		if (PINB & (1 << PB4)){				// If button PB4 pressed -> LOW ( Opener, For a closer you need !PINB )
			status();
		}
	}
}
