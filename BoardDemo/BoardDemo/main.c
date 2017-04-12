/*
 * BoardDemo.c
 * Arduino 5510 Shield 
 * Created: 12-Apr-17 15:10:27
 * Author : Egor Tamarin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "5510_Controls.h"

volatile uint8_t displayCase = 0;
volatile uint8_t portbhistory = 0xFF;
volatile uint8_t buttonState1 = 0;

int main(void)
{
	DDR_LCD |= (1<<LCD_LED); // pin 9 is output
	DDRD = 0b00000000; // pins 5,6,7 are inputs. others don't matter
	PCICR |=  (1<<PCIE2); // enable PCINT on port B
	PCMSK2 |= (1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22); // enable interrupts on pins 5,6,7
    LCD_initialize(); // init LCD
	LCD_setBrightness();
	LCD_update();
	sei(); // enable interrupts
    while (1) 
    {
		switch (displayCase){
			case 1:
				LCD_writeChar('a',2);
				LCD_update();
				displayCase = 0;
		}
    }
}

ISR (PCINT2_vect){
	if (PIND & (1<<4)){ // left button

	}	
	if (PIND & (1<<5)){ // middle button
		displayCase = 1;
	}
	if (PIND & (1<<6)){ // right button

	}
}
