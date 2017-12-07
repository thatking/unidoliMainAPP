#ifndef _UNIDOLI_RTMP_CONFIG_H
#define _UNIDOLI_RTMP_CONFIG_H

#include <stdint.h>
#include <pthread.h>

//rtmp_mode
#define UNIDOLI_MODE_RTMP 0
#define UNIDOLI_MODE_API  1

//en_audio
#define UNIDOLI_AUDIO_OFF 0
#define UNIDOLI_AUDIO_ON  1

//en_video
#define UNIDOLI_VIDEO_OFF 0
#define UNIDOLI_VIDEO_ON  1

typedef enum _unidoliPIC_SIZE_E
{
    UNIDOLI_PIC_QCIF = 0,
    UNIDOLI_PIC_CIF,
    UNIDOLI_PIC_2CIF,
    UNIDOLI_PIC_HD1,
    UNIDOLI_PIC_D1,
    UNIDOLI_PIC_960H,

    UNIDOLI_PIC_QVGA,    /* 320 * 240 */
    UNIDOLI_PIC_VGA,     /* 640 * 480 */
    UNIDOLI_PIC_XGA,     /* 1024 * 768 */
    UNIDOLI_PIC_SXGA,    /* 1400 * 1050 */
    UNIDOLI_PIC_UXGA,    /* 1600 * 1200 */
    UNIDOLI_PIC_QXGA,    /* 2048 * 1536 */

    UNIDOLI_PIC_WVGA,    /* 854 * 480 */
    UNIDOLI_PIC_WSXGA,   /* 1680 * 1050 */
    UNIDOLI_PIC_WUXGA,   /* 1920 * 1200 */
    UNIDOLI_PIC_WQXGA,   /* 2560 * 1600 */

    UNIDOLI_PIC_HD720,   /* 1280 * 720 */
    UNIDOLI_PIC_HD1080,  /* 1920 * 1080 */
    UNIDOLI_PIC_2304x1296, /* 3M:2304 * 1296 */
    UNIDOLI_PIC_2592x1520, /* 4M:2592 * 1520 */
    UNIDOLI_PIC_5M,        /* 2592 * 1944 */
    UNIDOLI_PIC_UHD4K,     /* 3840 * 2160 */
    UNIDOLI_PIC_3Kx3K,      /* 3000 * 3000 */
	UNIDOLI_PIC_12M,     	/* 4000 * 3000 */
	UNIDOLI_PIC_16M,     	/* 4608 * 3456 */

    UNIDOLI_PIC_BUTT
} UNIDOLI_PIC_SIZE_E;

typedef struct __unidoli_rtmp_conf_s {
	uint16_t frm_rate;
	uint32_t bit_rate;
	UNIDOLI_PIC_SIZE_E enc_size;
	char rtmp_url[256];
	uint8_t en_audio;
	uint8_t en_video;
	char client_id[100];      //Optional

}UNIDOLI_RTMP_CONFIG_S;


typedef struct __unidoli_rtmp_message_s {
	UNIDOLI_RTMP_CONFIG_S conf;
	uint32_t video_megabyte;
	uint32_t audio_megabyte;
	uint32_t push_timestamp;
	uint8_t status;
	pthread_t pthread_id;
}UNIDOLI_RTMP_MESSAGE_S;

int unidoli_rtmp_pthread_start();
int unidoli_rtmp_pthread_stop();
int unidoli_rtmp_set_config(UNIDOLI_RTMP_CONFIG_S *conf);

#endif

