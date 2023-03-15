/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2023-03-05 23:27:54
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2023-03-07 21:43:39
 * @FilePath: \DTMF-ZE-main\MDK-ARM\task.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "task.h"
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include "DTMF.h"

GPIO_TypeDef* key_ports[] = {GPIOE, GPIOE};
uint16_t key_pins[] = {GPIO_PIN_4, GPIO_PIN_3};
uint8_t key_value = 0;

void IsKeyPressed()
{
	if(HAL_GPIO_ReadPin(key_ports[0], key_pins[0]) == 0)
	{
		HAL_Delay(5);	
		if(HAL_GPIO_ReadPin(key_ports[0], key_pins[0]) == 0)
		{
			OLED_Clear();
			key_value = 0;
		}
	}

	if(HAL_GPIO_ReadPin(key_ports[1], key_pins[1]) == 0)
	{
		HAL_Delay(5);
		if(HAL_GPIO_ReadPin(key_ports[1], key_pins[1]) == 0)
		{
			OLED_Clear();
			key_value = 1;
			playing_flag = 1;
			value = 0;
		}
	}
}


char show_mode[16];
char show_char[16];
char show_num[16];

void oled_show()
{
	switch(key_value)
	{
		case 0:
		{
			
			sprintf(show_mode, "Detection...");
			sprintf(show_char, "Detected num is");
			
			//sprintf(show_num, "               ");
			OLED_ShowString(2,0,(unsigned char *)show_mode,16);
			OLED_ShowString(2,2,(unsigned char *)show_char,16);
			OLED_ShowString(2,6,(unsigned char *)show_num,16);
		}break;
		
		case 1:
		{
			sprintf(show_mode, "Record Mode...");
			OLED_ShowString(2,0,(unsigned char *)show_mode,16);
		}break;

		
		default:
			break;
	}
	  
}

void AllTask()
{
	IsKeyPressed();
	
	oled_show();
}
