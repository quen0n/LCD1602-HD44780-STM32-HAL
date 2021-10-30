/**
 * \file            LCD.c
 * \brief           File with functions for working with LCD
 * \authors			Quenon
 */

#include "LCD.h"

#ifdef LCD_PRINTF_ENABLE
#include <stdio.h>
#include <stdarg.h>
#endif



//I2C interface pointer
static I2C_HandleTypeDef *i2c;
//LCD I2C address
static uint8_t DA;

//Variable for storing the bus value
static uint8_t bus = 0x08;
//Display on/off control value
static uint8_t displayControlValue = 0x0C;
//Enabling/disabling cursor position control value
static uint8_t cursorControl = 1;

static uint8_t dWidth, dLines;
static uint8_t currentX = 0, currentY = 0;

#ifdef LCD_CYRILLIC_SUPPORT
/**
 * \brief           Converting a Cyrillic character from ASCII to a display character generator table
 * \param[in]       c: ASCII character
 * \return          LCD CGRAM character
 */
static char _ASCIItoDisplay(char c) {
	const char cyrillicAlphabet[64] = {
	  //A    Б    В    Г    Д    Е    Ж    З    И    Й    К    Л    М    Н    О    П
		0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
	  //Р    С    Т    У    Ф    Х    Ц    Ч    Ш    Щ    Ъ    Ы    Ь    Э    Ю    Я
		0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,0xAC,0xE2,0xAD,0xAE,0x62,0xAF,0xB0,0xB1,
	  //а    б    в    г    д    е    ж    з    и    й    к    л    м    н    о    п
		0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
	  //р    с    т    у    ф    х    ц    ч    ш    щ    ъ    ы    ь    э    ю    я
		0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
	};
	return cyrillicAlphabet[c-192];
}
#endif

#ifdef LCD_CYRILLIC_PSEUDOSUPPORT
const uint8_t cyrillicBitmaps[][8] = {
	{0x1F, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x1E, 0x00}, //Б
	{0x1F, 0x11, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00}, //Г
	{0x15, 0x15, 0x15, 0x0E, 0x15, 0x15, 0x15, 0x00}, //Ж
	{0x11, 0x11, 0x13, 0x15, 0x19, 0x11, 0x11, 0x00}, //И
	{0x0A, 0x04, 0x11, 0x13, 0x15, 0x19, 0x11, 0x00}, //Й
	{0x0F, 0x05, 0x05, 0x05, 0x05, 0x15, 0x09, 0x00}, //Л
	{0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00}, //П
	{0x04, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x04, 0x00}, //Ф
	{0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x1F, 0x01}, //Ц
	{0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x00}, //Ш
	{0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x01}, //Щ
	{0x18, 0x08, 0x08, 0x0E, 0x09, 0x09, 0x0E, 0x00}, //Ъ
	{0x11, 0x11, 0x11, 0x19, 0x15, 0x15, 0x19, 0x00}, //Ы
	{0x0E, 0x11, 0x01, 0x07, 0x01, 0x11, 0x0E, 0x00}, //Э
	{0x12, 0x15, 0x15, 0x1D, 0x15, 0x15, 0x12, 0x00}, //Ю
	{0x0F, 0x11, 0x11, 0x0F, 0x05, 0x09, 0x11, 0x00}, //Я
	{0x00, 0x00, 0x0F, 0x05, 0x09, 0x1F, 0x11, 0x00}, //д
	{0x00, 0x00, 0x15, 0x15, 0x0E, 0x15, 0x15, 0x00}, //ж
	{0x00, 0x04, 0x04, 0x0E, 0x15, 0x15, 0x0E, 0x04}, //ф
	{0x00, 0x00, 0x12, 0x12, 0x12, 0x12, 0x1F, 0x01}, //ц
	{0x00, 0x00, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x00}, //ш
	{0x00, 0x00, 0x15, 0x15, 0x15, 0x15, 0x1F, 0x01}, //щ
	{0x00, 0x00, 0x18, 0x08, 0x0E, 0x09, 0x0E, 0x00}, //ъ
	{0x00, 0x00, 0x11, 0x11, 0x19, 0x15, 0x19, 0x00}, //ы
	{0x00, 0x00, 0x0E, 0x11, 0x07, 0x11, 0x0E, 0x00}, //э
	{0x00, 0x00, 0x12, 0x15, 0x1D, 0x15, 0x12, 0x00}, //ю
	{0x00, 0x00, 0x0F, 0x11, 0x0F, 0x09, 0x11, 0x00}, //я
};

