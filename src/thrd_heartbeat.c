#include <stdio.h>
#include <stdlib.h>	/*atoi和exit等*/
#include <unistd.h>	/*close, read, write, sleep等*/
#include "common.h"
#include "message.h"
#include "uart_opt.h"
#include "thrd_heartbeat.h"

void send_heartbeat_to_coor();//发送心跳给协调器

/*#include <time.h>
struct timeval t_start, t_end;
gettimeofday(&t_start, NULL);
long start = ((long)t_start.tv_sec)*1000 + (long)t_start.tv_usec/1000;
gettimeofday(&t_end, NULL);
long end = ((long)t_end.tv_sec)*1000 + (long)t_end.tv_usec/1000;*/

void *thrd_net_connect(void *arg)
{
	int res;
	int thrd_no = (int)arg;
	while (1)
	{
		sleep(m_heartbeat_period);
		//上锁后再向串口发送数据防止与主进程冲突
		res = pthread_mutex_lock(&mutex);
		if (res)
		{
			printf("MSG：Thread %d lock failed!", thrd_no);
			pthread_exit(NULL);
		}
		wsn_timer_count++;
		
		if (dev_id_success == FALSE)
		{
			printf("MSG：WSN is disconnected!\n");
			wsn_timer_count = 0;
		}
		else
		{
			send_heartbeat_to_coor();//发送心跳给协调器	
			printf("MSG：Heartbeat sent to coor!\n");
			if (wsn_timer_count >= (m_heartbeat_timeout / 5))
			{
				printf("MSG：Time out! System is disconnected!\n");
				dev_id_success  = FALSE;
				wsn_connected   = FALSE;
				wsn_timer_count = 0;
			}
		}
		pthread_mutex_unlock(&mutex);
	}
}
/*******************************************************************************
*函数名：		send_heartbeat_to_coor
*参数：		无
*返回值：		无
*描述： 		发送心跳消息到协调器
*******************************************************************************/
void send_heartbeat_to_coor()
{
	s_comm_msg_t s_msg;
	s_msg.head      = MSG_HEAD;
	s_msg.len       = sizeof(s_comm_msg_t);
	s_msg.session   = S_SESSION_ID;
	s_msg.msg_code  = S_HEART_DETECTION;
	s_msg.check_sum = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}
