/*
 * _5510_Controls.h
 *
 * Created: 12-Apr-17 15:11:38
 *  Author: Egor Tamarin
 */ 


#ifndef _5510_CONTROLS_H_
#define _5510_CONTROLS_H_

#include <avr/pgmspace.h>
#include <stdint.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include "5510_Data.h"

// define message types
#define COMMAND 1
#define DATA 0
// define the connections
#define PORT_LCD PORTB
#define DDR_LCD DDRB
// pins in use
#define LCD_SCE PB2
#define LCD_RST PB4
#define LCD_DC PB0
#define LCD_DIN PB3
#define LCD_CLK PB5
#define LCD_LED PB1
// contrast
#define LCD_CONTRAST 0x40

static struct {
	/* screen byte massive */
	uint8_t screen[504];

	/* cursor position */
	uint8_t cursor_x;
	uint8_t cursor_y;

	} nokia_lcd = {
	.cursor_x = 0,
	.cursor_y = 0
};

static void LCD_write(uint8_t message, uint8_t type)
{
		register uint8_t i;
		/* Enable controller */
		PORT_LCD &= ~(1 << LCD_SCE);

		/* We are sending data */
		if (type == DATA)
		PORT_LCD |= (1 << LCD_DC);
		/* We are sending commands */
		else
		PORT_LCD &= ~(1 << LCD_DC);

		/* Send bytes */
		for (i = 0; i < 8; i++) {
			/* Set data pin to byte state */
			if ((message >> (7-i)) & 0x01)
			PORT_LCD |= (1 << LCD_DIN);
			else
			PORT_LCD &= ~(1 << LCD_DIN);

			/* Blink clock */
			PORT_LCD |= (1 << LCD_CLK);
			PORT_LCD &= ~(1 << LCD_CLK);
		}

		/* Disable controller */
		PORT_LCD |= (1 << LCD_SCE);
}

void LCD_initialize(void){
	register unsigned i;
	// Set pins as output
	DDR_LCD |= (1 << LCD_SCE);
	DDR_LCD |= (1 << LCD_RST);
	DDR_LCD |= (1 << LCD_DC);
	DDR_LCD |= (1 << LCD_DIN);
	DDR_LCD |= (1 << LCD_CLK);
	// Reset display
	PORT_LCD |= (1 << LCD_RST);
	PORT_LCD |= (1 << LCD_SCE);
	_delay_ms(10);
	PORT_LCD &= ~(1 << LCD_RST);
	_delay_ms(70);
	PORT_LCD |= (1 << LCD_RST);
	// start display init
	PORT_LCD &= ~(1 << LCD_SCE);
	// enter extended commands mode
	LCD_write(0x21, COMMAND);
	// set resolution
	LCD_write(0x13, COMMAND);
	// temp coefficient
	LCD_write(0x06, COMMAND);
	// Default VOP (3.06 + 66 * 0.06 = 7V)
	LCD_write(0xC2, COMMAND);
	// enter standard commands mode
	LCD_write(0x20, COMMAND);
	// normal operation from now on
	LCD_write(0x09,COMMAND);

	// clear screen
	LCD_write(0x80, COMMAND);
	LCD_write(LCD_CONTRAST, COMMAND);
	for (i = 0; i < 504; i++)
		LCD_write(0x00, DATA);

	// activate LCD
	LCD_write(0x08, COMMAND);
	LCD_write(0x0C, COMMAND);
}

void LCD_setBrightness(){
	PORT_LCD |= (1<<LCD_LED);
}

void LCD_setPixel(uint8_t x, uint8_t y, uint8_t value)
{
	uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
	if (value)
	*byte |= (1 << (y % 8));
	else
	*byte &= ~(1 << (y %8 ));
}

void LCD_setBitmap(const uint8_t * bitArray){
	  for (int i=0; i<504; i++)
	  {
		  char c = pgm_read_byte(&bitArray[i]);
		  nokia_lcd.screen[i] = c;
	  }
}

void LCD_writeChar(char code, uint8_t scale)
{
	register uint8_t x, y;

	for (x = 0; x < 5*scale; x++)
	for (y = 0; y < 7*scale; y++)
	if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
	LCD_setPixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
	else
	LCD_setPixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);

	nokia_lcd.cursor_x += 5*scale + 1;
	if (nokia_lcd.cursor_x >= 84) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y += 7*scale + 1;
	}
	if (nokia_lcd.cursor_y >= 48) {
		nokia_lcd.cursor_x = 0;
		nokia_lcd.cursor_y = 0;
	}
}

void LCD_writeString(const char *str, uint8_t scale)
{
	while(*str)
	LCD_writeChar(*str++, scale);
}

void LCD_setcursor(uint8_t x, uint8_t y)
{
	nokia_lcd.cursor_x = x;
	nokia_lcd.cursor_y = y;
}

void LCD_update(void)
{
	register unsigned i;
	/* Set column and row to 0 */
	LCD_write(0x80, COMMAND);
	LCD_write(0x40, COMMAND);

	/* Write screen to display */
	for (i = 0; i < 504; i++)
	LCD_write(nokia_lcd.screen[i], DATA);
}
#endif /* 5510_CONTROLS_H_ */