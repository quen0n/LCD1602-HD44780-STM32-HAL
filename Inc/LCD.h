/**
 * \file            LCD.h
 * \brief          	Header file with LCD functions prototypes
 * \authors			Quenon
 */

#ifndef LCD_H_
#define LCD_H_

#include "main.h"

/*************** SETTINGS ***************/
//Enable redirect printf to LCD
#define LCD_PRINTF_ENABLE
//Enable Unicode support
//#define LCD_UNICODE_SUPPORT

//Set display interface. Values:
// 0 - i2c interface (SDA, SCL)
// 1 - 4 bit parallel interface (RS, E, D4, D5, D6, D7). RW connected to GND
// 2 - 8 bit parallel interface (RS, E, D0, D1, D2, D3, D4, D5, D6, D7). RW connected to GND
#define LCD_INTERFACE 0

//Включить поддержку кириллических символов
//Внимание! Работает только на дисплеях с русскими символами в памяти!
//Для китайских используйте псевдоподдержку кириллицы
//Если вы используйте STM32 CubeIDE включите поддержку Unicode
//#define LCD_CYRILLIC_SUPPORT

//Включить псевдоподдержку кириллицы
//Если в дисплее нет подходящего символа, то библиотека загружает
//свой с помощью LCD_createChar. Подходит для японских дисплеев
//Если вы используйте STM32 CubeIDE включите поддержку Unicode
//#define LCD_CYRILLIC_PSEUDOSUPPORT

#if LCD_INTERFACE != 0
#define LCD_BL_PORT GPIOB
#define LCD_BL_PIN	GPIO_PIN_6
#endif


typedef enum {
	LCD_LEFT,
	LCD_RIGHT,
	LCD_NONE
} LCD_dir_t;

typedef enum {
	LCD_OFF,
	LCD_ON
} LCD_state_t;

#if LCD_INTERFACE == 0
	//LCD initialization function
	HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *_i2c, uint8_t dAddr, uint8_t width, uint8_t lines);
	//LCD PCF8574-based I2C default address
	#define LCD_I2C_DEFAULT_ADDRESS 0x7E
#else
	//LCD initialization function
	void LCD_init(uint8_t width, uint8_t lines);
#endif


//Function of sending data to LCD
void LCD_sendData(uint8_t data);
//Function of sending instruction to LCD
void LCD_sendCmd(uint8_t data);
//Function of printing char on LCD
void LCD_printChar(char c);
//Function of printing string on LCD
void LCD_printStr(char str[]);
//Cursor position setting function
void LCD_setCursor(uint8_t x, uint8_t y);
//Function of loading custom character into LCD
void LCD_createChar(uint8_t addr, const uint8_t array[8]);
//Function of display cleaning
void LCD_clear(void);
//Function of returning cursor and display position
void LCD_home(void);
//Backlight on/off function
void LCD_backlight(LCD_state_t state);
//Display on/off function
void LCD_display(LCD_state_t state);
//Cursor on/off function
void LCD_cursor(LCD_state_t state);
//Blinks on/off function
void LCD_blinks(LCD_state_t state);
//Cusor shift funtion
void LCD_shiftCursor(LCD_dir_t dir, uint8_t amount);
//Display shift funtion
void LCD_shiftDisplay(LCD_dir_t dir, uint8_t amount);
//Enabling/disabling cursor position control
void LCD_cursorControl(LCD_state_t state);
//Formatted printing function on LCD
void LCD_printf(const char * __restrict format, ...);
#endif

//TODO: Поддержка дисплеев с 8-битной схемой
//TODO: Проверить другие стандартные фичи и написать функции
//TODO: Выбор адреса дисплея для возможности использования двух и более
//TODO: Подробное комментирование функций, шапки по стандарту
//TODO: Набор прикольных значков из знакогенератора (стрелочки, градус Цельсия и т.д)
