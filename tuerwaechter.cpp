// Libraries

#include <avr/io.h>			// In/Out
#include <util/delay.h>		// Delay


// Variables

const int delay = 100;


// I/O ports

//DDRB &= ~(1 << PB4);					// PB 4 as input
//DDRB &= ~(1 << PB5);					// PB 5 as input
//DDRB |= (1 << PB6);						// PB 6 as output
//DDRB |= (1 << PB7);						// PB 7 as output


// I/O ports setup -> unnötig?

//PORTB &= ~(1 << PB6);					// Low
//PORTB &= ~(1 << PB7);					// Low


// Functions

void blinkLeft (){
	
	// .. than blink sequence left
	PORTB |= (1 << PB6);			// PB 6 on
	_delay_ms(delay);				// Delay in ms
	PORTB &= ~(1 << PB6);			// PB 6 off
	_delay_ms(delay);				// Delay in ms
}
	
void blinkRight(){
	
	// ... than blink sequence right
	PORTB |= (1 << PB7);			// PB 7 on
	_delay_ms(delay);				// Delay in ms
	PORTB &= ~(1 << PB7);			// PB 7 off
	_delay_ms(delay);				// Delay in ms
}


// Main program

int main (void){


DDRB |= (1 << PB6);						// PB 6 as output
DDRB |= (1 << PB7);						// PB 7 as output

	// Main loop
	while (1){								// Infinite loop
			
		if (PINB & (1 << PB4)){				// If button PB4 pressed,...
			blinkRight();
		}
		
		if (PINB & (1 << PB5)){				// If button PB5 pressed,...
			blinkLeft();
		}
	}
}
