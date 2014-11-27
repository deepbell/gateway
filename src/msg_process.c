#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "message.h"
#include "uart_opt.h"
#include "msg_process.h"

void send_dev_identify_ack();//应答设备识别消息
void send_dev_num_msg();//发送设备序号消息
void send_first_conn_msg();//发送第一个上层应用连接消息
void send_get_node_info(WORD addr);//获取协调器节点信息
void send_set_sampling(WORD addr, WORD rsrc_code, WORD rate, WORD mode);//向扩展板发送采样设置消息
void send_set_led_state(WORD addr, BYTE mo_brd, BYTE d1, BYTE d2, BYTE d3, BYTE d4);//设置母板LED状态
void send_set_buzzer_state(WORD addr, BYTE mo_brd, BYTE buz);//设置母板蜂鸣器状态
void send_set_relay_state(WORD addr, BYTE mo_brd, BYTE relay_v);//设置母板继电器状态，只有智能板（1）有继电器
void send_set_ex_relay_state(WORD addr, BYTE relay_v);//设置扩展板继电器状态

/*******************************************************************************
*函数名：	msg_process
*参数：		p_msg_header_t msg
*返回值：	无返回值
*描述： 	处理完整且正确的串口消息包
*******************************************************************************/
void msg_process(p_msg_header_t msg_h)
{
	int res, i;
	if (0 == msg_h->session)//服务消息
	{
		switch (msg_h->msg_code)//消息代码
		{
			case S_DEVICE_IDENTIFICATION:	//设备识别消息
				//若设备识别不成功，再响应协调器的设备识别消息。
				if (dev_id_success == FALSE)
				{
					//将uart_msg强制转换，也可以将msg_h强制转换，因为他们指向同一个地址
					m_heartbeat_timeout = ((p_s_dev_id_msg_t)uart_msg)->heartbeat_timeout;
					m_heartbeat_period  = ((p_s_dev_id_msg_t)uart_msg)->heartbeat_period;
					//......设置定时器
					
					res = pthread_mutex_lock(&mutex);	//上互斥锁
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
					send_dev_identify_ack();	//应答设备识别消息
					pthread_mutex_unlock(&mutex);	//解互斥锁
					
					usleep(300000);//300ms

					res = pthread_mutex_lock(&mutex);
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
					send_dev_num_msg();		//发送设备序号消息
					dev_id_success = TRUE;
					pthread_mutex_unlock(&mutex);

					printf("MSG：设备识别成功！\n");
				}/*end if*/
				break;
				
			case S_HEART_DETECTION:		//心跳检测消息
				printf("MSG：协调器发来心跳消息！\n");
				res = pthread_mutex_lock(&mutex);
				if(res)
				{
					printf("MSG：Thread lock failed!");
					pthread_exit(NULL);
				}/*end if*/
				wsn_timer_count = 0;
				printf("MSG：sys_timer_count reseted!\n");
				pthread_mutex_unlock(&mutex);
				break;
				
			case S_DEVICE_NUMBER:	//协调器对设备序号的应答
				if (((p_s_comm_resp_msg_t)uart_msg)->resp_code == RIGHT_MESSAGE)
				{
					res = pthread_mutex_lock(&mutex);
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
					send_first_conn_msg();	//发送第一个上层应用连接消息
					pthread_mutex_unlock(&mutex);
					usleep(300000);
					printf("MSG：设备序号应答成功！\n");
				}/*end if*/
				break;
			
			case S_SERVICE_TO_COORDINATOR:	//第一个上层应用连接服务的应答
				if (((p_s_comm_resp_msg_t)uart_msg)->resp_code == RIGHT_MESSAGE)
				{
					printf("MSG：第一个应用连接应答成功。\n");
					
					res = pthread_mutex_lock(&mutex);
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
					wsn_connected = TRUE;
					send_get_node_info(0);	//获取协调器节点信息
					pthread_mutex_unlock(&mutex);
					printf("\t====================\n");
					printf("\t====================\n");
				}/*end if*/
				break;
				
			default:break;
		}/*end switch (msg_h->msg_code)*/
	}
	else	//资源消息
	{
		if (dev_id_success == FALSE)
		{
			return;
		}/*end if*/
		switch (msg_h->msg_code)	//消息代码
		{
			case NODE_INFO:		//节点信息
				if (((p_node_info_t)msg_h)->node_addr == 0)
				{
					printf("MSG：没有获取到节点\n");
					//return;
				}/*end if*/
				printf("\t====节点信息====\n");
				printf("节点地址：%#x\n", ((p_node_info_t)msg_h)->node_addr);//%#表示的输出提示方式，如果是8进制，在前面加0，如果是十进制，不加任何字符，如果是十六进制，会加上0x
				printf("IEEE地址");
				for (i = 7; i >= 0; --i)
				{
					printf("：%x", ((p_node_info_t)msg_h)->ieeeAddress[i]);
				}
				printf("\n");
				printf("RSSI：%d dBm\n", ((p_node_info_t)msg_h)->rssi);
				printf("LQI：%d\n", ((p_node_info_t)msg_h)->lqi);
				printf("PAN ID：%d\n", ((p_node_info_t)msg_h)->pan_id);

				if (1 == ((p_node_info_t)msg_h)->mother_board)
				{
					printf("母板类型：智能版\n");
					res = pthread_mutex_lock(&mutex);
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
					send_set_relay_state(((p_node_info_t)msg_h)->node_addr, ((p_node_info_t)msg_h)->mother_board, 0x0A);//继电器2、4闭合，1、3断开
					pthread_mutex_unlock(&mutex);
					usleep(300000);
				}
				else if (2 == ((p_node_info_t)msg_h)->mother_board)
				{
					printf("母板类型：电源板\n");
				}
				else
				{
					printf("母板类型：未知\n");
				}/*end if*/
				//获取此母板数据。只能先发设置命令，协调器才回复资源数据消息
				res = pthread_mutex_lock(&mutex);
					if(res)
					{
						printf("MSG：Thread lock failed!");
						pthread_exit(NULL);
					}/*end if*/
				send_set_led_state(((p_node_info_t)msg_h)->node_addr, ((p_node_info_t)msg_h)->mother_board, 1, 0, 1, 0);
				pthread_mutex_unlock(&mutex);
				usleep(300000);
				res = pthread_mutex_lock(&mutex);
				if(res)
				{
					printf("MSG：Thread lock failed!");
					pthread_exit(NULL);
				}/*end if*/
				send_set_buzzer_state(((p_node_info_t)msg_h)->node_addr, ((p_node_info_t)msg_h)->mother_board, 0);//0：关闭蜂鸣器，1：开启蜂鸣器
				pthread_mutex_unlock(&mutex);
				usleep(300000);
				//若resource_code为resCode_Current_Dual，需要发送获取传感器校准信息：SendGetCalInfo(((NODE_INFO*)pFrame)->nodeAddress);
				printf("扩展板类型代码：%#x\n", ((p_node_info_t)msg_h)->resource_code);
				//获取此扩展板数据。先发送采集开始命令，才能得到数据
				res = pthread_mutex_lock(&mutex);
				if(res)
				{
					printf("MSG：Thread lock failed!");
					pthread_exit(NULL);
				}/*end if*/
				//先设置采样控制命令，才能获取到扩展板资源数据，包括继电器数据
				send_set_sampling(((p_node_info_t)msg_h)->node_addr, ((p_node_info_t)msg_h)->resource_code, 500, 2);

				if (((p_node_info_t)msg_h)->resource_code == 0x2020)
				{
					usleep(300000);
					send_set_ex_relay_state(((p_node_info_t)msg_h)->node_addr, 0xFF);//继电器2、4闭合，1、3断开
				}
				pthread_mutex_unlock(&mutex);
				usleep(300000);

				//将数据写入文件
				printf("\t====================\n");
				break;

			case NODE_EXPORT_RESOURCE_DATA:	//节点扩展板资源数据
				if (((p_ex_rs_msg_header_t)msg_h)->node_addr == 0)
				{
					printf("MSG：没有获取到节点\n");
					//return;
				}
				else
				{
					if (((p_ex_rs_msg_header_t)msg_h)->resource_code == 0)
					{
						printf("MSG：没有获取到资源\n");
						return;
					}/*end if*/
				}/*end if*/
				printf("\t====节点扩展板资源数据====\n");
				printf("节点地址：%#x\n", ((p_ex_rs_msg_header_t)msg_h)->node_addr);
				switch (((p_ex_rs_msg_header_t)msg_h)->resource_code)
				{
					case resCode_Temp_Humidity_Light:
						//calculate_value((p_three_rs_data_t)->resource_data1, (p_three_rs_data_t)->resource_data2, (p_three_rs_data_t)->resource_data3);
						printf("温度：%.1f °C\n", (float)(((p_three_rs_data_t)msg_h)->resource_data1*1));
						printf("湿度：%.1f %%\n", (float)(((p_three_rs_data_t)msg_h)->resource_data2*1));
						printf("光照：%.1f lx\n", (float)(((p_three_rs_data_t)msg_h)->resource_data3*3.3*913/4/8192));
						break;
						//写入文件
					case resCode_Relay_GPIN:
						printf("继电器1状态：%d\n", ((p_relay_gpin_control_t)msg_h)->relay & 0x01);
						printf("继电器2状态：%d\n", ((p_relay_gpin_control_t)msg_h)->relay & 0x02);
						printf("继电器3状态：%d\n", ((p_relay_gpin_control_t)msg_h)->relay & 0x04);
						printf("继电器4状态：%d\n", ((p_relay_gpin_control_t)msg_h)->relay & 0x08);
						//界面显示
						break;

					default:break;
				}/*end switch ((p_ex_rsrc_msg_header_t)msg_h)->resource_code)*/
				printf("\t====================\n");
				break;

			case NODE_MOTHER_RESOURCE_DATA:	//节点母板资源数据
				if (((p_mo_rs_msg_header_t)msg_h)->node_addr == 0)
				{
					printf("MSG：没有获取到节点\n");
					//return;
				}
				else
				{
					if (((p_mo_rs_msg_header_t)msg_h)->resource_code == 0)
					{
						printf("MSG：没有获取到资源\n");
						return;
					}/*end if*/
				}/*end if*/
				printf("\t====节点母板资源数据====\n");
				printf("节点地址：%#x\n", ((p_mo_rs_msg_header_t)msg_h)->node_addr);
				switch (((p_mo_rs_msg_header_t)msg_h)->resource_code)
				{
					case resCode_Relay://协调器母板(智能板)才有继电器
						printf("继电器1状态：%d\n", ((p_mo_relay_control_t)msg_h)->relay & 0x01);
						printf("继电器2状态：%d\n", ((p_mo_relay_control_t)msg_h)->relay & 0x02);
						printf("继电器3状态：%d\n", ((p_mo_relay_control_t)msg_h)->relay & 0x04);
						printf("继电器4状态：%d\n", ((p_mo_relay_control_t)msg_h)->relay & 0x08);
						//界面显示
						break;

					case resCode_Led:
						printf("LED1状态：%d\n", ((p_mo_led_control_t)msg_h)->led1);
						printf("LED2状态：%d\n", ((p_mo_led_control_t)msg_h)->led2);
						printf("LED3状态：%d\n", ((p_mo_led_control_t)msg_h)->led3);
						printf("LED4状态：%d\n", ((p_mo_led_control_t)msg_h)->led4);
						//界面显示
						break;

					case resCode_Buzzer:
						printf("蜂鸣器状态：%d\n", ((p_mo_buzzer_control_t)msg_h)->buzzer);
						//界面显示
						break;

					default:break;
				}/*end switch ((p_mo_rsrc_msg_header_t)msg_h)->resource_code)*/
				printf("\t====================\n");
				break;

			case NODE_PARENT:	//节点父子关系
				printf("\t====节点父子关系====\n");
				printf("节点地址：%#x  父节点地址：%#x\n", ((p_node_parent_msg_t)msg_h)->node_addr, ((p_node_parent_msg_t)msg_h)->pnode_addr);
				res = pthread_mutex_lock(&mutex);
				if(res)
				{
					printf("MSG：Thread lock failed!");
					pthread_exit(NULL);
				}/*end if*/
				send_get_node_info(((p_node_parent_msg_t)msg_h)->node_addr);
				usleep(300000);
				send_get_node_info(((p_node_parent_msg_t)msg_h)->pnode_addr);
				pthread_mutex_unlock(&mutex);
				printf("\t====================\n");
				break;

			case NODE_CAL_INFO://
				break;

			default:break;
		}/*end switch (msg_h->msg_code)*/
	}/*end if(0 == msg_h->session)*/
}




