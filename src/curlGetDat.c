/*
 * getUrl.c
 *
 *  Created on: Sep 28, 2017
 *      Author: root
 */
#include "curlGetDat.h"

/* for curl callback */
char *rtmp_dat   = NULL;
char *secret_dat = NULL;
static pthread_mutex_t curl_mutex = PTHREAD_MUTEX_INITIALIZER;

char getpushurl(char* rtmpUrl,char *client_id)
{
	char rtmp_url[256];
	pthread_mutex_lock(&curl_mutex);
	if(snprintf(rtmp_url, sizeof(rtmp_url), "http://live.unidoli.com/sgsl/v1/getPushUrl.php?user_id=%s", client_id) < 0)
	{
		PRINTF(RED_FONT, "[ERROR]");
		PRINTF("ID too long!");
		return -1;
	}
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, rtmp_url);
	//curl_easy_setopt(curl, CURLOPT_DNS_SERVERS, "8.8.8.8:53");
//	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
	//CURLOPT_WRITEFUNCTION 将后继的动作交给write_data函数处理
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_rtmp);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	while(1)
	{
		/* dat use malloc!!! */
		CURLcode ret = curl_easy_perform(curl);
		//third segmentation fault
		if(ret != CURLE_OK)
		{
			PRINTF(RED_FONT, "[ERROR]");
			PRINTF("%s", curl_easy_strerror(ret));
			goto con;
		}
		cJSON* json = cJSON_Parse(rtmp_dat);
		cJSON* data  = cJSON_GetObjectItem(json,"data");
		cJSON* content  = cJSON_GetObjectItem(data,"content");
		cJSON* push_url  = cJSON_GetObjectItem(content,"push_url");
		memcpy(rtmpUrl, push_url->valuestring, strlen(push_url->valuestring));
		rtmpUrl[strlen(push_url->valuestring)] = '\0';
		cJSON_Delete(json);
		free(rtmp_dat);
		rtmp_dat = NULL;
		curl_easy_cleanup(curl);
		curl = NULL;
		curl_global_cleanup();
		pthread_mutex_unlock(&curl_mutex);
		return 0;

		con:
		sleep(1);
	}
}

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data_rtmp(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = size*nmemb;

    rtmp_dat = malloc(written+1);
    memcpy(rtmp_dat, ptr, written);
    rtmp_dat[written] = '\0';
    return written;
}

char getSecret(char* secret,char *client_id)
{
	char post_par[256];
	pthread_mutex_lock(&curl_mutex);
	if(snprintf(post_par, sizeof(post_par), "ProductKey=mivZEajJdbC&DeviceName=%s", client_id) < 0)
	{
		PRINTF(RED_FONT, "[ERROR]");
		PRINTF("ID too long!");
		return -1;
	}
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL,"http://mqtt.unidoli.com/api/QueryDeviceByName");

	/* Now specify the POST data */
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,post_par);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_secret); //回调函数，可有可无
//	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L);
	curl_easy_setopt(curl, CURLOPT_POST, 1);                   //设置libcurl发送的协议
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);                //设置http数据头
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);         //设置返回的数据量
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

	while(1)
	{
		/* dat use malloc!!! */
		CURLcode ret = curl_easy_perform(curl);
		if(ret != CURLE_OK)
		{
			PRINTF(RED_FONT, "[ERROR]");
			PRINTF("%s\n", curl_easy_strerror(ret));
			goto con;
		}
		cJSON* json = cJSON_Parse(secret_dat);
		cJSON* data1  = cJSON_GetObjectItem(json,"data");
		cJSON* data2  = cJSON_GetObjectItem(data1,"data");
		cJSON* DeviceInfo  = cJSON_GetObjectItem(data2,"DeviceInfo");
		cJSON* DeviceSecret  = cJSON_GetObjectItem(DeviceInfo,"DeviceSecret");
		memcpy(secret, DeviceSecret->valuestring, strlen(DeviceSecret->valuestring));
		secret[strlen(DeviceSecret->valuestring)] = '\0';
		cJSON_Delete(json);
		free(secret_dat);
		secret_dat = NULL;
		curl_easy_cleanup(curl);
		curl = NULL;
		curl_global_cleanup();
		pthread_mutex_unlock(&curl_mutex);
		return 0;

		con:
		sleep(1);
	}
}

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data_secret(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = size*nmemb;

    secret_dat = malloc(written+1);
    memcpy(secret_dat, ptr, written);
    secret_dat[written] = '\0';
    return written;
}
