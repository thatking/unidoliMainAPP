/* CONFIG_H_ */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdlib.h>

#include "getaddr/getMacAddr.h"

#define MQTT_ADDR   "118.190.104.17"  //MQTT服务器地址
#define MQTT_PORT   61613             //MQTT服务器端口

#define MQTT_USERNAME "admin"          //MQTT服务器用户名
#define MQTT_PASSWORD "password"       //MQTT服务器密码
#define MQTT_ALIVE     10              //MQTT心跳时间
#define MQTT_TIMEOUT   2000            //MQTT连接超时时间(单位ms)

#define MQTT_CLIENT_SUB    "hicamv1.0/contl/"     //海思订阅主题
#define MQTT_CLIENT_RET    "hicamv1.0/ctlret/"    //海思控制回复
#define MQTT_CLIENT_PUB    "hicamv1.0/state/"     //海思发布主题
#define MQTT_CLIENT_WILL   "hicamv1.0/will/"      //海思遗嘱主题
#define MQTT_CLIENT_UPDATE "hicamv1.0/update/"    //海思更新主题
#define MQTT_CLIENT_CONFIG "hicamv1.0/config/"    //海思配置主题

#define MQTT_SENDBUF_SIZE  256
#define MQTT_RECVBUF_SIZE  256

#endif /* CONFIG_H_ */
