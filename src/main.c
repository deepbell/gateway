#include <stdio.h>
#include <stdlib.h>	/*atoi和exit等*/
#include <unistd.h>	/*close, read, write, sleep等*/
#include <pthread.h>
#include <errno.h>
#include <linux/string.h>
#include "common.h"
#include "message.h"
#include "uart_opt.h"
#include "thrd_heartbeat.h"
#include "msg_process.h"
#include "socket.h"


/*-----------------------------------------------------------------------------
  函数名:  main
  参  数:  int argc, char **argv
  返回值:  
  描  述:  
*-----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	//FILE *fd;
	int res;
	dev_id_success      = FALSE;
	wsn_connected       = FALSE;
	center_connected	= FALSE;
	wsn_timer_count     = 0;
	m_heartbeat_timeout = HEARTBEART_TIMEOUT;
	m_heartbeat_period  = HEARTBEART_PERIOD;
	uart_fd             = uart_init();//返回串口文件描述符
    p_msg_header_t msg_head;
	pthread_t thrd_net_connect_id;//网络连接维持线程
	//pthread_t thrd_socket
	/******test: uart rxd and txd****/
	#if 0
	while(1){
		memset(buf, 0, sizeof(buf));
		//memset(bufr, 0, sizeof(bufr));
		sleep(1);
		uart_read_fix(uart_fd, buf, 10);
		uart_send(uart_fd, buf);
	}
	#endif
	printf("UART connection completed!\n");
	
	srand(time(NULL));
	//网络连接维持线程
	res = pthread_create(&thrd_net_connect_id, NULL, thrd_net_connect, (void *)0);
	if (res != 0)
	{
		printf("Create Thread 0 failed!\n");
		exit(res);
	}
	// //Socket连接线程
	// res = pthread_create(&thrd_uart_connect_id, NULL, thrd_uart_connect, (void *)1);
	// if (res != 0)
	// {
	// 	printf("Create Thread 1 failed!\n");
	// 	exit(res);
	// }
	while(1)
	{
		do
		{
			memset(uart_msg, 0, sizeof(uart_msg));
			uart_read_msg(uart_fd, uart_msg);
			msg_head = (p_msg_header_t)uart_msg;
		}while (get_sum(uart_msg, msg_head->len) != uart_msg[msg_head->len - 1]);//读取整个包，且判断校验和是否正确，不正确再重新读取包
		
		if (center_connected)
		{
			//转发消息
		}
		else
		{
			msg_process(msg_head);
		}
		
		//printf("Testing is OK!\n");
		
				
		













		//memset(buf, 0, sizeof(buf));
		//memset(bufr, 0, sizeof(bufr));

		//connect_coor(uart_fd);
		//close(uart_fd);
		//sleep(2);//5s

		/*uart_read_left(uart_fd, buf);

		if((fd = fopen(WSN_DATA_FILE, "a+")) == NULL){
			printf("Cannot open file, strike any key to exit!\n");
			getchar();
			exit(1);
		}

//		printf("%d,%d\n", buf[0], buf[9]);

/////////////////sa
		for(i =0; i < sizeof(buf); i++)
		{
			#if 0
			if(buf[i] < 0)
			{
				//buf[i] = abs(buf[i]);
				printf("\"%d\"", i);
				printf("%d ", buf[i]);
			}
			#endif
			fprintf(fd, "%c", buf[i]);
			printf("%c", buf[i]);
			//if((i + 1) % 26 == 0)printf("\n");
		}
		fwrite(buf, sizeof(buf[0]), sizeof(buf), fd);
		fclose(fd);//完成1次写操作(fwrite())后必须关闭流(fclose());

		if((fd = fopen("../files/wsn_data", "a+")) == NULL){
			printf("Cannot open file, strike any key to exit!\n");
			getchar();
			exit(1);
		}
		fread(bufr, sizeof(buf[0]), sizeof(buf), fd);
		printf("\n\nstring = %s\n\n", bufr);*/
		//fclose(fd);
		//tcsetattr(uart_fd, TCSANOW, &old_opt);//还原配置
		//close(uart_fd);
	}
	return 0;
}