/*******************************************************************************
*函数名：	send_dev_identify_ack
*参数：		无
*返回值：	无
*描述： 	发送设备识别应答消息
*******************************************************************************/
void send_dev_identify_ack()
{
	s_comm_resp_msg_t s_msg;//一般应答消息
	s_msg.head      = MSG_HEAD;
	s_msg.len       = sizeof(s_comm_resp_msg_t);//7;
	s_msg.session   = S_SESSION_ID;
	s_msg.msg_code  = S_DEVICE_IDENTIFICATION;
	s_msg.resp_code = RIGHT_MESSAGE;
	s_msg.check_sum = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}

/*******************************************************************************
*函数名：	send_dev_num_msg
*参数：		无
*返回值：	无
*描述： 	发送设备序号消息
*******************************************************************************/
void send_dev_num_msg()
{
	s_dev_mun_msg_t s_msg;//设备序号消息
	s_msg.head      = MSG_HEAD;
	s_msg.len       = sizeof(s_dev_mun_msg_t);
	s_msg.session   = S_SESSION_ID;
	s_msg.msg_code  = S_DEVICE_NUMBER;
	s_msg.dev_num   = 0x01;//设备序号
	s_msg.check_sum = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}

/*******************************************************************************
*函数名：	send_first_conn_msg
*参数：		无
*返回值：	无
*描述： 	发送第一个上层应用连接消息
*******************************************************************************/
void send_first_conn_msg()
{
	s_comm_msg_t s_msg;
	s_msg.head      = MSG_HEAD;
	s_msg.len       = sizeof(s_comm_msg_t);
	s_msg.session   = S_SESSION_ID;
	s_msg.msg_code  = S_SERVICE_TO_COORDINATOR;
	s_msg.check_sum = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);	
}