/**
 * \brief           Converting a Cyrillic character from ASCII to a display character generator table
 * \param[in]       c: ASCII character
 * \return          LCD CGRAM character
 */
static char _ASCIItoDisplay(char c) {
	const char cyrillicAlphabet[64] = {
	  //A    Б    В    Г    Д    Е    Ж    З    И    Й    К    Л    М    Н    О    П
		'A', 0,  'B',  1,   'D', 'E', 2,  '3',  3,   4,  'K',  5,  'M', 'H', 'O',  6,
	  //Р    С    Т    У    Ф    Х    Ц    Ч    Ш    Щ    Ъ    Ы    Ь    Э    Ю    Я
	    'P', 'C', 'T', 'Y', 7,  'X',  8,   '4', 9,   10,  11,  12, 'b',  13,  14,  15,
	  //а    б    в    г    д    е    ж    з    и    й    к    л    м    н    о    п
		'a', '6', 'B', 0xD4,16,  'e', 17,  '3', 'u', 'u', 'k', 0xF7,'m', 'H', 'o', 'n',
	  //р    с    т    у    ф    х    ц    ч    ш    щ    ъ    ы    ь    э    ю    я
		'p', 'c', 'T', 'y', 18, 'x',  19,  '4', 20,  21,  22,  23,  'b', 24,  25,  26
	};
	if(cyrillicAlphabet[c-192] < 32) {
		static uint8_t i = 0;
		LCD_createChar(i, cyrillicBitmaps[(uint8_t)(cyrillicAlphabet[c-192])]);
		uint8_t a = i;
		if(++i > 7) i = 0;
		return a;
	}


	return cyrillicAlphabet[c-192];
}
#endif



/**
 * \brief           Bus data recording function
 * \return          I2C status
 */
static HAL_StatusTypeDef _writeBus(void) {
	uint8_t busBuff[1] = {bus};
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2c, DA, 10, 0xFF);
	if (status != HAL_OK) return status;
	return HAL_I2C_Master_Transmit(i2c, DA, busBuff, 1, 0xFF);
}
/**
 * \brief           Function of sending half a byte of information
 * \param[in]       data: Information to be sent to LCD
 * \param[in]       RSState: State of RS. 0 - instruction, 1 - data
 */
static void _sendHalfByte(uint8_t data, uint8_t RSState) {
	if (RSState) bus |= 0x01; else bus &= 0xFE;
	bus |= (1<<2);
	bus &= 0x0F;
	bus |= data<<4;
	_writeBus();
	bus &= ~(1<<2);
	_writeBus();
}

/**
 * \brief           Function of sending instruction to LCD
 * \param[in]       cmd: Instruction to be sent to LCD
 */
void LCD_sendCmd(uint8_t cmd) {
	_sendHalfByte(cmd>>4, 0);
	_sendHalfByte(cmd, 0);
}
/**
 * \brief           Function of sending data to LCD
 * \param[in]       data: Data to be sent to LCD
 */
void LCD_sendData(uint8_t data) {
	_sendHalfByte(data>>4, 1);
	_sendHalfByte(data, 1);
}
/**
 * \brief           Display initialization function
 * \param[in]       _i2c: Pointer to I2C interface
 * \param[in]       dAddr: LCD address on I2C bus
 * \param[in]       width: Number of characters per line
 * \param[in]       lines: Number of display lines
 * \return          I2C status
 */
HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *_i2c, uint8_t dAddr, uint8_t width, uint8_t lines) {
	i2c = _i2c;
	DA = dAddr;
	dWidth = width;
	dLines = lines;
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2c, DA, 10, 0xFF);
	if (status != HAL_OK) return status;

	HAL_Delay(40);

	_sendHalfByte(0x03, 0);
	HAL_Delay(4);
	_sendHalfByte(0x03, 0);
	_sendHalfByte(0x03, 0);
	_sendHalfByte(0x02, 0);

	LCD_sendCmd(0x28);
	LCD_sendCmd(displayControlValue);
	LCD_clear();
	LCD_sendCmd(0x06);
	return HAL_OK;
}
/**
 * \brief           Function of printing char on LCD
 * \param[in]       c: LCD printable symbol
 */
void LCD_printChar(char c) {
	//Carriage return
	if(c == '\r') {
		LCD_setCursor(0, currentY);
		return;
	}
	//Line break
	if(c == '\n') {
		LCD_setCursor(currentX, ++currentY);
		return;
	}
	//Screen cleaning
	if(c == '\f') {
		LCD_clear();
		return;
	}

#ifdef LCD_UNICODE_SUPPORT
	static char highByte = 0;
	if (highByte) {
		if (highByte ==  0xD0) c = ((uint16_t) highByte<<8 | c) - 0xCFD0;
		if (highByte ==  0xD1) c = ((uint16_t) highByte<<8 | c) - 0xD090;
		highByte = 0;
		#ifdef LCD_CYRILLIC_SUPPORT
		if (c == 0xB1) c = 0xA2; //Ё
		if (c == 0x01) c = 0xB5; //ё
		#endif
		#ifdef LCD_CYRILLIC_PSEUDOSUPPORT
		if (c == 0xB1) c = 'E'; //Ё
		if (c == 0x01) c = 'e'; //ё
		#endif
	} else if(c == 0xD0 || c == 0xD1) {
		highByte = c;
		return;
	}
#endif

#ifdef LCD_CYRILLIC_SUPPORT
	if (c >= 192) c = _ASCIItoDisplay(c);
#endif
#ifdef LCD_CYRILLIC_PSEUDOSUPPORT
	if (c >= 192) c = _ASCIItoDisplay(c);
#endif
	LCD_sendData(c);

	currentX++;
	if(cursorControl) {
		if (currentX >= dWidth)  {
			currentX = 0;
			if (++currentY >= dLines) currentY = 0;
			LCD_setCursor(currentX, currentY);
		}
	}
}
/**
 * \brief           Function of printing string on LCD
 * \param[in]       str: Pointer to array with string
 */
void LCD_printStr(char str[]) {
	for(uint8_t i = 0; str[i]; i++) LCD_printChar(str[i]);
}
/**
 * \brief           Cursor position setting function
 * \param[in]       x: Horizontal position 0-15 (0-19 for 20x4)
 * \param[in]       y: Vertical position 0-1 (0-3 for 20x4)
 */
void LCD_setCursor(uint8_t x, uint8_t y) {
	if(cursorControl) {
		if (x >= dWidth) x = 0;
		if (y >= dLines) y = 0;
	}
	currentX = x;
	currentY = y;

	switch(y) {
	case 0:
		LCD_sendCmd(x | 0x80);
		break;
	case 1:
		LCD_sendCmd((0x40 + x) | 0x80);
		break;
	case 2:
		LCD_sendCmd((0x14 + x) | 0x80);
		break;
	case 3:
		LCD_sendCmd((0x54 + x) | 0x80);
		break;
	}
}
/**
 * \brief           Function of loading custom character into LCD
 * \note			The function does not print a character, but only loads it into the display memory.
 * 					To print characters use LCD_printChar()
 * \param[in]       addr: The address where the symbol will be located 0-7
 * \param[in]       array: Pointer to array with 8 lines of user character
 */
