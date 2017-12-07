#ifndef __IOT_H_
#define __IOT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iot_import.h"
#include "iot_export.h"

#include <pthread.h>
#include "config.h"
//#include "mqtt/MQTTClient.h"
#include "types.h"
#include "json/cJSON.h"
#include "string.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "getaddr/getMacAddr.h"
#include <signal.h>
#include "modbus_app.h"
#include "curlGetDat.h"


#if defined(MQTT_ID2_AUTH) && defined(TEST_ID2_DAILY)
    #define PRODUCT_KEY             "*******************"
//    #define DEVICE_NAME             "*******************"
//    #define DEVICE_SECRET           "*******************"
#else
    #define PRODUCT_KEY             "mivZEajJdbC"
//    #define DEVICE_NAME             "devicec"
//    #define DEVICE_SECRET           "df6Gvm8kdVeEuG3vQXlwtJCeDPbIAnR9"
#endif

// These are pre-defined topics
//#define TOPIC_UPDATE             "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
//#define TOPIC_CONFIG             "/"PRODUCT_KEY"/"DEVICE_NAME"/config"
//#define TOPIC_CONTROL            "/"PRODUCT_KEY"/"DEVICE_NAME"/control"
//#define TOPIC_STATE              "/"PRODUCT_KEY"/"DEVICE_NAME"/state"

#define MSG_LEN_MAX             (1024)

void *MqttPthread(void *arg);
void mqttStop(int signo);

#endif
