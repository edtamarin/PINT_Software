/*
 * Board_Demo.c
 *
 * Created: 30-Mar-17 09:06:47
 * Author : Egor Tamarin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "LCDControls.h"

int main(void)
{
	DDRB = 0b00111111; // pins 8,9,10,11,12,13 are outputs
	DDRD = 00000000; // pins 4,5,6 are inputs
	SPCR |= ((1<<SPE)); // enable SPI, everything on default
    PCICR |= (1<<PCIE0); // enable pin change interrupts on PCINT7..0
    PCMSK2 |= ((1<<PCINT18)|(1<<PCINT19)|(1<<PCINT20)); // enable interrupts on pins 4,5,6
	sei();
    while (1) 
    {
    }
}

ISR(PCINT18_vect){ // pin 4
	if (PIND & (1<<0)){
		_delay_ms(20); // debounce - actually needed here as the state of the button is crucial
		if (PIND & (1<<0)){
			
		}
	}
}

ISR(PCINT19_vect){ // pin 5
		if (PIND & (1<<0)){
			_delay_ms(20); // debounce - actually needed here as the state of the button is crucial
			if (PIND & (1<<0)){
				
			}
		}
}

ISR(PCINT20_vect){ // pin 6
		if (PIND & (1<<0)){
			_delay_ms(20); // debounce - actually needed here as the state of the button is crucial
			if (PIND & (1<<0)){
				
			}
		}
}

