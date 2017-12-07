#ifndef __MODBUS_APP_H_
#define __MODBUS_APP_H_

#include <stdio.h>
#include <sys/time.h>
#include <memory.h>
#include <termios.h>
#include "modbus.h"
#include "serial.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "../types.h"
#include "../json/cJSON.h"
#include <sys/ioctl.h>
#include <fcntl.h>

#define MODBUS_ADDR   0x00
#define SERIAL_PATH   "/dev/ttyAMA1"     //串口设备地址

#define SETRELANGLE   0
#define SETABTANGLE   1
#define SETSECTOR     2
#define RESETPOSITION 3

typedef struct
{
	int pow;
	int temp;
	int humi;
	int light;
	char gps[32];
}Data;

typedef struct
{
	unsigned char par1_h;
	unsigned char par1_l;
	unsigned char par2_h;
	unsigned char par2_l;
	unsigned char par3_h;
	unsigned char par3_l;
}PatThree;

typedef struct
{
	const char *jsoncmd;
	u8 tag;
	u8 len;
}cmd;

typedef enum
{
	CMD_READ_DAT  = 0x03,
	CMD_READ_KEY  = 0x06,
	CMD_WRITE_PAN = 0x0c
}CMD;

void modbusSend(int fd,const char *buf,int len);
char modbusPolling(char *buf,int arg_len);
//void *ModbusPthread(void *arg);
int panMsgDeal(char *msg,unsigned int len);

#endif /* MODBUS_H_ */
