#ifndef __UART_OP_H__
#define __UART_OP_H__

#include <termios.h>
#include "common.h"
typedef struct
{
	char dev[WORDLEN];
	unsigned int  speed;
	unsigned char databits;
	unsigned char stopbits;
	unsigned char parity;
}uart_cfg_t;
struct termios opt, old_opt;//串口参数结构体
int uart_init(void);
int uart_read_fix(int fd, BYTE *str, DWORD len);//读取固定长度的数据
int uart_read_left(int fd, BYTE *str);//读取缓冲区剩余所有数据
int uart_read_msg(int fd, BYTE *str);//读取整个消息包
int uart_send(int fd, BYTE *str, BYTE len);

#endif /*__UART_OP_H__*/
