使用说明：
（1）首先打开 CollectorEB-PRO 工程模块的电源，当协调器上的 LED1 和 LED2 都处于点亮状态时，再依次打开其他模块的电源，其他模块的 LED1 处于闪烁状态、LED2 处于常亮时，表示加入网络成功。
（1）将tq2440_serial.cfg文件复制到开发板的文件系统的/etc/目录下，通过修改tq2440_serial.cfg文件可以设置串口号，波特率等等参数。
（2）TQ2440开发板的3个串口名分别为/dev/ttySAC0, ttySAC1, ttySAC2，其中串口0作为控制台使用。串口1和串口2空闲。默认是ttySAC2,波特率115200。计算机使用串口ttyS0。
（3）因为pthread的库不是linux系统的库，所以在进行编译的时候要加上“-lpthread”：# gcc filename -lpthread。
（4）将编译好的serial_test程序复制到开发板的/sbin/目录下，然后运行serial_test即可测试串口。
	
程序说明：
（1）自定义的变量、类型名：
	前缀为“p_”的是指针；
	前缀为“s_”的是服务消息类型；
	后缀为“_t”的是结构体类型名；

（2）引用的头文件：
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
//#include <errno.h>
//#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>	/*atoi和exit等*/
//#include <string.h>
//#include <time.h>
#include <unistd.h>	/*close, read, write, sleep等*/
#include <pthread.h>

(3)模块说明：
串口数据收发模块：uart_opt.c，uart_opt.h;
消息处理模块：msg_process.c，msg_process.h;
网络连接维持模块：thrd_timer.c
