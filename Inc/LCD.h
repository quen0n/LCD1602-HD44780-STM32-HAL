#ifndef LCD_H_
#define LCD_H_

#include "main.h"

#define LCD_I2C_ADDRES 0x40

//TODO: Псевдоподдержка русского языка
//TODO: Поддержка \r \n
//TODO: Управление подсветкой
//TODO: Слежение за положением курсора, автоперенос текста (может выключаться)
//TODO: Подробное комментирование функций, шапки по стандарту
//TODO: Поддержка обычных дисплеев с 4 и 8-битной схемой
//TODO: Поддержка русского языка для дисплеев, котоыре его реально поддерживают
//TODO: Указание типа дисплея - 1602 или 2004
//TODO: Поддержка шрифта 5х10 (чекать кастом чары)
//TODO: Указывать при инициализации адрес дисплея на шине
//TODO: Функция управления курсором
//TODO: Функция управления вкл/выкл дисплей
//TODO: Функция управления миганием знакоместа
//TODO: Функция очистки дисплея
//TODO: Функция сдвига дисплея
//TODO: Функция home
//TODO: Проверить другие стандартные фичи и написать функции

//LCD initialization function
void LCD_init(I2C_HandleTypeDef *i2c);
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
void LCD_writeCustomChar(uint8_t addr, uint8_t array[8]);

#endif
