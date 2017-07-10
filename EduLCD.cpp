/*
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DogLcd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DogLcd.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2014 Robert Wessels <make@energia.nu>
 *
 * Based on Arduino library for Dog character LCD's
 * http://code.google.com/p/doglcd/
 * Copyright: 2010 Eberhard Fahle e.fahle@wayoda.org
 * License: GNU Lesser GPL
 * 
 * Based on the Educational BoosterPack Library
 * http://embedXcode.weebly.com
 * Copyright: 2013 Rei Vilo
 * License: CC = BY NC SA
 */

#include <EduLCD.h>
#include <SPI.h>

#define LCD_COMMAND 0
#define LCD_DATA 1

EduLCD::EduLCD(uint8_t RS)
{
	this->RS = RS;
}

void EduLCD::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	if (lines > 1) {
	_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_currline = 0;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
	_displayfunction |= LCD_5x10DOTS;
	}

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);

	pinMode(RS, OUTPUT);

	/* Wakeup */
	send(LCD_COMMAND, 0x30);
	delay(2);
	send(LCD_COMMAND, 0x30);
	send(LCD_COMMAND, 0x30);

	/* Function Set: Instruction Table Select */
	send(LCD_COMMAND, 0x21);
	/* Internal Oscillator Frequency */
	send(LCD_COMMAND, 0x14);
	/* Power Control */
	send(LCD_COMMAND, 0x56);
	/* follower control */
	send(LCD_COMMAND, 0x6D);
	/* Contrast */
	send(LCD_COMMAND, 0x70);
	/* Auto-ident */
	send(LCD_COMMAND, 0x0c);
	/* Function Set: 8bit/2lines */
	send(LCD_COMMAND, 0x38);

	/* Left to Right */
	leftToRight();
	/* Clear */
	clear();
	/* Display on */
	display();
	/* Line 1 column 0 */
	home();
}

void EduLCD::send(uint8_t mode, uint8_t data)
{
	digitalWrite(RS, mode);
	SPI.transfer(data);
	delay(1);
}

inline size_t EduLCD::write(uint8_t value) {
	send(LCD_DATA, value);
}

void EduLCD::clear()
{
	send(LCD_COMMAND, LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void EduLCD::home()
{
	send(LCD_COMMAND, LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

// Turn the display on/off (quickly)
void EduLCD::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}

void EduLCD::display() {
	_displaycontrol |= LCD_DISPLAYON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void EduLCD::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}
void EduLCD::cursor() {
	_displaycontrol |= LCD_CURSORON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void EduLCD::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}
void EduLCD::blink() {
	_displaycontrol |= LCD_BLINKON;
	send(LCD_COMMAND, LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void EduLCD::scrollDisplayLeft(void) {
	send(LCD_COMMAND, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void EduLCD::scrollDisplayRight(void) {
	send(LCD_COMMAND, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void EduLCD::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	send(LCD_COMMAND, LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void EduLCD::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	send(LCD_COMMAND, LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void EduLCD::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	send(LCD_COMMAND, LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void EduLCD::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	send(LCD_COMMAND, LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void EduLCD::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	send(LCD_COMMAND, LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

void EduLCD::setCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row >= _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}

	send(LCD_COMMAND, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void EduLCD::setContrast(uint8_t contrast) {
	send(LCD_COMMAND, 0x39);
	send(LCD_COMMAND, 0x70 | (contrast & 0x0f));
}

