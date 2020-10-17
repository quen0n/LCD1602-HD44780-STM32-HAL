#include "LCD.h"
static I2C_HandleTypeDef *i2c;
static uint8_t DA;
//Variable for storing the bus value
static uint8_t bus = 0x08;
//Display on/off control value
static uint8_t displayControlValue = 0x0C;
//Bus data recording function
static HAL_StatusTypeDef _writeBus(void) {
	uint8_t busBuff[1] = {bus};
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2c, DA, 10, 0xFF);
	if (status != HAL_OK) return status;
	return HAL_I2C_Master_Transmit(i2c, DA, busBuff, 1, 0xFF);
}

//Function of sending half a byte of information
static void _sendHalfByte(uint8_t data, uint8_t RSState) {
	if (RSState) bus |= 0x01; else bus &= 0xFE;
	bus |= (1<<2);
	bus &= 0x0F;
	bus |= data<<4;
	_writeBus();
	bus &= ~(1<<2);
	_writeBus();
}
//Function of sending instruction to LCD
void LCD_sendCmd(uint8_t cmd) {
	_sendHalfByte(cmd>>4, 0);
	_sendHalfByte(cmd, 0);
}
//Function of sending data to LCD
void LCD_sendData(uint8_t data) {
	_sendHalfByte(data>>4, 1);
	_sendHalfByte(data, 1);
}
//Display initialization function
void LCD_init(I2C_HandleTypeDef *_i2c, uint8_t dAddr) {
	i2c = _i2c;
	DA = dAddr;
	HAL_Delay(40);

	_sendHalfByte(0b0011, 0);
	HAL_Delay(4);
	_sendHalfByte(0b0011, 0);
	_sendHalfByte(0b0011, 0);
	_sendHalfByte(0b0010, 0);

	LCD_sendCmd(0b00101000);
	LCD_sendCmd(displayControlValue);
	LCD_clear();
	LCD_sendCmd(0b00000110);
}
//Function of printing char on LCD
void LCD_printChar(char c) {
	LCD_sendData(c);
}
//Function of printing string on LCD
void LCD_printStr(char str[]) {
	for(uint8_t i = 0; str[i]; i++) LCD_sendData(str[i]);
}
//Cursor position setting function
void LCD_setCursor(uint8_t x, uint8_t y) {
	LCD_sendCmd(0b10000000 | (x + y*0x40));
}
//Function of loading custom character into LCD
void LCD_writeCustomChar(uint8_t addr, uint8_t array[8]) {
	LCD_sendCmd(0b01000000 | (addr*8));
	for(uint8_t i = 0; i < 8; i++) {
		LCD_sendData(array[i]);
	}
	LCD_setCursor(0, 0);
}
//Function of display cleaning
void LCD_clear(void) {
	LCD_sendCmd(0x01);
	HAL_Delay(2);
}
//Function of returning cursor and display position
void LCD_home(void) {
	LCD_sendCmd(0x03);
	HAL_Delay(2);
}

//Backlight on/off function
void LCD_backlight(uint8_t state) {
	if (state) {
		bus |= (1<<3);
	} else {
		bus &= ~(1<<3);
	}
	_writeBus();
}
//Display on/off function
void LCD_display(uint8_t state) {
	if (state) {
		displayControlValue |= (1<<2);
	} else {
		displayControlValue &= ~(1<<2);
	}
	LCD_sendCmd(displayControlValue);
}
//Cursor on/off function
void LCD_cursor(uint8_t state) {
	if (state) {
		displayControlValue |= (1<<1);
	} else {
		displayControlValue &= ~(1<<1);
	}
	LCD_sendCmd(displayControlValue);
}
//Blinks on/off function
void LCD_blinks(uint8_t state) {
	if (state) {
		displayControlValue |= (1<<0);
	} else {
		displayControlValue &= ~(1<<0);
	}
	LCD_sendCmd(displayControlValue);
}