/*******************************************************************************
*函数名：	send_get_node_info
*参数：		WORD addr
*返回值：	无
*描述： 	发送获取节点信息消息
*******************************************************************************/
void send_get_node_info(WORD addr)
{
	n_comm_msg_t s_msg;
	s_msg.head      = MSG_HEAD;
	s_msg.len       = sizeof(n_comm_msg_t);
	s_msg.session   = N_SESSION_ID;
	s_msg.msg_code  = GET_NODE_INFO;

	s_msg.node_addr = addr;
	s_msg.check_sum = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}

/*******************************************************************************
*函数名：	send_set_sampling
*参数：		WORD addr, WORD rsrc_code, WORD rate, WORD mode
*返回值：	无
*描述： 	向扩展板发送采样设置消息
*******************************************************************************/
void send_set_sampling(WORD addr, WORD rsrc_code, WORD rate, WORD mode)
{
	ex_sampling_control_t s_msg;
	s_msg.head              = MSG_HEAD;
	s_msg.len               = sizeof(ex_sampling_control_t);
	s_msg.session           = N_SESSION_ID;
	s_msg.msg_code          = SET_NODE_EXPORT_RESOURCE_DATA;
	
	s_msg.node_addr         = addr;
	s_msg.resource_code     = rsrc_code;
	
	s_msg.sampling_accuracy = 0;
	s_msg.sample_rate       = rate; //500(ms)
	s_msg.sample_mode       = mode;//2：连续采集，0：停止采样
	s_msg.sample_interval   = 0;//ms
	s_msg.sample_points     = 0;//采样点数 0表示无限制
	s_msg.packet_len        = 0;	
	s_msg.check_sum         = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}

