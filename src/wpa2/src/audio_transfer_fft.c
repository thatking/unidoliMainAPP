#include "audio_transfer_fft.h"

#define AUDIO_TRANSFER_FREE_FREQ 9000
#define AUDIO_TRANSFER_DAT(f, s, l) (f/(s/l)-6)
#define AUDIO_TRANSFER_GET_BYTE(a) ((Freq_Dat_buffer[a]<<6)|(Freq_Dat_buffer[a+1]<<4)|(Freq_Dat_buffer[a+2]<<2)|Freq_Dat_buffer[a+3])

float *audio_cos_table = NULL;
float *audio_sin_table = NULL;
static uint32_t Last_freqs[10];

unsigned int audio_transfer_getTime0()
{
    struct timeval tm;
    static unsigned char tm_sign = 0;
    static unsigned int tm_v_ms = 0;
    gettimeofday(&tm, 0);
    if(tm_sign == 0){
        tm_sign = 1;
        tm_v_ms = tm.tv_sec*1000 + tm.tv_usec/1000;
    }
    return tm.tv_sec*1000 + tm.tv_usec/1000 - tm_v_ms;
}

/*
 *sinwave_table init
 */
int audio_transfer_init(uint32_t len)
{
	const float pi = 3.14159265;
	int i, k, j = 0;

	audio_cos_table = (float *)malloc(len*len/2*sizeof (float));
	audio_sin_table = (float *)malloc(len*len/2*sizeof (float));
	if(audio_sin_table == NULL || audio_cos_table == NULL){
		printf("NULL %d\n", len*len/2*sizeof (float));
	}else{
		printf("malloc %d\n", len*len/2*sizeof (float));
	}

	//printf("1\n");
	for(k=0;k<len/2;k++)
	{
		for(i=0;i<len;i++)
		{
			audio_cos_table[j] = (float)cos(2*pi*k*i/len);
			audio_sin_table[j] = (float)sin(2*pi*k*i/len);
			j++;
		}
	}
	//printf("2\n");
	for(i=0;i<10;i++)
	{
		Last_freqs[i] = 0;
	}
	

	return 0;
}

int audio_transfer_fft(short *table, uint32_t len, float *rex, float *imx)
{
	int i, k, j = 0;

	for(i=0;i<len/2;i++)
	{
		rex[i] = 0;
		imx[i] = 0;
	}

	for(k=0;k<len/2;k++)
	{
		for(i=0;i<len;i++)
		{
			rex[k] += table[i] * audio_cos_table[j];
			imx[k] -= table[i] * audio_sin_table[j];
			j++;
		}
	}

	return 0;
}

int audio_transfer_find_max_freq(uint32_t samprate, short *table, uint32_t len)
{
	float rex[512];
	float imx[512];
	float Max_A = 0;
	uint32_t Max_Freq = 0;
	float temp;
	uint32_t i;
	//FFT
	//i = audio_transfer_getTime0();
	audio_transfer_fft(table, len, (float *)&rex, (float *)&imx);
	//i = audio_transfer_getTime0() - i;
	//printf("time: %d\n", i);
	//FIND MAX
	for(i=1;i<len/2;i++)
	{
		temp = sqrt(rex[i]*rex[i]+imx[i]*imx[i])/( len/2 );
		if(temp > Max_A)
		{
			Max_A = temp;
			Max_Freq = i;
		}
		//if(i*900==6300 || i*900==5400 || i*900==4500 || i*900==3600){
		//	printf("                       %d: %f\n", i*900, temp);
		//}
	}
	return Max_Freq*samprate/len;
}

int audio_transfer_find_most_freq(uint32_t *list_freq, uint32_t count)
{
	int i;
	uint8_t list_count[50];
	uint8_t max = 0;
	for(i=0;i<50;i++)
	{
		list_count[i] = 0;
	}
	for(i=0;i<count;i++)
	{
		list_count[list_freq[i]/900]++;
	}
	for(i=0;i<50;i++)
	{
		if(list_count[i] >= list_count[max])
		{
			max = i;
		}
	}
	return max*900;
}

/*
 * 分析编码后的PCM数据
 * table:   样本
 * samprate:采样率
 * len:     样本数
 */
