#ifndef SERIAL_H_
#define SERIAL_H_

//int SerialInit(const char *file,int speed);   //串口初始化
int SerialInit(const char *file,int speed,int trylock);
int SerialReadUnblock(int fd,char *buf,int maxlen,int mtime); //非阻塞读取串口数据
int SerialReadBlock(int fd,char *buf,int maxlen); //阻塞读取串口数据
int SerialWrite(int fd,const char *buf,int len); //串口发送数据
int SerialClose(int fd);   //关闭串口

#endif /* SERIAL_H_ */