//设置母板LED状态
void send_set_led_state(WORD addr, BYTE mo_brd, BYTE d1, BYTE d2, BYTE d3, BYTE d4)
{
	mo_led_control_t s_msg;
	s_msg.head          = MSG_HEAD;
	s_msg.len           = sizeof(mo_led_control_t);
	s_msg.session       = N_SESSION_ID;
	s_msg.msg_code      = SET_NODE_MOTHER_RESOURCE_DATA;
	
	s_msg.node_addr     = addr;
	s_msg.mother_board  = mo_brd;//母板类型
	s_msg.resource_code = resCode_Led;
	
	s_msg.led1          = d1;
	s_msg.led2          = d2;
	s_msg.led3          = d3;
	s_msg.led4          = d4;
	s_msg.check_sum     = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}
//设置母板蜂鸣器状态
void send_set_buzzer_state(WORD addr, BYTE mo_brd, BYTE buz)
{
	mo_buzzer_control_t s_msg;
	s_msg.head          = MSG_HEAD;
	s_msg.len           = sizeof(mo_buzzer_control_t);
	s_msg.session       = N_SESSION_ID;
	s_msg.msg_code      = SET_NODE_MOTHER_RESOURCE_DATA;
	
	s_msg.node_addr     = addr;
	s_msg.mother_board  = mo_brd;//母板类型
	s_msg.resource_code = resCode_Buzzer;
	s_msg.buzzer        = buz;
	s_msg.check_sum     = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}
