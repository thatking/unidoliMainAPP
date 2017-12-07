#include "modbus_app.h"

const char pubmsg[] = "{\"power\":%d,\"temp\":%d,\"humi\":%d,\"light\":%d,\"gps\":\"%s\"}";
Data data;
const unsigned char slave_addr[] =
{
		0x01,
		0x02
};

const cmd cmd_map[]=
{
	{"setRelAngle",0x04,0x04},
	{"setAbtAngle",0x05,0x04},
	{"setSector",0x07,0x06},
	{"resetPosition",0x08,0x01}
};
/*******************************************************************************
* 名称: YDL485_Disable
* 功能: 485芯片使能脚失能
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void YDL485_Disable(void)
{
	int ft = open("/sys/class/gpio/gpio24/value", O_RDWR | O_NOCTTY);
	if(ft > 0)
	{
		write(ft,"0",1);
		close(ft);
	}
	else
	{
		printf("gpio open error\n");
	}
}
/*******************************************************************************
* 名称: YDL485_Enable
* 功能: 485芯片使能脚使能
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void YDL485_Enable(void)
{
	int ft = open("/sys/class/gpio/gpio24/value", O_RDWR | O_NOCTTY);
	if(ft > 0)
	{
		write(ft,"1",1);
		close(ft);
	}
	else
	{
		printf("gpio open error\n");
	}
}
/*******************************************************************************
* 名称: YDL485_Send
* 功能:
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void modbusSend(int fd,const char *buf,int len)
{
//	SerialWrite(fd,buf,len);
		YDL485_Enable();
		usleep(1000);
		SerialWrite(fd,buf,len);
		usleep(len*800);
		YDL485_Disable();
}

/*******************************************************************************
* 名称:
* 功能:
* 形参:
* 返回:
* 说明:
*******************************************************************************/
void powerBoardtlv(tlv_alys* tlv)
{
	unsigned int i;
	for(i=0;i<tlv->tlv_num;i++)
	{
		if(tlv->memb[i].tag==0x00
			&&tlv->memb[i].len==0x01)
		{
			//data.pow = *tlv->memb[i].val;
			memcpy(&data.pow,tlv->memb[i].val,tlv->memb[i].len);
		}
	}
}
/*******************************************************************************
* 名称:
* 功能:
* 形参:
* 返回:
* 说明:
*******************************************************************************/
void sensorBoardtlv(tlv_alys* tlv)
{
	unsigned int i;
	for(i=0;i<tlv->tlv_num;i++)
	{
		switch(tlv->memb[i].tag)
		{
			case 0x04:memcpy(&data.temp,tlv->memb[i].val,tlv->memb[i].len);break;
			case 0x05:memcpy(&data.humi,tlv->memb[i].val,tlv->memb[i].len);break;
			case 0x06:memcpy(&data.light,tlv->memb[i].val,tlv->memb[i].len);break;
			case 0x07:memcpy(&data.gps,tlv->memb[i].val,tlv->memb[i].len);break;
		}
	}
}
void (*tlvfun[])(tlv_alys*) = {sensorBoardtlv,powerBoardtlv};

