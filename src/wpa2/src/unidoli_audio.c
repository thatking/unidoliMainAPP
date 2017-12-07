/******************************************************************************
    UNIDOLI_AUDIO.C
 ******************************************************************************
    Modification:  2017-9 Created
******************************************************************************/
#include "unidoli_audio.h"

HI_S32 unidoli_codec_config_audio(AUDIO_SAMPLE_RATE_E enSample)
{
    HI_S32 fdAcodec = -1;
    HI_S32 ret = HI_SUCCESS;
    ACODEC_FS_E i2s_fs_sel = 0;
    int iAcodecInputVol = 0;
    ACODEC_MIXER_E input_mode = 0;

    fdAcodec = open(ACODEC_FILE, O_RDWR);
    if (fdAcodec < 0)
    {
        printf("%s: can't open Acodec,%s\n", __FUNCTION__, ACODEC_FILE);
        return HI_FAILURE;
    }
    if (ioctl(fdAcodec, ACODEC_SOFT_RESET_CTRL))
    {
        printf("Reset audio codec error\n");
    }

    switch (enSample)
    {
        case AUDIO_SAMPLE_RATE_8000:
            i2s_fs_sel = ACODEC_FS_8000;
            break;

        case AUDIO_SAMPLE_RATE_16000:
            i2s_fs_sel = ACODEC_FS_16000;
            break;

        case AUDIO_SAMPLE_RATE_32000:
            i2s_fs_sel = ACODEC_FS_32000;
            break;

        case AUDIO_SAMPLE_RATE_11025:
            i2s_fs_sel = ACODEC_FS_11025;
            break;

        case AUDIO_SAMPLE_RATE_22050:
            i2s_fs_sel = ACODEC_FS_22050;
            break;

        case AUDIO_SAMPLE_RATE_44100:
            i2s_fs_sel = ACODEC_FS_44100;
            break;

        case AUDIO_SAMPLE_RATE_12000:
            i2s_fs_sel = ACODEC_FS_12000;
            break;

        case AUDIO_SAMPLE_RATE_24000:
            i2s_fs_sel = ACODEC_FS_24000;
            break;

        case AUDIO_SAMPLE_RATE_48000:
            i2s_fs_sel = ACODEC_FS_48000;
            break;

        case AUDIO_SAMPLE_RATE_64000:
            i2s_fs_sel = ACODEC_FS_64000;
            break;

        case AUDIO_SAMPLE_RATE_96000:
            i2s_fs_sel = ACODEC_FS_96000;
            break;
    
        default:
            printf("%s: not support enSample:%d\n", __FUNCTION__, enSample);
            ret = HI_FAILURE;
            break;
    }    

    if (ioctl(fdAcodec, ACODEC_SET_I2S1_FS, &i2s_fs_sel))
    {
        printf("%s: set acodec sample rate failed\n", __FUNCTION__);
        ret = HI_FAILURE;
    }

    /*input mode(depend on hardware):ACODEC_MIXER_IN0--Single-Ended Inputs;ACODEC_MIXER_IN_D--Differential Inputs*/
    input_mode = ACODEC_MIXER_IN1;
    if (ioctl(fdAcodec, ACODEC_SET_MIXER_MIC, &input_mode)) 
    {
        printf("%s: select acodec input_mode failed\n", __FUNCTION__);
        ret = HI_FAILURE;
    }
    if (1) /* should be 1 when micin */
    {
        /******************************************************************************************
        The input volume range is [-87, +86]. Both the analog gain and digital gain are adjusted.
        A larger value indicates higher volume.
        For example, the value 86 indicates the maximum volume of 86 dB,
        and the value -87 indicates the minimum volume (muted status).
        The volume adjustment takes effect simultaneously in the audio-left and audio-right channels.
        The recommended volume range is [+10, +56].
        Within this range, the noises are lowest because only the analog gain is adjusted,
        and the voice quality can be guaranteed.
        *******************************************************************************************/
        iAcodecInputVol = 40;
        if (ioctl(fdAcodec, ACODEC_SET_INPUT_VOL, &iAcodecInputVol))
        {
            printf("%s: set acodec micin volume failed\n", __FUNCTION__);
            return HI_FAILURE;
        }

    }

    close(fdAcodec);
    return ret;
}

