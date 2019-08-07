


#include "rf.h"
#include "tim.h"

#define HIGH	10000
#define LOW		20000
#define START	1000

typedef enum bit {
	BIT_NULL, BIT_START, BIT_CMD
} Bit;

Bit bit_tx = BIT_NULL;

volatile uint8_t cmd_tx;


uint16_t cmd_arr[250];
uint16_t *ptrcmd_arr=cmd_arr;

void recive(void);
void transmit(void);

void rfSendCmd(uint8_t x) {

	cmd_tx = 0;
	for (uint8_t i = 7, y = 0; i < 8; i--, y++) {
		cmd_tx |= ((x >> i) & 0x01) << y;
	}
	bit_tx = BIT_START;
	TIM2->ARR = START;
	HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);

}

///////---------------отправка посылки----------------/////////

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim == &htim2) {
		transmit();
	}
}

void transmit(void) {

	static uint16_t count = 0;
	switch (bit_tx) {
	case BIT_START:
		count++;
		if (count == 501) {
			count = 0;
			bit_tx = BIT_CMD;
		}
		break;
	case BIT_CMD:
		if (cmd_tx & 0x01) {
			TIM2->ARR = HIGH;
		} else {
			TIM2->ARR = LOW;
		}
		count++;
		cmd_tx >>= 1;
		if (count == 9) {
			HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
			TIM2->CNT = 0;
			count = 0;
		}
		break;
	default:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
		bit_tx = BIT_NULL;
		count = 0;
		break;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == BUTTON_Pin) {
		HAL_TIM_Base_Start_IT(&htim4);
	}
	if (GPIO_Pin == IN_Pin) {
		recive();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim4) {
		volatile static uint8_t countButtonPress = 0;
		countButtonPress++;
		if (countButtonPress == 200) {
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			rfSendCmd(0xAA);
			HAL_TIM_Base_Stop_IT(&htim4);
			countButtonPress = 0;
		}
	}
}

///////---------------прием посылки----------------/////////

void recive(void) {

	//TIM3 (100ns)

	static uint8_t count = 0;
	TIM3->CR1 = 0;
	uint16_t time = TIM3->CNT;
//	*ptrcmd_arr++ = time;


		if (time > HIGH - 1000 && time < HIGH + 1000) {
			cmd_rx <<= 1;
			cmd_rx |= 0x01;

		} else if (time > LOW - 1000 && time < LOW + 1000) {
			cmd_rx <<= 1;

		} else {
			count = 0;
		}
		count++;

		if (count == 8) {
			count = 0;
//			*ptrcmd_arr++ = cmd_rx;
		}

	TIM3->CNT = 0;
	TIM3->CR1 = TIM_CR1_CEN;
}
