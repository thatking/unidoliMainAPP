#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/time.h>
#include <sys/select.h>
#include <pthread.h>

#define OUTTIME_BYTE 20

static int outtimeus = 0;  //us

static pthread_mutex_t serial_mutex = PTHREAD_MUTEX_INITIALIZER;
/*******************************************************************************
* 名称: SerialSetRawmode
* 功能: 开启原始方式
* 形参: termios
* 返回: 无
* 说明: 原始方式(输入不装配成行，也不对特殊字符进行处理)
*******************************************************************************/
static void SerialSetRawmode(struct termios *Option)
{
    Option->c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    Option->c_oflag  &= ~OPOST;   /*Output*/
    Option->c_iflag &= ~(ICRNL | IXON);
}
/*******************************************************************************
* 名称: SerialSetCs8NoSpace
* 功能: 数据位8位无效
* 形参: termios
* 返回: 无
* 说明:
*******************************************************************************/
static void SerialSetCs8NoSpace(struct termios *Option)
{
    Option->c_cflag &= ~PARENB;
    Option->c_cflag &= ~CSTOPB;
    Option->c_cflag &= ~CSIZE;
    Option->c_cflag |= CS8;
}
/*******************************************************************************
* 名称: SerialInit
* 功能: 串口初始化
* 形参: file,串口设备路径;speed,波特率
* 返回: 无
* 说明: speed:B57600,B115200等
*******************************************************************************/
int SerialInit(const char *file,int speed,int trylock)
{
	int fd;
    struct termios opt;
    int spd;
    if(trylock)
    {
    	if(pthread_mutex_trylock(&serial_mutex) > 0)
    		return 0;
    }
    else
    {
        pthread_mutex_lock(&serial_mutex);
    }
    fd = open(file, O_RDWR | O_NOCTTY);    //阻塞读方式
    if(fd > 0)
    {
    	outtimeus = 1000000*9/speed*OUTTIME_BYTE;
    	switch(speed)
    	{
    		case 9600:
    			spd = B9600;
    			break;
    		case 19200:
    			spd = B19200;
    			break;
    		case 38400:
    			spd = B38400;
    			break;
    		case 115200:
    			spd = B115200;
    			break;
    		default:
    			spd = B115200;
    			break;
    	}
        tcgetattr(fd,&opt);
        cfsetispeed(&opt,spd);
        cfsetospeed(&opt,spd);
        SerialSetRawmode(&opt);
        SerialSetCs8NoSpace(&opt);
        tcsetattr(fd,TCSANOW,&opt);
    }
    else
    	 pthread_mutex_unlock(&serial_mutex);
    return fd;
}
/*******************************************************************************
* 名称: SerialReadUnblock
* 功能: 非阻塞读取串口数据
* 形参: mtime,阻塞时间(单位毫秒)
* 返回: 数据长度
* 说明: 无
*******************************************************************************/
int SerialReadUnblock(int fd,char *buf,int maxlen,int mtime)
{
    int len = 0;
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
  	tv.tv_sec = mtime/1000;
    tv.tv_usec = mtime%1000*1000;
    while(1)
    {
		if(select(1+fd, &rfds, NULL, NULL, &tv)>0)
		{
			if (FD_ISSET(fd, &rfds))
			{
				len += read(fd, buf+len, (size_t)maxlen-len);
		    	tv.tv_sec = outtimeus/1000000;
		    	tv.tv_usec = outtimeus%1000000;
			}
		}
		else
		{
			break;
		}
    }
    return len;
}
/*******************************************************************************
* 名称: SerialReadBlock
* 功能: 阻塞读取串口数据
* 形参:
* 返回: 数据长度
* 说明: 无
*******************************************************************************/
int SerialReadBlock(int fd,char *buf,int maxlen)
{
	return read(fd,buf,(size_t)maxlen);
}
/*******************************************************************************
* 名称: SerialWrite
* 功能: 串口发送数据
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
int SerialWrite(int fd,const char *buf,int len)
{
    return write(fd,buf,len);
}
/*******************************************************************************
* 名称: SerialClose
* 功能: 关闭串口
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
int SerialClose(int fd)
{
	int ret = close(fd);
	pthread_mutex_unlock(&serial_mutex);
    return ret;
}
