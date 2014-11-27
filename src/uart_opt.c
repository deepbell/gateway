/***********************************
** 串口通信程序
** 作者：李相
** 单位：西安理工大学
** 日期：2014年8月1日
***********************************/
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>	/*atoi和exit等*/
#include <unistd.h>	/*close, read, write, sleep等*/
#include "common.h"
//#include "message.h"
#include "uart_opt.h"

static uart_cfg_t uartread;//串口配置

static int speed_arr[] = {B230400, B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
//arch/xx/include/asm/termbits.h内宏定义，如 #define  B9600  0000015
static int name_arr[]  = {230400, 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300};

static void print_uart_read(void);
static void read_uart_cfg(void);
static void set_speed(int fd);
static int set_parity(int fd);
static int open_dev(char *dev, int is_block);//open uart port

/*-----------------------------------------------
  函数名:      print_uart_read
  参数:        无
  返回值:      void
  描述:        打印配置文件tq2440_uart.cfg的内容
-----------------------------------------------*/

void print_uart_read(void)
{
	printf("uartread.dev is %s\n",uartread.dev);
	printf("uartread.speed is %d\n",uartread.speed);
	printf("uartread.databits is %d\n",uartread.databits);
	printf("uartread.stopbits is %d\n",uartread.stopbits);
	printf("uartread.parity is %c\n",uartread.parity);
}

/*---------------------------------------------------
  函数名:      read_uart_cfg
  参数:        无
  返回值:      void
  描述:       读取tq2440_uart.cfg文件里到串口配置参数
---------------------------------------------------*/
void read_uart_cfg(void)
{
	FILE *fd;
	char j[10];
	//extern char *uart_cfg_file;
//	printf("Read serail cfg...\n");

	fd = fopen(UART_CFG_FILE,"r");
	if (NULL == fd)
	{
		printf("Can't open tq2440_uart.cfg!");
	}
	else
	{
		fscanf(fd, "DEV=%s\n", uartread.dev);

		fscanf(fd, "SPEED=%s\n", j);
		uartread.speed = atoi(j);

		fscanf(fd, "DATABITS=%s\n", j);
		uartread.databits = atoi(j);

		fscanf(fd, "STOPBITS=%s\n", j);
		uartread.stopbits = atoi(j);

		fscanf(fd, "PARITY=%s\n", j);
		uartread.parity = j[0];
	}
	fclose(fd);
}

/*-----------------------------------------------
  函数名:      set_speed
  参数:        int fd
  返回值:      void
  描述:        设置fd表述符的串口波特率
-----------------------------------------------*/
void set_speed(int fd)
{
	int i;
	int status;

	for (i = 0; i < sizeof(speed_arr) / sizeof(int); i++)
	{
		if (uartread.speed == name_arr[i])					//找到标准的波特率与用户一致
		{
			tcflush(fd, TCIOFLUSH);					//清除IO输入和输出缓存
			cfsetispeed(&opt, speed_arr[i]);		//设置串口输入波特率
			cfsetospeed(&opt, speed_arr[i]);		//设置串口输出波特率
			status = tcsetattr(fd, TCSANOW, &opt);	//将属性设置到opt的数据结构中，并且立即生效
			if(status != 0)
			{
				perror("tcsetattr fd:");			//设置失败
				return;
			}
			tcflush(fd, TCIOFLUSH);					//每次清除IO缓存
		}
	}
}

/*-----------------------------------------------
  函数名:      set_parity
  参数:        int fd
  返回值:      成功返回FALSE
  描述:        设置fd表述符的数据位、校验位和停止位
-----------------------------------------------*/
int set_parity(int fd)
{
	#if 1
	//消除收发模式规则：
	opt.c_lflag        = 0;
	opt.c_oflag        = 0;
	opt.c_iflag        = 0;
	#endif
	#if 1
	//消除字符屏蔽规则：
	opt.c_cc[VINTR]    = 0;   /**//* Ctrl-c */
	opt.c_cc[VQUIT]    = 0;  /**//* Ctrl- */
	opt.c_cc[VERASE]   = 0;  /**//* del */
	opt.c_cc[VKILL]    = 0;   /**//* @ */
	opt.c_cc[VEOF]     = 0;   /**//* Ctrl-d */
	//opt.c_cc[VTIME]    = 1;   /**//*inter-character timer, timeout VTIME*0.1*/
	//opt.c_cc[VMIN]     = 0;   /**//*blocking read until VMIN character arrives*/
	opt.c_cc[VSWTC]    = 0;   /**//* '' */
	opt.c_cc[VSTART]   = 0;   /**//* Ctrl-q */
	opt.c_cc[VSTOP]    = 0;   /**//* Ctrl-s */
	opt.c_cc[VSUSP]    = 0;   /**//* Ctrl-z */
	opt.c_cc[VEOL]     = 0;   /**//* '' */
	opt.c_cc[VREPRINT] = 0;   /**//* Ctrl-r */
	opt.c_cc[VDISCARD] = 0;   /**//* Ctrl-u */
	opt.c_cc[VWERASE]  = 0;   /**//* Ctrl-w */
	opt.c_cc[VLNEXT]   = 0;   /**//* Ctrl-v */
	opt.c_cc[VEOL2]    = 0;   /**//* '' */
	opt.c_cflag |= CLOCAL | CREAD;//本地连接和接收使能
	opt.c_cflag &= ~CSIZE;
	#endif
	#if 1
	opt.c_iflag |= IGNPAR | ICRNL;//忽略桢错误和奇偶校验错误|将输入中的回车翻译为新行字符（除非设置了IGNCR）
	opt.c_oflag |= OPOST; //选择原始数据输出
	opt.c_iflag &= ~(IXON|IXOFF|IXANY);//禁用流控制    
	#endif
	opt.c_cc[VTIME] = 0;//100;/* 设置超时10 seconds*/
	opt.c_cc[VMIN]  = 0;	/* Update the opt and do it NOW */
	#if 1
	/*如果不是开发终端之类的，只是串口传输数据，而不需要串口来处理，那么
    使用原始模式(Raw Mode)方式来通讯，设置方式如下：*/
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/ 
  opt.c_oflag &= ~OPOST; /*Output*/
	#endif
	//设置数据位
	switch (uartread.databits)
	{
		case 7:
			opt.c_cflag |= CS7;
			break;
		case 8:
			opt.c_cflag |= CS8;
			break;
		default:
			opt.c_cflag |= CS8;
			fprintf(stderr, "Unsupported data size\n");
			return(FALSE);
	}
	//设置校验位
	switch (uartread.parity)
	{
		case 'n':
		case 'N':
			opt.c_cflag &= ~PARENB;	/* Clear parity enable */
			opt.c_iflag &= ~INPCK;	/* Enable parity checking */
			break;
		case 'o':
		case 'O':
			opt.c_cflag |= (PARODD | PARENB);/*设置为奇效验*/ 
			opt.c_iflag |= INPCK;	/* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			opt.c_cflag |= PARENB;	/* Enable parity */
			opt.c_cflag &= ~PARODD;/* 转换为偶效验*/ 
			opt.c_iflag |= INPCK;	/* Disnable parity checking */
			break;
		case 'S':   
    case 's':  /*as no parity*/     
      opt.c_cflag &= ~PARENB;  
      opt.c_cflag &= ~CSTOPB;
      break;
		default:
			opt.c_cflag &= ~PARENB;
			opt.c_iflag &= ~INPCK;
			fprintf(stderr, "Unsupported parity\n");
			return(FALSE);
	}
	//设置停止位
	switch (uartread.stopbits)
	{
		case 1:
			opt.c_cflag &= ~CSTOPB;
			break;
		case 2:
			opt.c_cflag |= CSTOPB;
			break;
		default:
			opt.c_cflag &= ~CSTOPB;
			fprintf(stderr, "Unsupported stop bits\n");
			return(FALSE);
	}

	if (uartread.parity != 'n') opt.c_iflag |= INPCK;
	tcflush(fd, TCIFLUSH);   //清空输入缓存
	if (tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("Setupuart 3");
		return(FALSE);
	}
	return(TRUE);
}

/*-----------------------------------------------
函数名：		open_dev
参数：		char *dev, int is_block
返回值:      返回打开的文件描述符
描述：		打开串口设备，和串口是否阻塞, is_block为1表示阻塞
-----------------------------------------------*/

int open_dev(char *dev, int is_block)
{
	int fd = 0;
  int flag;

  flag = 0;
  flag |= O_RDWR | O_NOCTTY;
  if (is_block == 0)
    flag |= O_NONBLOCK;// 以前用 O_NDELAY;
	//int fd = open(dev, O_RDWR, 0);

	fd = open(dev, flag);
	#if 0
	printf("fd = %d\n", fd);
	printf("dev = %s\n", dev);
	printf("flag = %d\n", flag);
	#endif

	if (fd < 0)
	{
		perror("Can't Open uart Port");
		close(fd);
		return -1;
	}
	else
		return fd;
}

/*-----------------------------------------------------------------------------
函数名:      uart_init
参数:        
返回值:      初始化成功返回打开的文件描述符
描述:        串口初始化，根据串口文件路径名，串口的速度
*-----------------------------------------------------------------------------*/
int uart_init(void)
{
	char *dev;
	int fd;

	read_uart_cfg();
	print_uart_read();

	dev = uartread.dev;
	//打开串口设备
	fd = open_dev(dev, 1);//若为0则表示以非阻塞方式打开；
												//阻塞打开，读取串口数据会阻塞c_cc[VTIME]＊0.1秒，本程序设为10秒
	if (tcgetattr(fd, &old_opt) != 0)//保存原始配置
	{
		perror("Get opt in parity error:");
		return(FALSE);
	}
	if (fd > 0)
		set_speed(fd);		//设置波特率
	else
	{
		printf("Can't Open uart Port!\n");
		exit(0);
	}
	#if 0
	//恢复串口未阻塞状态?????????????????
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		printf("fcntl failed!\n");
		exit(0);
	}
	#endif
	//设置串口参数
	if (set_parity(fd) == FALSE)
	{
		printf("Set parity Error\n");
		exit(1);
	}
////	printf("Reading...\n");
	return fd;
}

