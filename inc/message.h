/*
	变量、类型名：
	前缀为“p_”的是指针；
	前缀为“s_”的是服务消息类型；
	后缀为“_t”的是结构体类型名；

*/
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#pragma once
/////////////////////////////////////////////////////////////////////////////////////////////////
#define MSG_HEAD						'@'    		//消息头
//#define ALLOCATE_BUF_SIZE               10240		 //存储缓冲区
#define HEARTBEATINTERBALTOSER			5			//向服务器发送的心跳间隔，单位:秒
#define HEARTBEATTOSERID				100			//向服务器发送心跳的计时器ID
#define NODEPARSONINFOID				101			//父子节点关系保持计时器ID
#define HEARTBEATINTERBALFROMSER		5			//来自服务器的心跳间隔，单位:秒
#define HEARTBEATOVERFROMSER			15000		//来自服务器的心跳超时时间，单位:毫秒
#define NODEPARSONOVER					10000		//父子节点关系超时
/////////////////////////////////////////////////////////////////////////////////////////////////
#define S_SESSION_ID					0x0000		//服务消息的会话ID
#define N_SESSION_ID					0x0001		//节点资源消息的会话ID
/////////////////////////////////////////////////////////////////////////////////////////////////
//应答代码定义
#define RIGHT_MESSAGE	                 0x00		 //正确
#define ERROR_MESSAGE	                 0x01		 //错误
#define UNKOWN_MESSAGE	                 0x02		 //未知
/////////////////////////////////////////////////////////////////////////////////////////////////
//服务消息定义///////////////////////////////////////////////////////////////////////////////////
#define S_HEART_DETECTION                0x01	  	 //心跳检测（双向）
#define S_SERVICE_TO_COORDINATOR         0x02		 //第一个上层应用连接服务（中间服务->协调器）
#define S_DEVICE_IDENTIFICATION          0x11		 //设备识别（协调器->中间服务）
#define S_DEVICE_NUMBER                  0x12		 //设备序号（中间服务->协调器）
/////////////////////////////////////////////////////////////////////////////////////////////////
//应用消息定义///////////////////////////////////////////////////////////////////////////////////
//一般消息
#define GET_NODE_INFO			         0x01		 //获取节点信息（PC->节点）
#define RESET_NODE			             0x02		 //重启节点（PC->节点）
#define GET_CAL_INFO			         0x03		 //传感器校准参数消息
//上传消息
#define NODE_INFO			             0x21		 //节点信息
#define NODE_EXPORT_RESOURCE_DATA	     0x22		 //节点扩展板资源数据
#define NODE_MOTHER_RESOURCE_DATA	     0x24		 //节点母板资源数据
#define NODE_PARENT			             0x26		 //节点父子关系
#define NODE_CAL_INFO			         0x29		 //上传传感器校准参数消息
//下发消息
#define SET_NODE_EXPORT_RESOURCE_DATA	 0x23		 //下传扩展板资源数据
#define SET_NODE_MOTHER_RESOURCE_DATA	 0x25		 //下传母版资源数据
#define SET_NODE_PAN_ID					 0x28		 //修改PANID
//群发消息
#define GROUP_GET_NODE_INFO			     0x11		 //获取节点信息（PC->节点）群发消息
#define GROUP_RESET_NODE				 0x12		 //重启节点（PC->节点）群发消息


#define SMART_BOARD                      1		 	//智能板
#define POWER_BOARD                      2		 	//电源板


/////////////////////////////////////////////////////////////////////////////////////////////////
//资源代码定义///////////////////////////////////////////////////////////////////////////////////
//AD方式
#define resCode_Voltage_Single		((0x10<<8)+0x00)
#define resCode_Voltage_Dual		((0x10<<8)+0x01)
#define resCode_Current_Single		((0x10<<8)+0x10)
#define resCode_Current_Dual		((0x10<<8)+0x11)
#define RS_CODE_LIGHT_S				((0x10<<8)+0x20)	//AD单通道，光敏传感器
#define resCode_Pressure			((0x10<<8)+0x30)
#define resCode_Alcohol				((0x10<<8)+0x31)
#define resCode_Pressure_Alcohol	((0x10<<8)+0x32)
//IIC接口方式
#define resCode_Temp_Humidity		((0x20<<8)+0x00)
#define resCode_Temp_Humidity_Light	((0x20<<8)+0x01)
#define resCode_Voltage_Output		((0x20<<8)+0x10)
#define resCode_Relay_GPIN			((0x20<<8)+0x20)
//IIC+外部CPU实现
#define resCode_Photoelectric		((0x21<<8)+0x00)
#define resCode_IR_Output			((0x21<<8)+0x01)
#define resCode_IR_Photoelectric	((0x21<<8)+0x02)
#define resCode_Ultrasonic			((0x21<<8)+0x10)
//串口接口
#define resCode_RS232_Wireless		((0x30<<8)+0x00)
#define resCode_RDID				((0x30<<8)+0x10)
#define resCode_RS232_Coordinator	((0x31<<8)+0x00)

