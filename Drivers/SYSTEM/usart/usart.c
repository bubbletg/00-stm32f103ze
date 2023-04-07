#include "main.h"
#include "./usart.h"
#include <stdio.h>

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "includes.h" /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1
/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


/* FILE 在 stdio.h里面定义. */
FILE __stdout;

/* MDK下需要重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
	  // SR 状态寄存器 位6(01000000 即0X40) 为发送完成标志位
    while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
		// DR 数据寄存器
    USART1->DR = (uint8_t)ch;             /* 将要发送的字符 ch 写入到DR寄存器 */
    return ch;
}
#endif
/******************************************************************************************/

// 接收状态标记
uint8_t G_USART_RX_STA = 0;

// 串口接收缓冲区
uint8_t G_USART_RX_BUF[USART_REC_LEN]; // 接收缓冲,最大USART_REC_LEN个字节.

// UART 句柄
UART_HandleTypeDef G_UART_InitStruct;

/**
 * @brief  USART 初始化
 * @param  baudrate 波特率
 */
void usart_init(uint32_t baudrate)
{
	// 串口初始化
	G_UART_InitStruct.Instance = USART1;			   // 串口1
	G_UART_InitStruct.Init.BaudRate = baudrate;			   // 波特率
	G_UART_InitStruct.Init.WordLength = UART_WORDLENGTH_8B; // 字长为8位数据格式
	G_UART_InitStruct.Init.StopBits = UART_STOPBITS_1;	   // 一个停止位
	G_UART_InitStruct.Init.Parity = UART_PARITY_NONE;	   // 无奇偶校验位
	G_UART_InitStruct.Init.HwFlowCtl = UART_HWCONTROL_NONE; // 无硬件流控
	G_UART_InitStruct.Init.Mode = UART_MODE_TX_RX;		   // 收发模式
	HAL_UART_Init(&G_UART_InitStruct);				   // 初始化串口

	/* 该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量 */
	HAL_UART_Receive_IT(&G_UART_InitStruct, (uint8_t *)G_USART_RX_BUF, USART_REC_LEN);
}

/**
 * @brief  串口中断回调函数
 * @param  huart 串口句柄
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	if (huart->Instance == USART1)
	{
		// 使能 GPIO 时钟
		__HAL_RCC_GPIOA_CLK_ENABLE();
		// 使能串口时钟
		__HAL_RCC_USART1_CLK_ENABLE();

		// 配置串口引脚
		GPIO_InitStruct.Pin = GPIO_PIN_9;		// PA9
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // 复用推挽输出
		GPIO_InitStruct.Pull = GPIO_PULLUP;		// 上拉
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		 // 初始化 PA9

		GPIO_InitStruct.Pin = GPIO_PIN_10;		// PA10
		GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT; // 复用输入
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		 // 初始化 PA10

		// 使能串口中断
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		// 串口中断优先级
		HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
	}
}

/**
 * @brief  串口中断服务函数
 */
void USART1_IRQHandler(void)
{
	// 串口中断服务函数
	HAL_UART_IRQHandler(&G_UART_InitStruct);
	// 接收中断
	HAL_UART_Receive_IT(&G_UART_InitStruct, (uint8_t *)G_USART_RX_BUF, USART_REC_LEN);
}

/**
 * @brief  串口数据接收回调函数
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
	// 	// 串口接收数据回调函数
	// 	if ((G_USART_RX_BUF[0] != 0x0d) && (G_USART_RX_BUF[0] != 0x0a))
	// 	{
			// 接收到的数据不是回车换行
			G_USART_RX_STA = 1;
		// }
	}
}


void my_printf(char *fmt, uint8_t length)
{
	// HAL_UART_Transmit 发送数据
  HAL_UART_Transmit(&G_UART_InitStruct, (uint8_t *)fmt, length, 1000);
  // 等等发送完成
  while (__HAL_UART_GET_FLAG(&G_UART_InitStruct, UART_FLAG_TC) != 1);
}






