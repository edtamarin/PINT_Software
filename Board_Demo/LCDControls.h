#include <avr/io.h>

/* PCD8544-specific defines: */
#define LCD_COMMAND  0
#define LCD_DATA     1

/* 84x48 LCD Defines: */
#define LCD_WIDTH   84 // Note: x-coordinates go wide
#define LCD_HEIGHT  48 // Note: y-coordinates go high
#define WHITE       0  // For drawing pixels. A 0 draws white.
#define BLACK       1  // A 1 draws black.

/* LCD control pin defines */
#define dcPin PB0
#define blPin PB1
#define scePin PB2
#define sdInPin PB3
#define sclkPin PB5

void LCD_write(char d_o_c, char data){
	PORTD |= (d_o_c<<dcPin);
	PORTD |= (0<<scePin);
	
}

void LCD_init(){
	
}
