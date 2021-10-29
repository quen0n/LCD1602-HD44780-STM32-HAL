/**
 * \file            LCD.h
 * \brief          	Header file with LCD functions prototypes
 * \authors			Quenon
 */


#ifndef LCD_H_
#define LCD_H_

#include "main.h"

#define LCD_I2C_ADDRES 0x40

typedef enum {
	LCD_LEFT,
	LCD_RIGHT
} LCD_dir_t;

//LCD initialization function
HAL_StatusTypeDef LCD_init(I2C_HandleTypeDef *i2c, uint8_t displayAddress);
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
void LCD_backlight(uint8_t state);
//Display on/off function
void LCD_display(uint8_t state);
//Cursor on/off function
void LCD_cursor(uint8_t state);
//Blinks on/off function
void LCD_blinks(uint8_t state);
//Cusor shift funtion
void LCD_shiftCursor(LCD_dir_t dir, uint8_t amount);
//Display shift funtion
void LCD_shiftDisplay(LCD_dir_t dir, uint8_t amount);
#endif

//TODO: Указание типа дисплея - 1602 или 2004
//TODO: Подробное комментирование функций, шапки по стандарту
//TODO: Проверить другие стандартные фичи и написать функции
//TODO: Слежение за положением курсора, автоперенос текста (может выключаться)
//TODO: Поддержка \r \n
//TODO: Псевдоподдержка русского языка
//TODO: Поддержка обычных дисплеев с 4 и 8-битной схемой
//TODO: Поддержка русского языка для дисплеев, которые его реально поддерживают
//TODO: Поддержка шрифта 5х10 (чекать кастом чары)
//TODO: Как насчёт LCD_printf?
//TODO: Добавить состояния ON или OFF с помощью typedef
