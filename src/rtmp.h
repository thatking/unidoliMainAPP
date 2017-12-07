/*
 * rtmp.h
 *
 *  Created on: Sep 30, 2017
 *      Author: root
 */
#ifndef __RTMP_H_
#define __RTMP_H_

#include <stdio.h>
#include <stdlib.h>
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
#include <unidoli_rtmp_config.h>
#include <signal.h>
#include "curlGetDat.h"

#define BUFFER_SIZE 1024

void rtmpStop(int signo);
void set_timer(void);
void rtmpStart(int signo);
void *rtmpPthread(void *arg);

#endif

