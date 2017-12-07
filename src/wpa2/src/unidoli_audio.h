#ifndef _UNIDOLI_AUDIO_H
#define _UNIDOLI_AUDIO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>

#include "sample_comm.h"
#include "acodec.h"
//#include "tlv320aic31.h"

#define audio_debug(s32Ret)\
do{\
    printf("s32Ret=%#x,fuc:%s,line:%d\n", s32Ret, __FUNCTION__, __LINE__);\
}while(0)

#define  ACODEC_FILE  "/dev/acodec"

typedef struct tagSAMPLE_AI_S
{
    HI_BOOL bStart;
    HI_S32  AiDev;
    HI_S32  AiChn;
    HI_S32  AencChn;
    HI_S32  AoDev;
    HI_S32  AoChn;
    HI_BOOL bSendAenc;
    HI_BOOL bSendAo;
    pthread_t stAiPid;
} SAMPLE_AI_S;



HI_S32 unidoli_audio_service_config(AUDIO_DEV AiDev, AUDIO_DEV AoDev, AUDIO_SAMPLE_RATE_E sample_rast, int num_perfrm);
HI_S32 unidoli_stop_ai(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, HI_BOOL bVqeEn);
HI_S32 unidoli_stop_ao(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bVqeEn);
int unidoli_aduio_read(AUDIO_DEV AiDev, AI_CHN AiChn, void *buf, uint32_t max_len, uint32_t *pts, uint32_t *id);
int unidoli_aduio_send(AUDIO_DEV AoDev, AO_CHN AoChn, void *buf, uint32_t len, uint32_t ms_timeout);

#endif
