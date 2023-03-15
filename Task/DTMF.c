#include "DTMF.h"
#include "math.h"
#include "string.h"
#include "task.h"
#include "oled.h"

#define PIx2				6.2831852		//2*pi
#define fs 					8000			//采样频率
#define MAX_BINS			8

/*
	1	2	3	A			1	2	3	4
	4	5	6	B			5	6	7	8
	7	8	9	C			9	10	11	12
	*	0	#	D			13	14	15	16
*/

char code[17] = {'1', '2', '3', 'a', 			//不知道为什么把这个数组放在DTMF.c中，程序将不能正确返回1,2,3其余均可以正常返回
				 '4', '5', '6', 'b',
				 '7', '8', '9', 'c', 
				 '*', '0', '#', 'd',   'H'}; // DTMF表中的16个ASCII码,H表示错误
/*
	1	2	3	A			1	2	3	4
	4	5	6	B			5	6	7	8
	7	8	9	C			9	15	15	12
	*	0	#	D			13	14	15	16	
*/
				 
uint8_t code_number [17] = {1,   2,    3,   4, 			//不知道为什么把这个数组放在DTMF.c中，程序将不能正确返回1,2,3其余均可以正常返回
				       5,   6,    7,   8,
				       9,   10,   11,  12, 
				       13,  14,   15,  16,  17}; // DTMF表中的16个ASCII码,H表示错误	

uint8_t N_decode=1;	            
uint8_t next_code=0;
int n=1;

uint16_t sound_one[16][400] = {0};				//16个DTMF信号
uint16_t freqs[8] = {697,770, 852, 941, 1209, 1336, 1477,1633};


char det_num[2];
char key1='F' ,key2='F',key3='F';
uint8_t key_number;//录到音的数字
uint8_t key_flag = 0; //录音播音标志位 1的时候录 2的时候播
int8_t key_N=1;





uint8_t data_number =1;//录音数组的位数
uint8_t data_number_COPY =21;//录音数组的位数

float Wf[MAX_BINS]={1.7077,1.6452,1.5686,1.4782,1.1640,0.9963,0.7985,0.5684};// 角频率8000
float Vf[MAX_BINS][3]={0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0,
						0.0,0.0,0.0 }; //8个频率的计算数据				
float Af[MAX_BINS];//频谱值


char find_key(float *Af_buffer);




void DTMF_Decode(float decode_data)		   //只采样计数110个点计算2次，N_decode控制采样计算的点数
{  										   //13.8ms计算出一个点
	int i,j,k;	

	if(N_decode>110)	
	{
		N_decode++;

		return;
	}
	
	for(k=0;k<8;k++)
	{
		Vf[k][0]=Wf[k]*Vf[k][1]-Vf[k][2] + decode_data;
		if(N_decode != 110)	
		{
			Vf[k][2]=Vf[k][1];
			Vf[k][1]=Vf[k][0];
		} 
	}
	
	if(next_code == 1)	
	{
		key3='H';		  //由每20ms判断是否有静音，通个变量next_code来控制key3控制是否输出码值
		next_code=0;	  //当没有静音时key3不会置‘H’，从而控制了多输出码值
	}		//静音时key3 = 'H'
	

	if(N_decode==110) 
	{
		for(k=0;k<8;k++)
		Af[k]=Vf[k][0]*Vf[k][0]+Vf[k][1]*Vf[k][1]-Wf[k]*Vf[k][0]*Vf[k][1]; //计算频谱值

		for(i=0;i<8;i++)	 //为下一个计数初始化Vf[i][j]
			for(j=0;j<3;j++)	
				Vf[i][j]=0.0;
		
		if(key_N==1)
		{
			key1 = find_key(Af);			
			key_number = find_key_number(Af);
			key_N++;
			
			N_decode++;		//每进行一次GOERTZEL，都要加一次N_decode
		
			return;
		}
		else 
		{
			key2=find_key(Af);
			key_N=1;

	
			if(key1==key2 && key1!='H' && key2!='H' && key1!=key3)
			{					
				key3=key1;
				printf("实时检测音的字符：");
				printf("%c\r\n", key1);//打印录到的音的数字字符
				if(key1 != 'H')
				{
					det_num[0] = key1;
					det_num[1] = '\0';
					strcat(show_num, det_num);
					if(strlen(show_num) == 15)
					{
						memset(show_num,'\0',sizeof(show_num));
					}
				}
				
				key1='H';
				key2='H';
				N_decode++;			
				return;
			}
			else
			{
				N_decode++;
				return;
			}
		}
	}
	N_decode++;
}

uint8_t test_DTMF(float *Af_buffer,int low,int high)
{
	uint8_t i;
	
	for (i=0;i<=3;i++)						//低高频段检测,主能量大于其它的5倍以上
		if(i!=low)
			if(Af_buffer[i]*5.0>=Af_buffer[low])	 
				return 1;  // 无效DTMF信号
	
	for (i=4;i<=7;i++)
		if(i!=high)
			if(Af_buffer[i]*5.0>=Af_buffer[high])
				return 1;
	
	return 0;//有效信号
}



char find_key(float *Af_buffer)
{   
	int8_t i,low_freq,high_freq,test_signal=1;
	int8_t tem;

	// 在低频找max
	for (low_freq = 0, i = 1; i < 4; i ++)
	{
		if (Af_buffer[i] > Af_buffer[low_freq])
		{
			low_freq = i;
		}
	}

	// 在高频找max
	for (high_freq = 4, i = 5; i < 8; i ++)
	{
		if (Af_buffer[i] > Af_buffer[high_freq])
		{
			high_freq = i;
		}
	}
	
	test_signal=test_DTMF(Af_buffer,low_freq,high_freq);
	tem = low_freq*4 + high_freq - 4;
		
	if(test_signal==1)   return 'H';	  //终止判定返回H表示错信号	   test_signal==1 
	else if(tem>-1 && tem<16 && test_signal == 0)

	{
		return code[tem];
	}
	
	return	'H';	 //H表示错了
}

 
uint8_t find_key_number (float *Af_buffer)
{   
	int8_t i,low_freq,high_freq,test_signal=1;
	int8_t tem;

	// 在低频找max
	for (low_freq = 0, i = 1; i < 4; i ++)
	{
		if (Af_buffer[i] > Af_buffer[low_freq])
		{
			low_freq = i;
		}
	}

	// 在高频找max
	for (high_freq = 4, i = 5; i < 8; i ++)
	{
		if (Af_buffer[i] > Af_buffer[high_freq])
		{
			high_freq = i;
		}
	}
	
	test_signal=test_DTMF(Af_buffer,low_freq,high_freq);
	tem = low_freq*4 + high_freq - 4;
		
	if(test_signal==1)   return 17;	  //终止判定返回17表示错信号	   test_signal==1 
	else if(tem>-1 && tem<16 && test_signal == 0)

	{
		return code_number [tem];
	}
	
	return	17;	 //17表示错了
}