/******************************************************************************
* function : Start Ai
******************************************************************************/
HI_S32 unidoli_start_ai(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, AIO_ATTR_S* pstAioAttr, HI_VOID* pstAiVqeAttr, HI_U32 u32AiVqeType)
{
    HI_S32 i;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AI_SetPubAttr(AiDevId, pstAioAttr);
    if (s32Ret)
    {
        printf("%s: HI_MPI_AI_SetPubAttr(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
        return s32Ret;
    }

    s32Ret = HI_MPI_AI_Enable(AiDevId);
    if (s32Ret)
    {
        printf("%s: HI_MPI_AI_Enable(%d) failed with %#x\n", __FUNCTION__, AiDevId, s32Ret);
        return s32Ret;
    }

    for (i = 0; i < s32AiChnCnt; i++)
    {
        s32Ret = HI_MPI_AI_EnableChn(AiDevId, i/(pstAioAttr->enSoundmode + 1));
        if (s32Ret)
        {
            printf("%s: HI_MPI_AI_EnableChn(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
            return s32Ret;
        }

        if (NULL != pstAiVqeAttr)
        {
            HI_BOOL bAiVqe = HI_TRUE;
            switch (u32AiVqeType)
            {
                case 0:
                    s32Ret = HI_SUCCESS;
                    bAiVqe = HI_FALSE;
                    break;
                case 1:
                    s32Ret = HI_MPI_AI_SetTalkVqeAttr(AiDevId, i, SAMPLE_AUDIO_AO_DEV, i, (AI_TALKVQE_CONFIG_S *)pstAiVqeAttr);
                    break;
                default:
                    s32Ret = HI_FAILURE;
                    break;
            }
            if (s32Ret)
            {
                printf("%s: SetAiVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AiVqeType, AiDevId, i, s32Ret);
                return s32Ret;
            }
            if (bAiVqe)
            {
            s32Ret = HI_MPI_AI_EnableVqe(AiDevId, i);
            if (s32Ret)
            {
                printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AiDevId, i, s32Ret);
                return s32Ret;
            }
        }
    }
    }
    
    return HI_SUCCESS;
}

HI_S32 unidoli_start_ao(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt,
                                 AIO_ATTR_S* pstAioAttr, HI_VOID* pstAoVqeAttr, HI_U32 u32AoVqeType)
{
    HI_S32 i;
    HI_S32 s32Ret;

    s32Ret = HI_MPI_AO_SetPubAttr(AoDevId, pstAioAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_SetPubAttr(%d) failed with %#x!\n", __FUNCTION__, \
               AoDevId, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_AO_Enable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Enable(%d) failed with %#x!\n", __FUNCTION__, AoDevId, s32Ret);
        return HI_FAILURE;
    }

    for (i = 0; i < s32AoChnCnt; i++)
    {
        s32Ret = HI_MPI_AO_EnableChn(AoDevId, i/(pstAioAttr->enSoundmode + 1));
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_EnableChn(%d) failed with %#x!\n", __FUNCTION__, i, s32Ret);
            return HI_FAILURE;
        }

        if (NULL != pstAoVqeAttr)
        {
            HI_BOOL bAoVqe = HI_TRUE;
            switch (u32AoVqeType)
            {
                case 0:
                    s32Ret = HI_SUCCESS;
                    bAoVqe = HI_FALSE;
                    break;
                case 1:
                    s32Ret = HI_MPI_AO_SetVqeAttr(AoDevId, i, (AO_VQE_CONFIG_S *)pstAoVqeAttr);
                    break;
                default:
                    s32Ret = HI_FAILURE;
                    break;
    }
            if (s32Ret)
            {
                printf("%s: SetAoVqe%d(%d,%d) failed with %#x\n", __FUNCTION__, u32AoVqeType, AoDevId, i, s32Ret);
                return s32Ret;
            }
            if (bAoVqe)
            {
                s32Ret = HI_MPI_AO_EnableVqe(AoDevId, i);
                if (s32Ret)
                {
                    printf("%s: HI_MPI_AI_EnableVqe(%d,%d) failed with %#x\n", __FUNCTION__, AoDevId, i, s32Ret);
                    return s32Ret;
                }
            }
        }
    }
    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Ai
******************************************************************************/
HI_S32 unidoli_stop_ai(AUDIO_DEV AiDevId, HI_S32 s32AiChnCnt, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AiChnCnt; i++)
    {
        if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AI_DisableVqe(AiDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }

        s32Ret = HI_MPI_AI_DisableChn(AiDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AI_Disable(AiDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
        return s32Ret;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* function : Stop Ao
******************************************************************************/
HI_S32 unidoli_stop_ao(AUDIO_DEV AoDevId, HI_S32 s32AoChnCnt, HI_BOOL bVqeEn)
{
    HI_S32 i;
    HI_S32 s32Ret;

    for (i = 0; i < s32AoChnCnt; i++)
    {

        if (HI_TRUE == bVqeEn)
        {
            s32Ret = HI_MPI_AO_DisableVqe(AoDevId, i);
            if (HI_SUCCESS != s32Ret)
            {
                printf("[Func]:%s [Line]:%d [Info]:%s\n", __FUNCTION__, __LINE__, "failed");
                return s32Ret;
            }
        }
        s32Ret = HI_MPI_AO_DisableChn(AoDevId, i);
        if (HI_SUCCESS != s32Ret)
        {
            printf("%s: HI_MPI_AO_DisableChn failed with %#x!\n", __FUNCTION__, s32Ret);
            return s32Ret;
        }
    }

    s32Ret = HI_MPI_AO_Disable(AoDevId);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: HI_MPI_AO_Disable failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    return HI_SUCCESS;
}

/*
 * 获取音频数据，格式为PCM
 */
int unidoli_aduio_read(AUDIO_DEV AiDev, AI_CHN AiChn, void *buf, uint32_t max_len, uint32_t *pts, uint32_t *id)
{
    HI_S32 s32Ret;
    AI_CHN_PARAM_S stAiChnPara;

    s32Ret = HI_MPI_AI_GetChnParam(AiDev, AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret) {
        printf("%s: Get ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    stAiChnPara.u32UsrFrmDepth = 30;

    s32Ret = HI_MPI_AI_SetChnParam(AiDev, AiChn, &stAiChnPara);
    if (HI_SUCCESS != s32Ret) {
        printf("%s: set ai chn param failed\n", __FUNCTION__);
        return -1;
    }

    fd_set read_fds;
    FD_ZERO(&read_fds);
    HI_S32 AiFd = HI_MPI_AI_GetFd(AiDev, AiChn);
    FD_SET(AiFd, &read_fds);

    struct timeval TimeoutVal;
    TimeoutVal.tv_sec = 1;
    TimeoutVal.tv_usec = 0;
    FD_ZERO(&read_fds);
    FD_SET(AiFd, &read_fds);

    s32Ret = select(AiFd + 1, &read_fds, NULL, NULL, &TimeoutVal);
    if (s32Ret < 0) {
        return -1;
    } else if (0 == s32Ret) {
        printf("%s: get ai frame select time out.\n", __FUNCTION__);
        return -1;
    }

    int buf_len = 0;
    if (FD_ISSET(AiFd, &read_fds)) {
        /* get frame from ai chn */
        AUDIO_FRAME_S stFrame;
        AEC_FRAME_S   stAecFrm;
        memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
        s32Ret = HI_MPI_AI_GetFrame(AiDev, AiChn, &stFrame, &stAecFrm, HI_FALSE);
        if (HI_SUCCESS != s32Ret ) {
            printf("%s: get ai frame failed.\n", __FUNCTION__);
            return -1;
        }

        //printf("id:%d len:%d time:%lld\r\n", stFrame.u32Seq, stFrame.u32Len, stFrame.u64TimeStamp/1000);
        buf_len = stFrame.u32Len;
        *pts = stFrame.u64TimeStamp/1000;
        *id = stFrame.u32Seq;
        if(buf_len < max_len) {
            memcpy(buf, stFrame.pVirAddr[0], buf_len);
        } else {
            printf("%s: max len failed.\n", __FUNCTION__);
            return -1;
        }

        /* finally you must release the stream */
        s32Ret = HI_MPI_AI_ReleaseFrame(AiDev, AiChn, &stFrame, &stAecFrm);
        if (HI_SUCCESS != s32Ret ) {
            printf("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n", __FUNCTION__, AiDev, AiChn, s32Ret);
            return -1;
        }
    }

    return buf_len;
}

int unidoli_aduio_send(AUDIO_DEV AoDev, AO_CHN AoChn, void *buf, uint32_t len, uint32_t ms_timeout)
{
    AUDIO_FRAME_S audio_frame_s;
    audio_frame_s.enBitwidth = AUDIO_BIT_WIDTH_16;
    audio_frame_s.enSoundmode = AUDIO_SOUND_MODE_MONO;
    audio_frame_s.pVirAddr[0] = buf;
    audio_frame_s.u32Len = len;
    return HI_MPI_AO_SendFrame(AoDev, AoChn, &audio_frame_s, 1000);
}

/******************************************************************************
* function : Ai -> Ao(with fade in/out and volume adjust)
******************************************************************************/
HI_S32 unidoli_audio_service_config(AUDIO_DEV AiDev, AUDIO_DEV AoDev, AUDIO_SAMPLE_RATE_E sample_rast, int num_perfrm)
{
    HI_S32 s32Ret = HI_FAILURE;
    AIO_ATTR_S stAioAttr;

    stAioAttr.enSamplerate   = sample_rast;
    stAioAttr.enBitwidth     = AUDIO_BIT_WIDTH_16;
    stAioAttr.enWorkmode     = AIO_MODE_I2S_MASTER;
    stAioAttr.enSoundmode    = AUDIO_SOUND_MODE_MONO;
    stAioAttr.u32EXFlag      = 0;
    stAioAttr.u32FrmNum      = 30;
    stAioAttr.u32PtNumPerFrm = num_perfrm;
    stAioAttr.u32ChnCnt      = 1;
    stAioAttr.u32ClkSel      = 0;
    
    /* config audio codec */
    /*** INNER AUDIO CODEC ***/
    s32Ret = unidoli_codec_config_audio(stAioAttr.enSamplerate);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s:SAMPLE_INNER_CODEC_CfgAudio failed\n", __FUNCTION__);
        return s32Ret;
    }
    
    /* enable AI channle */    
    HI_S32 s32AiChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = unidoli_start_ai(AiDev, s32AiChnCnt, &stAioAttr, NULL, 0);
    if (s32Ret != HI_SUCCESS)
    {
        audio_debug(s32Ret);
        return s32Ret;
    }

    HI_S32 s32AoChnCnt = stAioAttr.u32ChnCnt;
    s32Ret = unidoli_start_ao(AoDev, s32AoChnCnt, &stAioAttr, NULL , 0);
    if (s32Ret != HI_SUCCESS)
    {
        audio_debug(s32Ret);
        return s32Ret;
    }

    return s32Ret;
}
