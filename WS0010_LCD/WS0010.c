#include "stm32f1xx_hal.h"
#include "rus_font_table.h"
#include "WS0010.h"

void delay_w(void);

unsigned char test_val;

const unsigned char rowAddres[4]={ADDR_1_STRING, ADDR_2_STRING, ADDR_3_STRING, ADDR_4_STRING};
size_t str_len(char const *string);

void LCD_init(void)
{
	HAL_Delay(10);
	DATA_OUT &= ~(LED_PIN_MASK);
	HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_SET);
	DATA_OUT = 0x20;
	HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_RESET);
	lcdWrite(INSTR_FUNC_SET | PARAM_DATA_4BIT | PARAM_TAB_ENG_RUS | PARAM_2_LINES,0);
	lcdWrite(INSTR_ON_OFF | PARAM_ON, 0);
	lcdWrite(INSTR_ENTRY_MODE | PARAM_INC, 0);
	lcdWrite(INSTR_RETURN, 0);
	lcdWrite(INSTR_CLEAR, 0);
}


void lcdWrite(unsigned char data, unsigned char mode)
{
  while( checkBusy() );
  DATA_DDR = (DATA_DDR & ~LED_CRL_MASK) | PORT_OUT_CFG;
  DATA_OUT &= ~(LED_PIND_MASK);
  if(mode==0)   HAL_GPIO_WritePin(GPIOB, LCD_RS, GPIO_PIN_RESET);
  if(mode==1)   HAL_GPIO_WritePin(GPIOB, LCD_RS, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, LCD_RW, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_SET);
  delay_w();
  DATA_OUT |= data;
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_RESET);
  DATA_OUT &= ~(LED_PIND_MASK);
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_SET);
  delay_w();
  DATA_OUT |= (data<<4) & 0xF0;
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_RESET);
  DATA_OUT &= ~(LED_PIND_MASK);
}

unsigned char checkBusy(void)
{
  uint8_t data = 0;
  DATA_DDR = (DATA_DDR & ~LED_CRL_MASK) | PORT_IN_CFG;
  HAL_GPIO_WritePin(GPIOB, LCD_RS, GPIO_PIN_RESET);	//инструкция
  HAL_GPIO_WritePin(GPIOB, LCD_RW, GPIO_PIN_SET);	//чтение
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_SET);
  data = DATA_IN & 0xF0;	        //DATA read
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_SET);
  delay_w();
  data = data | ((DATA_IN & 0xF0) >> 4);
  HAL_GPIO_WritePin(GPIOB, LCD_E, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, LCD_RW, GPIO_PIN_RESET);	//чтение
  if(data & 0x80) return 1;
  else return 0;
}

void LCD_WriteText(char const* string) {
	uint8_t pos = 0;
	while (*string != '\0') {
		switch (*string) {
		case 0x0A:
			//lcdWrite(*string++, 1);		//LF
			SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[1]);
			pos = 0x00;
			string++;
			break;
		case 0x0D:
			//lcdWrite(*string++, 1);		//CR
			SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[1]);
			pos = 0x00;
			string++;
			break;
//		case 0x09:
//			lcdWrite(*string++, 1);		//HT
//			break;
		default:
			if (*(string) == 0xD0 || *(string) == 0xD1) {
				if (*(string + 1) >= 0x90 && *(string + 1) <= 0xBF) {
					lcdWrite((rus_table[(*(string + 1)) - 0x90]), 1);
					string += 2;
				} else if ((*(string + 1) >= 0x80 && *(string + 1) <= 0x8F)) {
					lcdWrite((rus_table[(*(string + 1)) - 80]), 1);
					string += 2;
				}
			} else {
				lcdWrite(*string++, 1);
			}

			switch (pos) {
			case 0x13:
				SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[1]);
				pos = 0x40;
				break;
			case 0x53:
				SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[2]);
				pos = 0x14;
				break;
			case 0x27:
				SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[3]);
				pos = 0x54;
				break;
			case 0x67:
				SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[0]);
				lcdWrite(INSTR_CLEAR, 0);
				pos = 0x00;
				break;
			default:
				pos++;
				break;
			}
			HAL_Delay(100);
			break;
		}
	}
}

//-----------UTF-8---------------//
void writeString(char const* string) {
	unsigned char cnt;
	unsigned char num = str_len(string);
	for (cnt = 0; cnt < num; cnt++) {
		if (*(string) == 0xD0) {
			lcdWrite((rus_table[(*(string + 1)) - 0x90]), 1);
			string += 2;
			cnt++;
		} else {
			switch (*string) {
			case 0x0A:
				lcdWrite(*string++, 1);		//LF
				break;
			case 0x0D:
				lcdWrite(*string++, 1);		//CR
				break;
			case 0x09:
				lcdWrite(*string++, 1);		//HT
				break;
			default:
				lcdWrite(*string++, 1);
				break;
			}
		}
	}
}
//-----------CP1251---------------//
void writeString_1251(char const* string)
{
  unsigned char cnt;
  unsigned char num = str_len(string);
  for(cnt=0; cnt<num; cnt++)
  {
    if(*(string) >= RUS_FONT_OFFSET)
      lcdWrite((rus_table[(*string++)-0xC0]), 1);
    else
      lcdWrite(*string++, 1);
  }
}


void writeStringCntr(char const* string, unsigned char strPos)
{
  unsigned char cnt;
  unsigned char num = str_len(string);
  SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[strPos]);
//  SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[strPos] + ((20-num)>>1));

  writeString(string);
}

void clearString(unsigned char stRow, unsigned char stCol, unsigned char count)
{
  unsigned char i;
  if(stRow-- > 3) 
    return;
  if(stCol + count>20) count = 20 - stCol;
  
  SEND_INSTR(INSTR_SET_DDRAM_ADDR | rowAddres[stRow] + stCol);
  for(i=0; i<count; i++)
  {
    SEND_DATA(' ');
  }
}

size_t str_len(char const *string)
{
  size_t size = 0;
  while( (*string++) != '\0') size++;
  return size;
}


void delay_w(void)
{
  volatile uint32_t i;
  for (i=0; i<1; i++);
  i++;
}
