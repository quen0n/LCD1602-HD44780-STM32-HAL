#include "LCD.h"
I2C_HandleTypeDef *i2c;

//Инициализация переменной данных шины дисплея
static uint8_t bus = 0x08;

//Функция записи данных на шину
static HAL_StatusTypeDef _writeBus(void) {
	uint8_t busBuff[1] = {bus};
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2c, LCD_I2C_ADDRES, 10, 0xFF);
	if (status != HAL_OK) return status;
	return HAL_I2C_Master_Transmit(i2c, LCD_I2C_ADDRES, busBuff, 1, 0xFF);
}

static void _sendHalfByte(uint8_t data, uint8_t RSState) {
	if (RSState) bus |= 0x01; else bus &= 0xFE;
	bus |= (1<<2); 	//Подъём E
	bus &= 0x0F;	//Очистка старшей части шины
	bus |= data<<4;	//Добавление данных в старшую часть шины
	_writeBus();	//Отправка данных на шину
	bus &= ~(1<<2); //Опускание E
	_writeBus();	//Отправка данных на шину
}

void LCD_sendCmd(uint8_t cmd) {
	_sendHalfByte(cmd>>4, 0);
	_sendHalfByte(cmd, 0);
}

void LCD_sendData(uint8_t data) {

	_sendHalfByte(data>>4, 1);
	_sendHalfByte(data, 1);

}

void LCD_init(I2C_HandleTypeDef *_i2c) {
	i2c = _i2c;

	HAL_Delay(40);

	_sendHalfByte(0b0011, 0);
	HAL_Delay(4);
	_sendHalfByte(0b0011, 0);
	_sendHalfByte(0b0011, 0);
	_sendHalfByte(0b0010, 0);

	LCD_sendCmd(0b00101000);
	LCD_sendCmd(0b00001100);
	LCD_sendCmd(0b00000001);
	HAL_Delay(2);
	LCD_sendCmd(0b00000110);
}

void LCD_print(char str[]) {
	for(uint8_t i = 0; str[i]; i++) LCD_sendData(str[i]);
}

void LCD_setCursor(uint8_t x, uint8_t y) {
	LCD_sendCmd(0b10000000 | (x + y*0x40));
}

void LCD_writeCustomChar(uint8_t addr, uint8_t array[8]) {
	LCD_sendCmd(0b01000000 | (addr*8));
	for(uint8_t i = 0; i < 8; i++) {
		LCD_sendData(array[i]);
	}
	LCD_setCursor(0, 0);
}
