#include "network.h"

typedef struct _ntp_time
{
    unsigned int coarse;
    unsigned int fine;
} ntp_time;

struct ntp_packet
{
     unsigned char leap_ver_mode;
     unsigned char startum;
     char poll;
     char precision;
     int  root_delay;
     int  root_dispersion;
     int reference_identifier;
     ntp_time reference_timestamp;
     ntp_time originage_timestamp;
     ntp_time receive_timestamp;
     ntp_time transmit_timestamp;
};

struct addrinfo *getip(const char *ip)
{
	struct addrinfo hints;
	struct addrinfo *res=NULL;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; /* Allow IPv4 */
	hints.ai_flags = AI_PASSIVE; /* For wildcard IP address */
	hints.ai_protocol = 0; /* Any protocol */
	hints.ai_socktype = SOCK_STREAM;
	if(getaddrinfo(ip, NULL,&hints,&res)==-1)
		return NULL;
	return res;
}


/*******************************************************************************
* 名称: udpSend
* 功能: udp发送并等待回复
* 形参:
* 返回:
* 说明: 超时时间2s
*******************************************************************************/
static int udpSend(unsigned int ip,unsigned short port,char *smsg,int slen,char *rmsg,int *rlen,int rlenmax)
{
	struct sockaddr_in serveraddr;
	/* 创建套接字 */
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd <0 )
	{
		perror("socket");
		return 0;
	}
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = ip;
	if (sendto(sockfd, smsg,slen, 0, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0)
	{
		perror("sendto");
		return 0;
	}

	socklen_t addrlen;
    fd_set pending_data;
    struct timeval block_time;
    /*调用select()函数，并设定超时时间为2s*/
    FD_ZERO(&pending_data);
    FD_SET(sockfd, &pending_data);
    block_time.tv_sec=2;
    block_time.tv_usec=0;
    if (select(sockfd + 1, &pending_data, NULL, NULL, &block_time) > 0)
	{
		if ((*rlen = recvfrom(sockfd, rmsg,rlenmax, 0,
				(struct sockaddr *)&serveraddr,&addrlen)) > 0)
		{
			return 1;
		}
	}
    return 0;
}
/*******************************************************************************
* 名称: construct_packet
* 功能: 构建NTP协议包
* 形参:
* 返回:
* 说明:
*******************************************************************************/
static int constructPacket(char *packet)
{
	long tmp_wrd;
	time_t timer;
	tmp_wrd = htonl((LI << 30)|(VN << 27)
	  |(MODE << 24)|(STRATUM << 16)|(POLL << 8)|(PREC & 0xff));
	memcpy(packet, &tmp_wrd, sizeof(tmp_wrd));

	/*设置Root Delay、Root Dispersion和Reference Indentifier */
	tmp_wrd = htonl(1<<16);
	memcpy(&packet[4], &tmp_wrd, sizeof(tmp_wrd));
	memcpy(&packet[8], &tmp_wrd, sizeof(tmp_wrd));
	/*设置Timestamp部分*/
	time(&timer);
	/*设置Transmit Timestamp coarse*/
	tmp_wrd = htonl(JAN_1970 + (long)timer);
	memcpy(&packet[40], &tmp_wrd, sizeof(tmp_wrd));
	/*设置Transmit Timestamp fine*/
	tmp_wrd = htonl((long)NTPFRAC(timer));
	memcpy(&packet[44], &tmp_wrd, sizeof(tmp_wrd));
	return NTP_PCK_LEN;
}
/*******************************************************************************
* 名称: AnalysisPacket
* 功能: 解析NTP协议包
* 形参:
* 返回:
* 说明:
*******************************************************************************/
static int AnalysisPacket(char *data,int len,struct ntp_packet *ret_time)
{
	if(len < NTP_PCK_LEN)
		return 0;
	/* 设置接收NTP包的数据结构 */
	ret_time->leap_ver_mode = ntohl(data[0]);
	ret_time->startum = ntohl(data[1]);
	ret_time->poll = ntohl(data[2]);
	ret_time->precision = ntohl(data[3]);
	ret_time->root_delay = ntohl(*(int*)&(data[4]));
	ret_time->root_dispersion = ntohl(*(int*)&(data[8]));
	ret_time->reference_identifier = ntohl(*(int*)&(data[12]));
	ret_time->reference_timestamp.coarse = ntohl (*(int*)&(data[16]));
	ret_time->reference_timestamp.fine = ntohl(*(int*)&(data[20]));
	ret_time->originage_timestamp.coarse = ntohl(*(int*)&(data[24]));
	ret_time->originage_timestamp.fine = ntohl(*(int*)&(data[28]));
	ret_time->receive_timestamp.coarse = ntohl(*(int*)&(data[32]));
	ret_time->receive_timestamp.fine = ntohl(*(int*)&(data[36]));
	ret_time->transmit_timestamp.coarse = ntohl(*(int*)&(data[40]));
	ret_time->transmit_timestamp.fine = ntohl(*(int*)&(data[44]));
	return 1;
}
/*******************************************************************************
* 名称:
* 功能:
* 形参:
* 返回:
* 说明:
*******************************************************************************/
int getNtpTime(unsigned int ntpip,struct timeval *tv)
{
	char msg[NTP_PCK_LEN*8];
	int msglen = constructPacket(msg);
	if(udpSend(ntpip,NTP_PORT,msg,msglen,msg,&msglen,NTP_PCK_LEN*8)>0)
	{
		struct ntp_packet new_time_packet;
		if(AnalysisPacket(msg,msglen,&new_time_packet)>0)
		{
			tv->tv_sec = new_time_packet.transmit_timestamp.coarse - JAN_1970+28800;
			tv->tv_usec = USEC(new_time_packet.transmit_timestamp.fine);
			return 1;
		}
	}
	return 0;
}
