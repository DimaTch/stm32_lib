#ifndef RF_H_
#define RF_H_

//**************************************************//
#include "stm32f1xx_hal.h"
#include "main.h"
//**************************************************//
//72MHz
//TIM2_CH1 - генератор команды 	PSC=7, ARR=999; CRR1 = 1. T=100ns

//------------ PA0 - TX (TIM2.CH1 - OC mode) ------------//
//------------ PA1 - Button (EXTI)-----------------------//
//------------ PA2 - RX (EXTI)---------------------------//

//TIM4_Base	-	дребезг кнопки	PSC=799; ARR=99.	T=10mks
//TIM3_Base	-	вычисление длительности принятых импульсов	PSC=7; ARR=65000. T=100ns

volatile uint8_t cmd_rx;

void rfSendCmd(uint8_t x);


#endif /* RF_H_ */