//设置母板继电器状态，只有智能板（1）有继电器
void send_set_relay_state(WORD addr, BYTE mo_brd, BYTE relay_v)
{
	mo_relay_control_t s_msg;
	s_msg.head          = MSG_HEAD;
	s_msg.len           = sizeof(mo_relay_control_t);
	s_msg.session       = N_SESSION_ID;
	s_msg.msg_code      = SET_NODE_MOTHER_RESOURCE_DATA;
	
	s_msg.node_addr     = addr;
	s_msg.mother_board  = mo_brd;//1;//母板类型
	s_msg.resource_code = resCode_Relay;
	s_msg.relay         = relay_v;
	s_msg.check_sum     = get_sum((BYTE *)&s_msg, s_msg.len);
	uart_send(uart_fd, (BYTE *)&s_msg, s_msg.len);
}
//设置扩展板继电器状态
void send_set_ex_relay_state(WORD addr, BYTE relay_v)
{
	relay_gpin_control_t s_msg;
	s_msg.head          = MSG_HEAD;
	s_msg.len           = sizeof(relay_gpin_control_t);
	s_msg.session       = N_SESSION_ID;
	s_msg.msg_code      = SET_NODE_EXPORT_RESOURCE_DATA;
	
	s_msg.node_addr     = addr;
	s_msg.resource_code = resCode_Relay_GPIN;
	s_msg.relay         = relay_v;
	s_msg.check_sum     = get_sum((BYTE *)&s_msg, s_msg.len);
}

// void CRS232_Wireless::SetSerial( BYTE InSerialnum,BYTE InBaudrate )
// {
// 	SERIAL_CONTROL serial_control;
// 	serial_control.head = FRAME_HEAD;
// 	serial_control.length = sizeof(SERIAL_CONTROL);
// 	serial_control.session = 1;
// 	serial_control.messageCode = SetNodeExportResourceData;
// 	serial_control.nodeAddress = nodeAddress;
// 	serial_control.resourcecode = resCode_RS232_Wireless;
// 	serial_control.serialnum = InSerialnum;
// 	serial_control.serialmode = 0x01;
// 	serial_control.baudrate = InBaudrate;
// 	serial_control.parity = 0x00;
// 	serial_control.databit = 8;
// 	serial_control.stopbit = 0x00;
// 	serial_control.flowcontrol = 0x00;
// 	serial_control.checkSum = 0;
// 	for (int i = 0;i<(sizeof(SERIAL_CONTROL)-1);i++)
// 	{
// 		serial_control.checkSum += ((BYTE*)(&serial_control))[i];
// 	}

