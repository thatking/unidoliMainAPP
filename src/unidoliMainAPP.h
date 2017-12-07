#ifndef __UNIDOLIMAINAPP_H_
#define __UNIDOLIMAINAPP_H_

#include <stdio.h>
#include <stdlib.h>

#include "getaddr/getMacAddr.h"
#include "rtmp.h"
#include "iot.h"
#include "modbus_app.h"
#include "curlGetDat.h"
#include "network.h"
#include "./wpa2/src/wpa2APP.h"

void *networkStatusPthread(void *arg);
void termination(int signo);

#endif
