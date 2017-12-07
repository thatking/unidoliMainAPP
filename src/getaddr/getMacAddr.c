#include "getMacAddr.h"

char DeviceID[24];

/*******************************************************************************
* 名称: GetClientID
* 功能: 获取设备
* 形参:
* 返回:
* 说明: 用Wlan0的mac地址作为设备id
*******************************************************************************/
int clientID_init(void)
{
	const char ifr[] = "wlan0";
	int sock;
	struct ifreq ifq;
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) <0)
	{
		PRINTF("socket error ");
		return -1;
	}
	strcpy(ifq.ifr_name, ifr);
	if(ioctl(sock, SIOCGIFHWADDR, &ifq) <0)
	{
		PRINTF("ioctl error");
		strcpy(DeviceID,"0857001B7862_00");
		return -1;
	}
	sprintf(DeviceID,"%02X%02X%02X%02X%02X%02X",
			(unsigned char)ifq.ifr_hwaddr.sa_data[0],
			(unsigned char)ifq.ifr_hwaddr.sa_data[1],
			(unsigned char)ifq.ifr_hwaddr.sa_data[2],
			(unsigned char)ifq.ifr_hwaddr.sa_data[3],
			(unsigned char)ifq.ifr_hwaddr.sa_data[4],
			(unsigned char)ifq.ifr_hwaddr.sa_data[5]);
	return 0;
}

void getClientID(char *p)
{
	strcpy(p,DeviceID);
}
