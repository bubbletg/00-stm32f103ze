#ifndef __USART_H__
#define __USART_H__

#define USART_REC_LEN 200 //定义最大接收字节数 200


// 接收状态标记
extern uint8_t G_USART_RX_STA;

// 串口接收缓冲区
extern uint8_t G_USART_RX_BUF[USART_REC_LEN]; // 接收缓冲
// UART 句柄
extern UART_HandleTypeDef G_UART_InitStruct;


void usart_init(uint32_t baudrate);

#endif