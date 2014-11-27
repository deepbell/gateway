#include <stdio.h>
#include "common.h"


/*******************************************************************************
*函数名：get_sum
*参数：BYTE *p,BYTE len
*返回值：返回校验和
*描述：计算校验和
*******************************************************************************/
BYTE get_sum(BYTE *p, BYTE len)
{
	DWORD sum = 0;
	int i;
	for (i = 0; i < len - 1; i++)
		sum += *p++;
	return (BYTE)(sum & 0x000000FF);
}