//电源板，智能板

#define resCode_Led					((0x02<<8)+0x00)
#define resCode_Buzzer				((0x03<<8)+0x00)
//智能板 增加两个
#define resCode_AD					((0x04<<8)+0x00)
#define resCode_Relay				((0x01<<8)+0x00)
/////////////////////////////////////////////////////////////////////////////////////////////////
///
/////////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push) //压栈保存
#pragma pack(1)	// 设置字节对齐

typedef struct
{
	WORD node;					//控制的节点
	BYTE device;				//设备号
	BYTE seq;					//序号
	BYTE state;					//状态
}WEBCMD,*PWEBCMD;

//WSN消息封装头部
typedef struct
{
	BYTE	head;             	//消息头
	BYTE	len;             	//消息长度
	WORD	session;            //会话ID
	BYTE  	msg_code;         	//消息类型
	BYTE  data_head;		 	//指向数据体
}msg_header_t,*p_msg_header_t;
//WSN消息封装母板资源头部
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码
	
	WORD node_addr;				//网络节点地址
	BYTE mother_board;			//母板类型1-智能板 2-电源板
	WORD resource_code;			//资源代码
	BYTE data_head;		 		//指向数据体
}mo_rs_msg_header_t,*p_mo_rs_msg_header_t;
//WSN消息封装扩展板资源头部
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	BYTE data_head;		 		//指向数据体
}ex_rs_msg_header_t,*p_ex_rs_msg_header_t;
/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////协调器与网关间//////////////////////////////////////////////////////////////////////////
//------------------服务消息------------------------
//服务一般消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	BYTE check_sum;				//校验和
}s_comm_msg_t,*p_s_comm_msg_t;
//服务一般应答消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	BYTE resp_code;				//应答代码
	BYTE check_sum;				//校验和
}s_comm_resp_msg_t, *p_s_comm_resp_msg_t;
//设备识别消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码0x11

	BYTE app_type;              //应用类型
	BYTE equ_type;              //设备类型
	BYTE heartbeat_timeout;     //心跳检测超时
	BYTE heartbeat_period;      //服务发送心跳间隔
	BYTE check_sum;				//校验和
}s_dev_id_msg_t, *p_s_dev_id_msg_t;
//设备序号消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码0x11

	BYTE dev_num;				//设备序号
	BYTE check_sum;				//校验和
}s_dev_mun_msg_t,*p_s_dev_mun_msg_t;
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////协调器与网关间////////////////////////////////////////////////////////////////////////////
//------------------资源消息---------------------------
//节点一般消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE check_sum;				//校验和
}n_comm_msg_t,*p_n_comm_msg_t;
//节点一般应答消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE resp_code;				//应答代码
	BYTE check_sum;				//校验和
}n_comm_resp_msg_t,*p_n_comm_resp_msg_t;
//协调器一般消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	BYTE check_sum;				//校验和
}c_comm_msg_t,*p_c_comm_msg_t;


//PANID 消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD panid;					//PAN ID
	BYTE check_sum;				//校验和
}PANID_MESSAGE,*PPANID_MESSAGE;
//协调器一般应答消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	BYTE resp_code;				//应答代码
	BYTE check_sum;				//校验和
}CORES_MESSAGE,*PCORES_MESSAGE;

//一般群发消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	BYTE group;					//组号，0xFF表示群发
	BYTE check_sum;				//校验和
}GROUP_MESSAGE,*PGROUP_MESSAGE;
//节点信息(上传)
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长18
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE ieeeAddress[8];		//IEEE地址
	BYTE rssi;					//RSSI
	BYTE lqi;					//LQI
	WORD pan_id;				//PANID网络ID
	BYTE mother_board;			//母板类型1-智能板 2-电源板
	WORD resource_code;			//扩展板类型代码表
	BYTE check_sum;				//校验和
}node_info_t,*p_node_info_t;

//校准参数信息(上传)
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长12
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//扩展板类型代码表
	WORD max;					//最大值
	WORD zero;					//零点
	WORD min;					//最小值
	BYTE check_sum;				//校验和
}NODE_CALINFO,*PNODE_CALINFO;
//节点父子关系消息
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD pnode_addr;			//父节点地址
	BYTE check_sum;				//校验和
}node_parent_msg_t,*p_node_parent_msg_t;

/////////////////////////////////////////////////////////////////////////////////////////////////
//采样控制
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码
	
	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	BYTE channel_combin;		//通道组合
	BYTE sampling_accuracy;		//采样精度
	WORD sample_rate;			//采样速率
	WORD sample_mode;			//采样模式
	WORD sample_interval;		//采样间隔
	WORD sample_points;			//采样点数 0表示无限制
	BYTE packet_len;			//数据包长度，累积到（0-128）个点数后统一上传
	BYTE check_sum;				//校验和
}ex_sampling_control_t,*p_ex_sampling_control_t;
/////////////////////////////////////////////////////////////////////////////////////////////////
//母板资源/////////////////////////////////////////////////////////////////////////////////////////////////
//LED控制
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE mother_board;			//母板类型
	WORD resource_code;			//资源代码
	BYTE led1;					//1亮，0灭
	BYTE led2;					//1亮，0灭
	BYTE led3;					//1亮，0灭
	BYTE led4;					//1亮，0灭
	BYTE check_sum;				//校验和
}mo_led_control_t,*p_mo_led_control_t;

