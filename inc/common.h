#ifndef __COMMON_H__
#define __COMMON_H__
#include <pthread.h>
typedef unsigned char  BYTE;//1字节
typedef unsigned char  UINT8;
typedef unsigned int   DWORD;//4字节
typedef unsigned short WORD;//2字节
typedef unsigned int   UINT;//4字节

#define FALSE		  		0
#define TRUE		  		1

#define WORDLEN				32
#define UART_RX_BUF_SIZE	512 //串口接收缓存数组大小
#define UART_RX_LEN			512 //预读字节数
#define MSG_MAX_LEN			256

#define UART_CFG_FILE		"/root/tq2440_uart.cfg"//uart configuration file
#define WSN_DATA_FILE		"../files/wsn_data"//WSN data file

#define HEARTBEART_TIMEOUT	30
#define HEARTBEART_PERIOD	5

//BYTE buf[UART_RX_BUF_SIZE];
BYTE uart_msg[MSG_MAX_LEN];//存储串口收到到完整消息

int volatile uart_fd;	//串口文件描述
int volatile dev_id_success;	//设备识别是否成功
int volatile wsn_timer_count;	//底层传感器网络连接计数器
int volatile wsn_connected;		//WSN连接状态
int volatile center_connected;	//监控中心连接状态

BYTE volatile m_heartbeat_timeout;         //心跳检测超时
BYTE volatile m_heartbeat_period;          //服务发送心跳间隔

pthread_mutex_t mutex;


BYTE get_sum(BYTE *p, BYTE len);
#endif /*__COMMON_H__*/
