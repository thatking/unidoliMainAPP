#ifndef __WPA2APP_H_
#define __WPA2APP_H_

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#include <wpa_ctrl.h>

#include "unidoli_audio.h"
#include "sample_comm.h"
#include "app_debug.h"
#include "audio_transfer_fft.h"
#include "../../types.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "modbus_app.h"

#define FILEPATH "/etc/wpa_supplicant.conf"

void configureWIFI(void);
void reportWIFIConnetStatus(void);
char getWIFIConfigureStasus(void);
char getKeyStatus(void);

#endif
