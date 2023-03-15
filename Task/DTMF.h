#ifndef _DTMF_H
#define _DTMF_H	

//#include "stm32f10x.h"
#include "main.h"
#include "stdio.h"

extern uint8_t N_decode;	            
extern uint8_t next_code;

extern uint16_t sound_one[16][400];
extern char code[17];
extern uint8_t code_number [17];
extern uint8_t key_data[40];
void DTMF_Produc_Init(void);
uint8_t find_key_number (float *Af_buffer);
void DTMF_Decode(float decode_data);		   //ֻ��������110�������2�Σ�N_decode���Ʋ�������ĵ���
//float filter(float b[],int Length_b,float ADC_CovnData);
void generate_dtmf_signal(uint8_t digit, uint16_t* output_buffer);

extern char key1,key2,key3;
extern uint8_t key_flag;
extern uint8_t key_number;
extern uint8_t data_number ;
extern uint8_t data_number_COPY ;
extern char key_data_char[40] ; //��¼�����������ַŵ��ַ�����
extern char key_data_char_COPY[40];
#endif

