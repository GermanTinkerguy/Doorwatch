// Clock speed

//#ifndef F_CPU
#define F_CPU 8000000		// AT90USB162 runs at 8 MHz
//#endif


// Libraries

#include <stdint.h>			// Integer variable
#include <avr/io.h>			// I/O ports
#include <util/delay.h>		// Delay function
#include <avr/sleep.h>		// Sleep mode (not used atm)


// Functions

void status (){
	PORTB |= (1 << PB6);
	_delay_ms (100);
	PORTB &= !(1 << PB6);
	_delay_ms (100);
}

void open (){
	PORTB |= (1 << PB5);
	_delay_ms (100);
	PORTB &= !(1 << PB5);
	_delay_ms (100);
}

void alarm (){
	PORTB |= (1 << PB5);
	PORTB |= (1 << PB6);	
	_delay_ms (100);
	PORTB &= !(1 << PB5);
	PORTB &= !(1 << PB6);	
	_delay_ms (100);
}


// Main program

int main (void){

	DDRB &= !(1 << PB4);		// Pin PB4 as input - Button
	DDRB |= (1 << PB5);			// Pin PB5 as output - green LED
	DDRB |= (1 << PB6);			// Pin PB6 as output - Buzzer/ red LED

	uint8_t counter = 0;

	while (1){										// Infinite loop
//		status();									// Status blink
		if (PINB & (1 << PB4) && (counter < 2)){	// If button PB4 pressed -> LOW
			for (uint8_t i = 0; i <= 2; i++){		// Loop until it checks input again
				open();								// Door open
			}
			counter += 1;							// Counts up for how many times is input "door open" checked
		}
		if (PINB & (1 << PB4) && (counter == 2)){	// If button PB4 pressed -> LOW
			for (uint8_t i = 0; i <= 2; i++){		// Loop until it checks input again
				alarm();							// Door to long open -> alarm
			}
		}
		if ((!(PINB & (1 << PB4))) && (counter == 2)) {		// If button PB4 release -> HIGH
			counter = 0;									// Reset counter
		}
	}
}
