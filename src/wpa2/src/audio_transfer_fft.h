#ifndef _AUDIO_TRANSFER_H
#define _AUDIO_TRANSFER_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

int audio_transfer_init(uint32_t len);
int audio_transfer_analysis(uint32_t samprate, short *table, uint32_t len, void *buf);
int audio_transfer_exit();

#endif