// 	theApp.m_sSensor.Write(&serial_control,sizeof(SERIAL_CONTROL));
// }

// void CRS232_Wireless::SendData( BYTE InSerialnum,CString szSend )
// {
// 	CString temp = szSend;
// 	char *tempsend = new char[temp.GetLength()];


// 	CUtility::UnsignedShortToCharp(temp.GetBuffer(0),tempsend);

// 	//printf("strlen(tempsend) = %d \n",strlen(tempsend));
// 	//printf("tempsend = %s \n",tempsend);

// 	BYTE *serialdata = new BYTE[strlen(tempsend)+13];

// 	((SERIALDATA*)serialdata)->head = FRAME_HEAD;
// 	((SERIALDATA*)serialdata)->length = strlen(tempsend)+13;
// 	((SERIALDATA*)serialdata)->session = 1;
// 	((SERIALDATA*)serialdata)->messageCode = SetNodeExportResourceData;
// 	((SERIALDATA*)serialdata)->nodeAddress = nodeAddress;
// 	((SERIALDATA*)serialdata)->resourcecode = resCode_RS232_Wireless;
// 	((SERIALDATA*)serialdata)->serialmode = 0x02;//串口模式0x01-配置串口 0x02-数据
// 	((SERIALDATA*)serialdata)->serialnum = InSerialnum;					// 串口号
// 	((SERIALDATA*)serialdata)->datalength = strlen(tempsend);
// 	memcpy(((SERIALDATA*)serialdata)->data,tempsend,strlen(tempsend));

// 	BYTE checkSum = 0;
// 	for (int i = 0;i<(((SERIALDATA*)serialdata)->length-1);i++)
// 	{
// 		checkSum += serialdata[i];
// 		//printf("%02x ",serialdata[i]);
// 	}
// 	serialdata[((SERIALDATA*)serialdata)->length-1] = checkSum;
// 	//printf("checkSum = %x\n",serialdata[((SERIALDATA*)serialdata)->length-1]);
// 	theApp.m_sSensor.Write(serialdata,((SERIALDATA*)serialdata)->length);
// 	delete[] tempsend;
// 	delete[] serialdata;
// }


//发送获取传感器校准参数消息: resCode_Current_Dual，
// SendGetCalInfo(WORD InnodeAddress)//////////////////////////////////////////////////////////////////////////////////////
// {
// 	N_MESSAGE n_Message;
// 	n_Message.head        = FRAME_HEAD;
// 	n_Message.length      = sizeof(N_MESSAGE);
// 	n_Message.session     = 1;
// 	n_Message.messageCode = GetCalInfo;
// 	n_Message.nodeAddress = InnodeAddress;
// 	n_Message.checkSum    = 0;
// 	for (int i = 0;i<(sizeof(N_MESSAGE)-1);i++)
// 	{
// 		n_Message.checkSum += ((BYTE*)(&n_Message))[i];
// 	}

// 	theApp.m_sSensor.Write(&n_Message,sizeof(N_MESSAGE));
// }

//发送重启节点消息
// void CWSNDemoDlg::SendResetNode(WORD nodeAddress)//////////////////////////////////////////////////////////////////////////////////////////
// {
// 	N_MESSAGE n_Message;
// 	n_Message.head = FRAME_HEAD;
// 	n_Message.length = sizeof(N_MESSAGE);
// 	n_Message.session = 1;
// 	n_Message.messageCode = ResetNode;
// 	n_Message.nodeAddress = nodeAddress;
// 	n_Message.checkSum = 0;
// 	for (int i = 0;i<(sizeof(N_MESSAGE)-1);i++)
// 	{
// 		n_Message.checkSum += ((BYTE*)(&n_Message))[i];
// 	}

// 	theApp.m_sSensor.Write(&n_Message,sizeof(N_MESSAGE));
// }