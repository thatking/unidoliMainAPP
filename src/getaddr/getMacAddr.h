#ifndef _UNIDOLI_GETPUSHADDR_H
#define _UNIDOLI_GETPUSHADDR_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "../types.h"

int clientID_init(void);
void getClientID(char *p);

#endif