int audio_transfer_analysis(uint32_t samprate, short *table, uint32_t len, void *buf)
{
	uint32_t Max_Freq = 0;
	uint32_t Most_Freq = 0;
	uint32_t i;
	static uint8_t Freq_Dat_buffer[512];
	static uint8_t ptr = 0;
	static uint8_t Dat_Len = 0;
	static uint32_t Last_Freq = 0;

	/* 找出最大幅值频率点 */
	Max_Freq = audio_transfer_find_max_freq(samprate, table, len);

	/* 记录最近10次历史 */
	for(i=9;i>0;i--)
	{
		Last_freqs[i] = Last_freqs[i-1];
	}
	Last_freqs[0] = Max_Freq;

	/* 筛选出出现次数最多的频率 */
	Most_Freq = audio_transfer_find_most_freq(Last_freqs, 10);

	
	//if(Most_Freq == AUDIO_TRANSFER_FREE_FREQ)
	//{
	//	printf("n\n");
	//}
	//else //if(Most_Freq < 9900 && Most_Freq > 4500)
	//{
	//	printf("Freq : %d %01x\n", Most_Freq, AUDIO_TRANSFER_DAT(Most_Freq, samprate, len));
	//}


	/* 去重 */
	if(Most_Freq != Last_Freq)
	{
		/* 接收数据处理 */
		if(Most_Freq == AUDIO_TRANSFER_FREE_FREQ)
		{
			//printf("n\n");
		}
		else if(Most_Freq < 9900 && Most_Freq > 4500)
		{
			Freq_Dat_buffer[ptr] = AUDIO_TRANSFER_DAT(Most_Freq, samprate, len);
			//printf("Freq [ptr:%d]: %d %01x\n", ptr, Most_Freq, AUDIO_TRANSFER_DAT(Most_Freq, samprate, len));
			if( (Freq_Dat_buffer[ptr] == 0 && ptr == 0) ||
			    (Freq_Dat_buffer[ptr] == 1 && ptr == 1) ||
				(Freq_Dat_buffer[ptr] == 1 && ptr == 2) ||
			    (Freq_Dat_buffer[ptr] == 3 && ptr == 3) ||
			    (Freq_Dat_buffer[ptr] == 0 && ptr == 4) ||
			    (Freq_Dat_buffer[ptr] == 1 && ptr == 5) ||
				(Freq_Dat_buffer[ptr] == 1 && ptr == 6) ||
			    (Freq_Dat_buffer[ptr] == 3 && ptr == 7) ||
				(ptr >= 8 && ptr < 12))
			{
				/* 接收包头 */
				ptr++;
				if(ptr == 12)
				{
					Dat_Len = AUDIO_TRANSFER_GET_BYTE(8);
					printf("S\n");
				}
			}
			else if(ptr >= 12 && ptr < Dat_Len+12)
			{
				/* 接收数据 */
				ptr++;
			}
			else if((Freq_Dat_buffer[ptr] == 0 && ptr == Dat_Len+12 +0) ||
			    	(Freq_Dat_buffer[ptr] == 1 && ptr == Dat_Len+12 +1) ||
					(Freq_Dat_buffer[ptr] == 2 && ptr == Dat_Len+12 +2) ||
			    	(Freq_Dat_buffer[ptr] == 0 && ptr == Dat_Len+12 +3))
			{
				/* 接收包尾 */
				ptr++;
				if(ptr == Dat_Len+12 +4)
				{
					int n = 0;
					for(i=12;i<Dat_Len+12;i+=4)
					{
						((char *)buf)[n++] = AUDIO_TRANSFER_GET_BYTE(i);
					}
					Dat_Len = 0;
					ptr = 0;

					/* RETURN RESULT */
					Last_Freq = Most_Freq;
					return n;
				}
			}
			else
			{
				/* 无效数据 */
				ptr = 0;
			}
		}
		Last_Freq = Most_Freq;
	}

	return -1;
}

int audio_transfer_exit()
{
	free(audio_cos_table);
	audio_cos_table = NULL;
	free(audio_sin_table);
	audio_sin_table = NULL;
	return 0;
}
