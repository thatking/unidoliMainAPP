/*
 ============================================================================
 Name        : unidoliMainAPP.c
 Author      : hp
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "unidoliMainAPP.h"

//test mac:0857001B7862
//{"cmd":"setAbtAngle","vertical":30,"level":60}
//{"cmd":"setAbtAngle","vertical":30,"level":60}
//{"cmd":"update","url":"http://119.23.53.127/test.php"}

const char *domain[] =
{
	"pool.ntp.org",
	"time.nist.gov",
	"time.windows.com"
};

char netStatus     = 0;
char lastNetStatus = 0;

int main(int argc, char *argv[])
{
	char DeviceID[24];
	pthread_t mqtt_pid;
	pthread_t rtmp_pid;
	pthread_t net_pid;
	int i;
	struct addrinfo* cur;
	struct in_addr ip;
	struct timeval t;

	signal(SIGINT,  termination);
	signal(SIGTERM, termination);

	clientID_init();
	getClientID(DeviceID);
	PRINTF("wlan0 mac addr: %s",DeviceID);

	if(getWIFIConfigureStasus() == 0)    //if first use this camera ,you have to configure the WIFI
		configureWIFI();

	reportWIFIConnetStatus();

	while(1)
	{
		i++;
		if(i == 3)   //infinite loop
		{
			i = 0;
		}
		if(getKeyStatus())
			configureWIFI();
		sleep(1);
		PRINTF("check the net status");
		cur = getip(domain[i]);
		if(cur != NULL)
		{
			ip = ((struct sockaddr_in *)cur->ai_addr)->sin_addr;
			if(getNtpTime(*(unsigned int*)&ip,&t))
			{
				settimeofday(&t, NULL);
				PRINTF("set time success!");
				netStatus     = 1;
				lastNetStatus = 1;
				freeaddrinfo(cur);
				break;
			}
		}
		freeaddrinfo(cur);
	}

	if(pthread_create(&net_pid,NULL,networkStatusPthread,NULL) != 0)
	{
		PRINTF("err: Network PTHREAD created fail!");
		return -1;
	}

restart:
	if(pthread_create(&mqtt_pid,NULL,MqttPthread,NULL) != 0)
	{
		PRINTF("err: MQTT PTHREAD created fail!");
		return -1;
	}

	if(pthread_create(&rtmp_pid,NULL,rtmpPthread,NULL) != 0)
	{
		PRINTF("err: RTMP PTHREAD created fail!");
		return -1;
	}

	sleep(5);
	while(1)
	{
		sleep(60);
		PRINTF("net status lastNetStatus : %d netStatus : %d",lastNetStatus,netStatus);
		if(lastNetStatus == 0 && netStatus ==1)
		{
			lastNetStatus = netStatus;
			goto restart;
		}
		else if(lastNetStatus == 1 && netStatus == 0)
		{
			lastNetStatus = netStatus;
			raise(SIGUSR2);
			raise(SIGUSR1);
		}
		netStatus = 0;
	}
	return EXIT_SUCCESS;
}

void *networkStatusPthread(void *arg)
{
	int i;
	struct addrinfo* cur;

	while(1)
	{
		sleep(3);
		i++;
		if(i == 3)   //infinite loop
		{
			i = 0;
		}
		if(getKeyStatus() == 1)
		{
			raise(SIGUSR2);
			raise(SIGUSR1);
			configureWIFI();
		}
		cur = getip(domain[i]);
		if(cur != NULL)
			netStatus = 1;
		freeaddrinfo(cur);
	}
	return EXIT_SUCCESS;
}

void termination(int signo)
{
	raise(SIGUSR2);
	raise(SIGUSR1);
	exit(-1);
}