/*-----------------------------------------------------------------------------
  函数名:      uart_read_fix
  参数:        int fd,char *str,DWORD len
  返回值:      在规定的时间内读取数据，超时则退出
  描述:        从fd描述符的串口缓存区读len个字节读数据，存入str指向读空间
 *-----------------------------------------------------------------------------*/
int uart_read_fix(int fd, BYTE *str, DWORD len)
{
	fd_set rfds;//读数据文件描述符集合
	BYTE readlen = 0;
	int ret;
	int sret;        //select监控结果
	BYTE *ptr;
	ptr = str;                          //读指针，每次移动，因为实际读出的长度和传入参数可能存在差异
    
	FD_ZERO(&rfds);                     //清除文件描述符集合
	FD_SET(fd,&rfds);                   //将fd加入fds文件描述符，以待下面用select方法监听

	/*开始读*/
	while (readlen < len)
	{
		sret = select(fd+1,&rfds,NULL,NULL,NULL);        //检测串口是否可读
		if (sret == -1)                              //检测失败
		{
			perror("select:");
			break;
		}
		else if (sret > 0)      //检测成功可读
		{
			ret = read(fd,ptr,1);
	//		printf("nnn = %d\n", ret);
			if (ret < 0)
			{
				perror("read err:");
				break;
			}
			else if (ret == 0)
				break;
		
			readlen += ret;                             //更新读的长度
			ptr     += ret;                             //更新读的位置
		}
	}
	//printf("readlen = %d\n", readlen);
	return readlen;
}

