/*
 ============================================================================
 Name        : rtmpTest.c
 Author      : hp
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "rtmp.h"

//static int count = 0;
static struct itimerval oldtv;

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
char rtmp_status = 0;

void rtmpStop(int signo)
{
    if (SIGUSR1 == signo)
    {
    	PRINTF("------------------------------------");
    	unidoli_rtmp_pthread_stop();
    	PRINTF("rtmp_thread termination!");
    }
    rtmp_status = 0;
}

void set_timer(void)
{
    struct itimerval itv;
    itv.it_interval.tv_sec = 60 * 60 *11;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 60 * 60 *11;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, &oldtv);
}

void rtmpStart(int signo)
{
	char ret = -1;
	int fd;
	char buffer[BUFFER_SIZE];
	char *p = buffer;
	int readNumOfByte = 0;
	int tryReadNumOfByte = 12;
	int sumRead = 0;
	char clientId[24];
	UNIDOLI_RTMP_CONFIG_S conf;
	char autoUrl[256];

	if ((signo == SIGALRM || signo == SIGCHLD) && rtmp_status == 0)
		goto EXIT;
	rtmp_status = 1;
	PRINTF("RTMP start");
	ret = unidoli_rtmp_pthread_stop();
	if(ret < 0)
	{
		PRINTF("unidoli_rtmp_pthread_stop failed!");
	}
	if((fd=open("./unidoli.conf",O_RDONLY))==-1)
	{
		PRINTF("open unidoli.config error!");
		exit(1);
	}
	do
	{
		p += readNumOfByte;
		readNumOfByte = read(fd,p,tryReadNumOfByte);
		if(readNumOfByte == -1)
		{
			PRINTF("read error");
		}
		sumRead += readNumOfByte;
	}
	while(readNumOfByte == tryReadNumOfByte);
	buffer[sumRead] = '\0';
	close(fd);

	cJSON* jsonFiel = cJSON_Parse(buffer);
	cJSON* resolution = cJSON_GetObjectItem(jsonFiel,"resolution");
	cJSON* frameRate = cJSON_GetObjectItem(jsonFiel,"frameRate");
	cJSON* bitRate = cJSON_GetObjectItem(jsonFiel,"bitRate");
	cJSON* urlFrom = cJSON_GetObjectItem(jsonFiel,"urlFrom");
	cJSON* rtmpUrl = cJSON_GetObjectItem(jsonFiel,"rtmpUrl");

	if((strcmp(urlFrom->valuestring,"auto")) == 0)
	{
		getClientID(clientId);
		getpushurl(autoUrl,clientId);
		PRINTF("autoUrl : %s",autoUrl);
		strcpy(conf.rtmp_url, autoUrl);
	}
	else
	{
		strcpy(conf.rtmp_url, rtmpUrl->valuestring);
	}
	conf.frm_rate = frameRate->valueint;
	conf.bit_rate = bitRate->valueint;
	if(resolution->valueint == 720)
		conf.enc_size = UNIDOLI_PIC_HD720;
	else if(resolution->valueint == 1080)
		conf.enc_size = UNIDOLI_PIC_HD1080;
	conf.en_audio = 1;
	conf.en_video = 1;
	unidoli_rtmp_set_config(&conf);
	ret = unidoli_rtmp_pthread_start();
	if(ret < 0)
	{
		PRINTF("unidoli_rtmp_pthread_start failed!");
	}
	cJSON_Delete(jsonFiel);
EXIT:
	PRINTF("end of the RTMP start!");
}

void *rtmpPthread(void *arg)
{
	signal(SIGUSR1, rtmpStop);
	signal(SIGALRM, rtmpStart);
	signal(SIGCHLD, rtmpStart);

	set_timer();
	rtmpStart(SIGIO);

	sleep(60);
	return EXIT_SUCCESS;
}

