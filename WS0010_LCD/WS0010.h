
#ifndef __WS0010_H__
#define __WS0010_H__

#include "main.h"

//lib WEH002004 4-wire

//PIN config

#define     LED_PORT              GPIOB
#define     LED_OUT               LED_PORT->ODR
#define     LED_PIN_RS            LED_PIN_RS_Pin          // PB8
#define     LED_PIN_RW            LED_PIN_RW_Pin          // PB9
#define     LED_PIN_EN            LED_PIN_EN_Pin          // PB10
#define     LED_PIN_D7            LED_PIN_D7_Pin          // PB7
#define     LED_PIN_D6            LED_PIN_D6_Pin          // PB6
#define     LED_PIN_D5            LED_PIN_D5_Pin          // PB5
#define     LED_PIN_D4            LED_PIN_D4_Pin          // PB4
#define     LED_PIN_MASK   (LED_PIN_RS | LED_PIN_RW | LED_PIN_EN | LED_PIN_D7 | LED_PIN_D6 | LED_PIN_D5 | LED_PIN_D4)
#define     LED_PIND_MASK  (LED_PIN_D7 | LED_PIN_D6 | LED_PIN_D5 | LED_PIN_D4)
#define 	LED_CRL_MASK 	      0xFFFF0000			  //GPIOB->CRL

#define DATA_DDR	GPIOB->CRL
#define DATA_OUT	GPIOB->ODR
#define DATA_IN		GPIOB->IDR

#define LCD_RS		LED_PIN_RS				//data/instruction
#define LCD_RW		LED_PIN_RW				//read/write
#define LCD_E	    LED_PIN_EN				//strobe

#define PORT_IN_CFG 	0x44440000			//Input mode, Floating input   PB4,PB5,PB6,PB7
#define PORT_OUT_CFG    0x22220000			//Output mode, 2MHz, Push-pull PB4,PB5,PB6,PB7

//LCD instructions
#define INSTR_CLEAR			    0x01	//clear display

#define INSTR_RETURN			0x02	//return home position

#define INSTR_ENTRY_MODE 		0x04
#define PARAM_SHIFT		        0x01
#define PARAM_INC		      	0x02

#define INSTR_ON_OFF		  	0x08
#define PARAM_ON 				0x04
#define PARAM_CURSOR 		  	0x02
#define PARAM_BLINK 		  	0x01

#define INSTR_SHIFT		    	0x10
#define PARAM_RIGHT		    	0x04
#define PARAM_DISP	        	0x08

#define INSTR_MODE		        0x13
#define PARAM_GR		      	0x08
#define PARAM_PWR		      	0x04

#define INSTR_FUNC_SET			0x20
#define PARAM_DATA_8BIT			0x10
#define PARAM_DATA_4BIT			0x20

#define PARAM_2_LINES		   	0x08
#define PARAM_FONT		 		0x04
#define PARAM_TAB_EUR1			0x01
#define PARAM_TAB_ENG_RUS		0x02
#define PARAM_TAB_EUR2			0x03

#define INSTR_SET_DDRAM_ADDR	        0x80

#define INSTR_SET_CGRAM_ADDR            0x40

#define ADDR_1_STRING			0x00
#define ADDR_2_STRING			0x40
#define ADDR_3_STRING			0x14
#define ADDR_4_STRING			0x54

#define RUS_FONT_OFFSET			0x80u

//Macros
#define SEND_INSTR(instr)		lcdWrite(instr, 0)
#define SEND_DATA(data)			lcdWrite(data, 1)
#define LCD_CLEAR()			    SEND_INSTR(INSTR_CLEAR)
#define WRITE_SPACE()			lcdWrite(0x20, 1)

//Function prototypes
void lcdWrite(unsigned char data, unsigned char mode);
void LCD_init(void);
void LCD_WriteText(char const* string);
unsigned char checkBusy(void);
void writeString(char const* string);
void writeStringCntr(char const* string, unsigned char pos);
void clearString(unsigned char stRow, unsigned char stCol, unsigned char count);
extern const unsigned char rowAddres[];

#endif
