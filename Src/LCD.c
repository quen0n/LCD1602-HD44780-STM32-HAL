/**
 * \file            LCD.c
 * \brief           File with functions for working with LCD
 * \authors			Quenon
 */

#include "LCD.h"

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
	if(c == '\r') {
		LCD_setCursor(0, currentY);
		return;
	}
	if(c == '\n') {
		LCD_setCursor(currentX, ++currentY);
		return;
	}
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
	LCD_setCursor(0, 0);
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
 * \param[in]       state: Display backlight status. 0 - backlight off, 1 - backlight on
 */
void LCD_backlight(uint8_t state) {
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
 * \param[in]       state: Display status. 0 - off, 1 - on
 */
void LCD_display(uint8_t state) {
	if (state) {
		displayControlValue |= (1<<2);
	} else {
		displayControlValue &= ~(1<<2);
	}
	LCD_sendCmd(displayControlValue);
}
/**
 * \brief           Cursor on/off function
 * \param[in]       state: Display backlight status. 0 - backlight off, 1 - backlight on
 */
void LCD_cursor(uint8_t state) {
	if (state) {
		displayControlValue |= (1<<1);
	} else {
		displayControlValue &= ~(1<<1);
	}
	LCD_sendCmd(displayControlValue);
}
/**
 * \brief 			Character blinks on/off function
 * \param[in]		state: Blink familiar status. 0 - off, 1 - on
 */
void LCD_blinks(uint8_t state) {
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
 * \param[in]		state: 1 - the library will automatically move the cursor when leaving the display, 0 - off
 */
void LCD_cursorControl(uint8_t state) {
	cursorControl = state;
}
