#include "types.h"

void printfhex(u8 *dat,u8 len)
{
	u8 a;
	for(a=0;a<len;a++)
	{
		printf("%02x ",dat[a]);
	}
	printf("len=%d\n",len);
}
