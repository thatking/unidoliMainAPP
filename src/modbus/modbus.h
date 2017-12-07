/*
 * YDL_485.h
 *
 *  Created on: Jun 26, 2017
 *      Author: root
 */

#ifndef MODBUS_H_
#define MODBUS_H_

/********* 硬件平台接口对接*******/
#include "../types.h"
/******************************/

#define YDL_MODBUS_START  0xAA
#define REV_MAX_LEN 128
#define SED_MAX_LEN 128

typedef struct
{
#define TLV_MAX_NUM 6
    u8 src;
    u8 cmd;
    u8 tlv_num;
    struct
    {
        u8 tag;
        u8 len;
        u8 *val;
    }memb[TLV_MAX_NUM];
}tlv_alys;

void YDLModbusInit(u8 addr);
u8 YDLModbusPack(u8 *buf,u8 addr,u8 cmd,u8 num,...);
//void YDLModbusReceive(u8 *msg,u8 len);
u8 YDLModbusAnalysis(u8 *msg,u8 len,tlv_alys *tlv);

#endif /* MODBUS_H_ */