/*******************************************************************************
* 名称: modbusPolling
* 功能:
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
char modbusPolling(char *buf,int arg_len)
//void *ModbusPthread(void *arg)
{
	int len;
	int i;
	tlv_alys tlv;
	int fd;

	YDLModbusInit(MODBUS_ADDR);
	memset(buf,0,arg_len);
	memset(&data,0,sizeof(data));
	fd = SerialInit(SERIAL_PATH,9600,0);
	if(fd > 0)
	{
		for(i=0;i<sizeof(slave_addr)/sizeof(slave_addr[0]);i++)
		{
			len = YDLModbusPack((u8*)buf,slave_addr[i],CMD_READ_DAT,0);
			tcflush(fd,TCIOFLUSH);     //刷清输入、输出队列
			modbusSend(fd,buf,len);
			len = SerialReadUnblock(fd,buf,REV_MAX_LEN,1000);
			if(len > 0)
			{
				if(YDLModbusAnalysis((u8*)buf,len,&tlv)!=0)
				{
					tlvfun[i](&tlv);
				}
				else
				{
					printf("function : %s line : %d : sensor=%d\n",__FUNCTION__,__LINE__,i+1);
				}
			}
			else
			{
				printf("function : %s line : %d : sensor=%d\n",__FUNCTION__,__LINE__,i+1);
			}
		}
		SerialClose(fd);
	}
	len = sprintf(buf,pubmsg,data.pow,data.temp,data.humi,data.light,data.gps);
	buf[len] = '\0';
	printf("%s\n",buf);

	return 0;
}

/*******************************************************************************
* 名称: ModbusPthread
* 功能:
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/


int panMsgDeal(char *msg,unsigned int len)
{
	int ret = -1;
	u8 modbus_buf[64];
	u8 modbus_send_len = 0;
	PRINTF("%.*s", len, msg);
	msg[len] = '\0';
	cJSON* json = cJSON_Parse(msg);
	if(json == NULL)
		goto RET;
	cJSON* cmd = cJSON_GetObjectItem(json,"cmd");
	if(cmd == NULL)
		goto RET;
	int pan_cmd;
	for(pan_cmd = 0; pan_cmd < (sizeof(cmd_map) / sizeof(cmd_map[0])); pan_cmd++)
	{
		if(strcmp(cmd_map[pan_cmd].jsoncmd,(const char *)cmd->valuestring) == 0)
			break;
	}
	switch(pan_cmd)
	{
		case SETRELANGLE:          //setRelAngle
		case SETABTANGLE:          //setAbtAngle
		{
			cJSON* ver = cJSON_GetObjectItem(json,"vertical");  //垂直
			cJSON* lev = cJSON_GetObjectItem(json,"level");     //水平
			PatThree val;
			val.par1_h = ((short int)ver->valueint & 0xff00)>>8;
			val.par1_l = (short int)ver->valueint & 0xff;
			val.par2_h = ((short int)lev->valueint & 0xff00)>>8;
			val.par2_l = (short int)lev->valueint & 0xff;
			modbus_send_len = YDLModbusPack((u8*)modbus_buf,0x01,CMD_WRITE_PAN,1,
					cmd_map[pan_cmd].tag,cmd_map[pan_cmd].len,&val);
		}
		break;
		case SETSECTOR:  		//setSector
		{
			cJSON* ver = cJSON_GetObjectItem(json,"vertical");  //垂直
			cJSON* lev = cJSON_GetObjectItem(json,"start_level");   //水平
			cJSON* angle = cJSON_GetObjectItem(json,"angle");
			PatThree val;
			val.par1_h = ((short int)ver->valueint & 0xff00)>>8;
			val.par1_l = (short int)ver->valueint & 0xff;
			val.par2_h = ((short int)lev->valueint & 0xff00)>>8;
			val.par2_l = (short int)lev->valueint & 0xff;
			val.par3_h = ((short int)angle->valueint & 0xff00)>>8;
			val.par3_l = (short int)angle->valueint & 0xff;
			modbus_send_len = YDLModbusPack((u8*)modbus_buf,0x01,CMD_WRITE_PAN,1,
					cmd_map[pan_cmd].tag,cmd_map[pan_cmd].len,&val);
		}
		break;
		case RESETPOSITION:
		{
			int val = 1;
			modbus_send_len = YDLModbusPack((u8*)modbus_buf,0x01,CMD_WRITE_PAN,1,
								cmd_map[pan_cmd].tag,cmd_map[pan_cmd].len,&val);
		}
		break;
		default:
			break;
	}
	if(modbus_send_len > 0)
	{
		int fd = SerialInit(SERIAL_PATH,9600,0);
		if(fd > 0)
		{
			PRINTF("send msg to device!:");
			printfhex(modbus_buf,modbus_send_len);
			modbusSend(fd,(const char *)(char *)modbus_buf,modbus_send_len);
			int len = SerialReadUnblock(fd,(char *)modbus_buf,32,100);
			if(len > 0)
			{
				PRINTF("rev = ");
				printfhex(modbus_buf,len);
				ret = 0;
			}
			else
			{
				ret = 3;
				PRINTF("device no received");
			}
			SerialClose(fd);
		}
		else
			ret = 2;
	}
RET:
	return ret;
}