/*-----------------------------------------------------------------------------
  函数名:      uart_read_left
  参数:        int fd,char *str
  返回值:      在规定的时间内读取数据，超时则退出
  描述:        从fd描述符的串口缓存区剩余数据，存入str指向读空间
 *-----------------------------------------------------------------------------*/
int uart_read_left(int fd, BYTE *str)
{
	fd_set rfds;//读数据文件描述符集合
	int ret = 0;
	int sret;        //select监控结果

	FD_ZERO(&rfds);                     //清除文件描述符集合
	FD_SET(fd,&rfds);                   //将fd加入fds文件描述符，以待下面用select方法监听

/*开始读*/
	sret = select(fd+1, &rfds, NULL, NULL, NULL);        //检测串口是否可读
	if (sret == -1)                              //检测失败
	{
		perror("select:");
	}
	else if (sret > 0)      //检测成功可读
	{
		ret = read(fd, str, UART_RX_LEN);
//		printf("nnn = %d\n", ret);
		if (ret < 0)
		{
			perror("read err:");
		}
	}
	return ret;
}

/*-----------------------------------------------------------------------------
  函数名:      uart_read_msg
  参数:        int fd,char *str
  返回值:      返回读到到数据包长度
  描述:        从fd描述符的串口缓存区读取一个完整到数据包，存入str指向读空间
 *-----------------------------------------------------------------------------*/
int uart_read_msg(int fd, BYTE *str)
{
	fd_set rfds;//读数据文件描述符集合
	BYTE readlen = 1;
	int ret;
	int sret;        //select监控结果
	BYTE *ptr;
	*str = '@';//添加包头
	ptr = str + 1;                          //读指针，每次移动
    
	FD_ZERO(&rfds);                     //清除文件描述符集合
	FD_SET(fd,&rfds);                   //将fd加入fds文件描述符，以待下面用select方法监听

	/*开始读*/
	do
	{
		sret = select(fd+1,&rfds,NULL,NULL,NULL);        //检测串口是否可读
		if (sret == -1)                              //检测失败
		{
			perror("select:");
			break;
		}
		else if (sret > 0)      //检测成功可读
		{
			ret = read(fd,ptr,1);//读取一个字节写入ptr指向到空间
	//		printf("nnn = %d\n", ret);
			if (ret < 0)
			{
				perror("read err:");
				break;
			}
			else if (ret == 0)
				break;
			if (*ptr != '@')
			{
				readlen += ret;                             //更新读的长度
				ptr     += ret;                             //更新读的位置，ptr指向下一个位置
			}

		}
	} while((readlen == 1) || (*ptr != '@'));//读到'@'有两种情况：这个包的'@'和下一个包到'@'。前者需要继续读完这个包，后者标志着一个包已经读取完。
	
	return readlen;
}

/*-----------------------------------------------------------------------------
  函数名:      uart_send
  参数:        int fd,char *str,unsigned int len
  返回值:      发送成功返回发送长度，否则返回小于0的值
  描述:        向fd描述符的串口发送数据，长度为len，内容为str
*-----------------------------------------------------------------------------*/
int uart_send(int fd, BYTE *str, BYTE len)
{
    int ret;

//    if(len > strlen(str))                    //判断长度是否超过str的最大长度
//      int  len = strlen(str);

    ret = write(fd,str,len);
    if (ret < 0)
    {
        perror("uart send err:");
        return -1;
    }
    return ret;
}
