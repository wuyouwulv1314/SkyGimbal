/*
 * =====================================================================================
 *
 *       Filename:  my_uart.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014/6/15 20:38:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "my_usart.h"
#include "stm32f10x.h"
#include "UARTtools.h"
#include "DataReceiver.h"
#include <stdio.h>


void USART_Config(void)/*{{{*/
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	//GPIO_PinRemapConfig(GPIO_FullRemap_USART3 , ENABLE);
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				   //PA10Ϊ��������RX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		   //���߷�ת�ٶ�Ϊ2MHz	 �������ֻ��115.2k����ô��2M��GPIO�������ٶȾ͹��ˣ���ʡ��Ҳ����С
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				   //PA9Ϊ�������TX 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;	 //��ʼ����������
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART3,&USART_ClockInitStructure);

	USART_InitStructure.USART_BaudRate = 115200;				   //���ô��ڲ���
	USART_InitStructure.USART_WordLength =USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART3,&USART_InitStructure);

	USART_ITConfig(USART3,USART_IT_TXE,ENABLE);
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);

	USART_Cmd(USART3,ENABLE);
}
/*}}}*/

void USART3_IRQHandler(void)/*{{{*/
{
	uint16_t USART3_RxData;
	uint8_t temp8 = 0;
	if(USART_GetFlagStatus(USART3,USART_FLAG_TXE) == SET)
	{
		temp8 = 1;
		if((UART_SendCurrentAddr[Tool_USART3] != 0))
		{
			//user process
			UARTsendNext(Tool_USART3);
		}
		else
		{
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}
	if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE) == SET)
	{
		temp8 = 1;
		USART3_RxData = USART_ReceiveData(USART3);
		//user process
		//loop
//		USART_SendData(USART1,USART1_RxData);
//		DRReceiver(USART1_RxData);
//		UARTreceiveDnC(Tool_USART1,USART1_RxData);
	}
	if(temp8 == 0)
	{
		//user process
		USART_ReceiveData(USART3);
		UART_SetErrorCommand(Tool_USART3);
	}
}/*}}}*/


/*******************************************************************************
* Function Name  : int fputc(int ch, FILE *f)
* Description    : Retargets the C library printf function to the USART.printf�ض���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
  /* Write a character to the USART */
  USART_SendData(USART1, (u8) ch);

  /* Loop until the end of transmission */
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET))
  {
  }

  return ch;
}

/*******************************************************************************
* Function Name  : int fgetc(FILE *f)
* Description    : Retargets the C library printf function to the USART.fgetc�ض���
* Input          : None
* Output         : None
* Return         : ��ȡ�����ַ�
*******************************************************************************/
int fgetc(FILE *f)
{
  /* Loop until received a char */
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET))
  {
  }
  
    /* Read a character from the USART and RETURN */
  return (USART_ReceiveData(USART1));
}

