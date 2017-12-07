/*
 * YDL_485.c
 *
 *  Created on: Jun 26, 2017
 *      Author: root
 */


#include "modbus.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#pragma pack(1)
typedef struct {
    u8 start;
    struct
    {
        u8 dis_addr:4;
        u8 src_addr:4;
    } addr;
    u8 len;
    u8 check;
} YDL_MODBUS_HD;
#pragma pack()
//static modbus_rev_cb tlvdeal = NULL;
static u8 dev_addr;
/*******************************************************************************
* 名称: CalcLrc
* 功能: 计算LRC校验值
* 形参:
* 返回: 校验值
* 说明: 无
*******************************************************************************/
static u8 CalcLrc(uc8 *msg,u8 len)
{
    u8 lrc = 0;
    for(; len>0; len--)
        lrc += *msg++;
    return ~lrc+1;
}
/*******************************************************************************
* 名称: YDLModbusSend
* 功能: modbus数据打包发送
* 形参:
* 返回: 无
* 说明: 无
*******************************************************************************/
void YDLModbusInit(u8 addr)
{
    dev_addr = addr;
}
/*******************************************************************************
* 名称: YDLModbusPack
* 功能: modbus数据打包
* 形参:
* 返回: 无
* 说明: 无
*******************************************************************************/
u8 YDLModbusPack(u8 *buf,u8 addr,u8 cmd,u8 num,...)
{
    va_list argptr;
    u8 len;
    u8 *val;
//	u8 msg[PACK_MAX_LEN];
    u8* p = buf+ sizeof(YDL_MODBUS_HD);              //head

    YDL_MODBUS_HD* head=(YDL_MODBUS_HD*)buf;
    head->start = YDL_MODBUS_START;
    head->addr.dis_addr = addr;
    head->addr.src_addr = dev_addr;
    *p++ = cmd;                                     //
    va_start(argptr, num);                          //
    while(num--)
    {
        *p++ = va_arg(argptr, u32);    //tag
        len = va_arg(argptr, u32);     //length
        *p++ = len;
        val = va_arg(argptr, u8*);     //value
        while(len--) {
            *p++ = *val++;
        }
    }
    va_end(argptr);
    len = p-buf;
    head->len = len;
    head->check = 0x00;
    head->check = CalcLrc(buf,len);
    return len;
}
/*******************************************************************************
* 名称: YDLModbusReceive
* 功能: modbus数据接收处理
* 形参:
* 返回: 无
* 说明: 无
*******************************************************************************/
//void YDLModbusReceive(u8 *msg,u8 len)
//{
//	YDL_MODBUS_HD* head = (YDL_MODBUS_HD*)msg;
//	u8 *p = msg + sizeof(YDL_MODBUS_HD);
//	u8 cmd;
//	if(head->start!=YDL_MODBUS_START
//		|| head->addr.dis_addr!=dev_addr
//		|| head->len!=len)
//		return;
//	if(0 != CalcLrc(msg,len))
//		return;
//	cmd = *p++;
//	if(tlvdeal != NULL)
//		tlvdeal(cmd,len-sizeof(YDL_MODBUS_HD)-1,p);
//}
/*******************************************************************************
* 名称: YDLModbusAnalysis
* 功能: modbus数据解析
* 形参:
* 返回: 无
* 说明: 无
*******************************************************************************/
u8 YDLModbusAnalysis(u8 *msg,u8 len,tlv_alys *tlv)
{
    YDL_MODBUS_HD* head = (YDL_MODBUS_HD*)msg;
    u8 *p = msg + sizeof(YDL_MODBUS_HD);
    u8 *end = msg + len;
    if(head->start!=YDL_MODBUS_START
            || head->addr.dis_addr!=dev_addr
            || head->len!=len)
        return 0;
    if(0 != CalcLrc(msg,len))
        return 0;
    tlv->src = head->addr.src_addr;
    tlv->cmd = *p++;
    tlv->tlv_num = 0;
    while(p<end)
    {
        tlv->memb[tlv->tlv_num].tag = *p++;
        tlv->memb[tlv->tlv_num].len = *p++;
        tlv->memb[tlv->tlv_num].val = p;
        p += tlv->memb[tlv->tlv_num].len;
        tlv->tlv_num++;
    }
    return 1;
}