//Buzzer控制
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE mother_board;			//母板类型
	WORD resource_code;			//资源代码
	BYTE buzzer;				//1响，0不响
	BYTE check_sum;				//校验和
}mo_buzzer_control_t,*p_mo_buzzer_control_t;
//Relay控制
//输出数据	*	1	uInt8	
//继电器输出bit位说明:
//Bit.7	Bit.6	Bit.5	Bit.4	Bit.3	Bit.2	Bit.1	Bit.0
//RSV	RSV	RSV	RSV	KA	KB	KC	KD
//0	0	0	0	0	0	0	0
//?	ENA~END：对应继电器控制，高有效。
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	BYTE mother_board;			//母板类型
	WORD resource_code;			//资源代码
	BYTE relay;					//设置继电器开关状态：0表示继电器断开，1表示继电器闭合
	BYTE check_sum;				//校验和
}mo_relay_control_t,*p_mo_relay_control_t;
/////////////////////////////////////////////////////////////////////////////////////////////////
//扩展模块资源/////////////////////////////////////////////////////////////////////////////////////////////////
//单个资源数据
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	WORD resource_data;			//资源数据
	BYTE check_sum;				//校验和
}one_rs_data_t,*p_one_rs_data_t;
//两个资源数据
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	WORD resource_data1;		//资源数据
	WORD resource_data2;		//资源数据
	BYTE check_sum;				//校验和
}TWO_RESOURCE_DATA,*PTWO_RESOURCE_DATA;

//三个资源数据
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	WORD resource_data1;		//资源数据
	WORD resource_data2;		//资源数据
	WORD resource_data3;		//资源数据
	BYTE check_sum;				//校验和
}three_rs_data_t,*p_three_rs_data_t;
//DA控制
//DA模式字节位定义:
//Bit.7	Bit.6	Bit.5	Bit.4	Bit.3	Bit.2	Bit.1	Bit.0
//RST	ENS	RSV	RSV	ENA	ENB	ENC	END
//0	0	0	0	0	0	0	0
//?	RST:芯片复位控制，高有效。
//?	ENS:芯片总体OE控制。高有效。
//?	ENA~END:对应DA输出有效控制，高有效。
//?	当整体控制字节中RST=0,ENS=1；且ENA~END对应位有效时，后续数据才有意义。
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	BYTE damode;				//DA模式
	WORD da1;					//DA数据
	WORD da2;					//DA数据
	WORD da3;					//DA数据
	WORD da4;					//DA数据
	BYTE check_sum;				//校验和
}DA_CONTROL,*PDA_CONTROL;
//GPIN
//Bit.7  Bit.6  Bit.5  Bit.4  Bit.3  Bit.2  Bit.1  Bit.0
//RSV    RSV    RSV    RSV    KA     KB     KC     KD
//0      0      0      0      0      0      0      0
//KA~KD：对应开关量输入位，高位有效。
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	BYTE relay;					//设置继电器开关状态：0表示继电器断开，1表示继电器闭合
	//BYTE gpin;				//GPIN
	BYTE check_sum;				//校验和
}relay_gpin_control_t,*p_relay_gpin_control_t;

typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码
	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	WORD ir;					//红外值
	BYTE check_sum;				//校验和
}IR_CONTROL,*PIR_CONTROL;
//串口配置包
typedef struct
{
	BYTE head;					//消息头@
	BYTE len;					//消息长
	WORD session;				//设备ID和设备索引
	BYTE msg_code;				//消息代码

	WORD node_addr;				//网络节点地址
	WORD resource_code;			//资源代码
	BYTE uartmode;				//串口模式0x01-配置串口 0x02-数据
	BYTE uartnum;				//串口号
	BYTE baudrate;				//波特率
	BYTE parity;				//奇偶校验0x00-无 0x01-奇校验 0x02-偶校验 0x03-空格 0x04-标记
	BYTE databit;				//数据位5,6,7,8
	BYTE stopbit;				//停止位0x00-1 0x01-1.5 0x02-2
	BYTE flowcontrol;			//流控0x00-无 0x01-Xon/Xoff 0x02-硬件
	BYTE check_sum;				//校验和
}uart_CONTROL,*Puart_CONTROL;

//串口数据包
typedef struct
{
	BYTE head;				//消息头@
	BYTE len;				//消息长
	WORD session;			//设备ID和设备索引
	BYTE msg_code;			//消息代码

	WORD node_addr;			//网络节点地址
	WORD resource_code;		//资源代码
	BYTE uartmode;			//串口模式0x01-配置串口 0x02-数据
	BYTE uartnum;			// 串口号
	BYTE datalen;			// data 长度
	BYTE data[1];			// data
}uartDATA,*PuartDATA;

#endif
