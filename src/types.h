/*
 * types.h
 *
 *  Created on: Jun 26, 2017
 *      Author: root
 */

#ifndef __TYPES_H_
#define __TYPES_H_

#include "stdio.h"
#include"time.h"

typedef int  s32;
typedef short int s16;
typedef signed char  s8;

typedef const int sc32;  /*!< Read Only */
typedef const short int sc16;  /*!< Read Only */
typedef const signed char sc8;   /*!< Read Only */

typedef unsigned int  u32;
typedef unsigned short int u16;
typedef unsigned char u8;

typedef const unsigned int uc32;  /*!< Read Only */
typedef const unsigned short int uc16;  /*!< Read Only */
typedef const unsigned char uc8;   /*!< Read Only */

#define BLUE_FONT "\033[40;34m%s\033[0m "
#define RED_FONT "\033[40;31m%s\033[0m "
#define GREEN_FONT "\033[40;32m%s\033[0m "
#define YELLOW_FONT "\033[40;33m%s\033[0m "
#define PURPLE_FONT "\033[40;35m%s\033[0m "
#define DGREEN_FONT "\033[40;36m%s\033[0m "
#define WHITE_FONT "\033[40;37m%s\033[0m "
#define PRINTF(fmt, args...)  \
    do {\
    	time_t rawtime;\
		struct tm * timeinfo;\
		char *p;\
		time ( &rawtime );\
		timeinfo = localtime ( &rawtime );\
		p = asctime (timeinfo);\
		p[strlen(p)-1] = '\0';\
		printf ( "%s  ",p);\
        printf("%s|%03d :: ", __func__, __LINE__);\
        printf(fmt, ##args);\
        printf("%s", "\r\n");\
    } while(0)

void printfhex(u8 *dat,u8 len);
#endif /* TYPES_H_ */