void LCD_createChar(uint8_t addr, const uint8_t array[8]) {
	LCD_sendCmd(0x40 | (addr*8));
	for(uint8_t i = 0; i < 8; i++) {
		LCD_sendData(array[i]);
	}
	LCD_setCursor(currentX, currentY);
}
/**
 * \brief           Function of display cleaning
 * \note			Function erases text from display and insert cursor at position 0,0
 */
void LCD_clear(void) {
	LCD_sendCmd(0x01);
	currentX = 0;
	currentY = 0;
	HAL_Delay(2);
}
/**
 * \brief           Function of returning cursor and display position
 */
void LCD_home(void) {
	LCD_sendCmd(0x03);
	currentX = 0;
	currentY = 0;
	HAL_Delay(2);
}
/**
 * \brief           Backlight on/off function
 * \param[in]       state: Display backlight status. LCD_OFF - backlight off, LCD_ON - backlight on
 */
void LCD_backlight(LCD_state_t state) {
	if (state) {
		bus |= (1<<3);
	} else {
		bus &= ~(1<<3);
	}
	_writeBus();
}
/**
 * \brief           Display on/off function
 * \note			The function does not clear the display.
 * 					After turning on the display, the previous image returns
 * \param[in]       state: Display status. LCD_OFF - off, LCD_ON - on
 */
void LCD_display(LCD_state_t state) {
	if (state) {
		displayControlValue |= (1<<2);
	} else {
		displayControlValue &= ~(1<<2);
	}
	LCD_sendCmd(displayControlValue);
}
/**
 * \brief           Cursor on/off function
 * \param[in]       state: Display backlight status. LCD_OFF - backlight off, LCD_ON - backlight on
 */
void LCD_cursor(LCD_state_t state) {
	if (state) {
		displayControlValue |= (1<<1);
	} else {
		displayControlValue &= ~(1<<1);
	}
	LCD_sendCmd(displayControlValue);
}
/**
 * \brief 			Character blinks on/off function
 * \param[in]		state: Blink familiar status. LCD_OFF - off, LCD_ON - on
 */
void LCD_blinks(LCD_state_t state) {
	if (state) {
		displayControlValue |= (1<<0);
	} else {
		displayControlValue &= ~(1<<0);
	}
	LCD_sendCmd(displayControlValue);
}
/**
 * \brief 			Cusor shift funtion
 * \param[in]		dir: Direction - LCD_LEFT or LCD_RIGHT
 * \param[in]		amount: The number of characters by which the cursor will be moved
 */
void LCD_shiftCursor(LCD_dir_t dir, uint8_t amount) {
	if ((currentX + amount >= dWidth) && cursorControl) amount = dWidth-currentX-1;
	currentX += amount;
	while(amount > 0) {
		LCD_sendCmd(0x10 | (dir << 2));
		amount--;
	}
}

/**
 * \brief 			Display shift funtion
 * \param[in]		dir: Direction - LCD_LEFT or LCD_RIGHT
 * \param[in]		amount: The number of characters by which the display will be moved
 */
void LCD_shiftDisplay(LCD_dir_t dir, uint8_t amount) {
	while(amount) {
		LCD_sendCmd(0x18 | (dir << 2));
		amount--;
	}
}
/**
 * \brief 			Enabling/disabling cursor position control
 * \param[in]		state: LCD_ON - the library will automatically move the cursor when leaving the display, LCD_OFF - off
 */
void LCD_cursorControl(LCD_state_t state) {
	cursorControl = state;
}


//Overriding standard output function
int _write(int file, char *ptr, int len) {
	LCD_printStr(ptr);
	return len;
}

#ifdef LCD_PRINTF_ENABLE
/**
 * \brief 			Formatted printing function on LCD
 * \param[in]		format: formatted string like a printf
 */
void LCD_printf(const char * __restrict format, ...) {
	va_list argptr;
	va_start(argptr, format);
	vfprintf(stderr, format, argptr);
	va_end(argptr);
}
#endif
