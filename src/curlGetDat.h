/*
 * getUrl.h
 *
 *  Created on: Sep 28, 2017
 *      Author: root
 */
#ifndef __GETURL_H_
#define __GETURL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#include "json/cJSON.h"
#include "libcurl4.4.0/include/curl/curl.h"
#include "curl/easy.h"
#include "types.h"

//#define BLUE_FONT "\033[40;34m%s\033[0m "
//#define RED_FONT "\033[40;31m%s\033[0m "
//#define GREEN_FONT "\033[40;32m%s\033[0m "
//#define YELLOW_FONT "\033[40;33m%s\033[0m "
//#define PURPLE_FONT "\033[40;35m%s\033[0m "
//#define DGREEN_FONT "\033[40;36m%s\033[0m "
//#define WHITE_FONT "\033[40;37m%s\033[0m "

char getpushurl(char* rtmpUrl,char *client_id);
char getSecret(char* secret,char *client_id);
size_t write_data_rtmp(void *ptr, size_t size, size_t nmemb, void *stream);
size_t write_data_secret(void *ptr, size_t size, size_t nmemb, void *stream);

#endif

