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
 * \return          I2C status
 */
HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *_i2c, uint8_t dAddr) {
	i2c = _i2c;
	DA = dAddr;
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
	LCD_sendData(c);
}
/**
 * \brief           Function of printing string on LCD
 * \param[in]       str: Pointer to array with string
 */
void LCD_printStr(char str[]) {
	for(uint8_t i = 0; str[i]; i++) LCD_sendData(str[i]);
}
/**
 * \brief           Cursor position setting function
 * \param[in]       x: Horizontal position 0-15 (0-19 for 20x4)
 * \param[in]       y: Vertical position 0-1 (0-3 for 20x4)
 */
void LCD_setCursor(uint8_t x, uint8_t y) {
	LCD_sendCmd(0x80 | (x + y*0x40));
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
	HAL_Delay(2);
}
/**
 * \brief           Function of returning cursor and display position
 */
void LCD_home(void) {
	LCD_sendCmd(0x03);
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
