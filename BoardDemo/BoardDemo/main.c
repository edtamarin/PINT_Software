/*
 * BoardDemo.c
 * Arduino 5510 Shield 
 * Created: 12-Apr-17 15:10:27
 * Author : Egor Tamarin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "5510_Controls.h"

int cursorPos = 1; // cursor position
uint8_t isInMenu = 1; // flags
uint8_t lumRun = 0;

volatile uint8_t displaycase = 4;
volatile uint8_t portbhistory = 0xFF;
volatile uint8_t buttonState1 = 0;

char* textOnDisp[4]={
	"DISPLAY DEMO","Luminosity","About","Logo"
};
// functions used
void about(); // about section
void redrawMenu(); // redraws the menu
int isSelected(); // checks if a menu item is selected
void luminosity(); // runs the luminosity sensor

int main(void)
{
	DDR_LCD |= (1<<LCD_LED); // pin 9 is output
	DDRD = 0b00000000; // pins 5,6,7 are inputs. Others don't matter
	DDRC = 0b00000000; // pin A2 (PC2) is an input. Others don't matter
	PCICR |=  (1<<PCIE2); // enable PCINT on port B
	PCMSK2 |= (1<<PCINT20)|(1<<PCINT21)|(1<<PCINT22); // enable interrupts on pins 5,6,7
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //Prescaler at 128 so we have an 125Khz clock source
	ADMUX |= (1<<REFS0)|(1<<MUX1);
	ADMUX &= ~(1<<REFS1);   //Avcc(+5v) as voltage reference
	ADCSRB &= ~((1<<ADTS2)|(1<<ADTS1)|(1<<ADTS0)); //ADC in free-running mode
	ADCSRA |= (1<<ADATE)|(1<<ADEN)|(1<<ADSC); //define signal source, power up, start converting
    LCD_initialize(); // init LCD
	LCD_setBrightness();
	LCD_setBitmap(saxionLogo);
	LCD_update();
	_delay_ms(1500);
	sei(); // enable interrupts
    while (1) // loop
    {
		switch (displaycase){ // switch depending on which buttom we press
			case 1: // we go down
				if (cursorPos == 3){
					cursorPos = 1;
				}else{
					cursorPos++;
				}
				displaycase = 4;
				redrawMenu();
				break;
			case 2: // we go up
				if (cursorPos == 1){
					cursorPos = 3;
				}else{
					cursorPos--;
				}
				displaycase = 4;
				redrawMenu();
				break;
		    case 3: // select
				if (isInMenu){ // is we are in menu
					if (cursorPos == 1){ // if position 1 
						isInMenu = 0;
						displaycase = 0;
						while(lumRun){ // run luminosity check
							luminosity();
						}
						}else if (cursorPos == 2){ // display about section
						isInMenu = 0;
						displaycase = 0;
						about();
						LCD_update();
						}else if (cursorPos == 3){ // display Saxion logo
						isInMenu = 0;
						displaycase = 0;
						LCD_setBitmap(saxionLogo);
						LCD_update();
					}
					}else{ // if we are not in menu return there
					LCD_clear(WHITE);
					isInMenu = 1;
					displaycase = 4;
					redrawMenu();
				}
				break;
			case 4: // idling
				redrawMenu();
				break;
		}
    }
}

int isSelected(int menuPos){ // check whether the text is selected
	if (menuPos == cursorPos){
		return 0;
		}else{
		return 1;
	}
}

void redrawMenu(){ // redraws menu
	LCD_clear(WHITE);
	LCD_writeString(textOnDisp[0],6,0,1,BLACK);
	for (int i=1;i<4;i++){
		LCD_writeString(textOnDisp[i],0,i*12,1,isSelected(i));
	}
	LCD_update();
}

void about(){ // about section
	LCD_clear(WHITE);
	LCD_writeString("A B O U T",15,0,1,BLACK);
	LCD_writeString("Made by Egor",0,15,1,BLACK);
	LCD_writeString("417342",0,25,1,BLACK);
	LCD_writeString("April 2017",0,35,1,BLACK);
}

void luminosity(){ // luminosity check
	int LDR_value = ADCW; // adc input
	float voltage = LDR_value*(0.0048828125);
	float resistance = (10 * 5)/voltage - 10;
	int lux = floor(255.84 * pow(resistance,(-10/9)));
	LCD_clear(WHITE);
	uint8_t luxHundr = floor(lux/100); // splitting into numbers
	uint8_t luxTens = floor((lux-luxHundr*100)/10);
	uint8_t luxOnes = floor(lux-luxHundr*100-luxTens*10);
	LCD_writeChar((char)(luxHundr+48),18,16,1,BLACK); // printing
	LCD_writeChar((char)(luxTens+48),24,16,1,BLACK);
	LCD_writeChar((char)(luxOnes+48),30,16,1,BLACK);
	LCD_writeString("LUX",18+6*4,8*2,1,BLACK);
	LCD_update();
	_delay_ms(250);
}

ISR (PCINT2_vect){ // interrupt handling
	if (PIND & (1<<4)){ // left button
		_delay_ms(30); // debounce
		if (PIND & (1<<4)){ 
			displaycase = 1;
		}
	}	
	if (PIND & (1<<5)){ // middle button
		_delay_ms(30); // debounce
		if (PIND & (1<<5)){ 
			lumRun =! lumRun;
			displaycase = 3;
		}
	}
	if (PIND & (1<<6)){ // right button
		_delay_ms(30); // debounce
		if (PIND & (1<<6)){
			displaycase = 2;
		}
	}
}
