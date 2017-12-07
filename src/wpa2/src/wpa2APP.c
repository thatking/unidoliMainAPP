/******************************************************************************
  UNIDOLI_HISI_RTMP
  Unidoli_Main.c
 ******************************************************************************
    Modification:  2017-6 Created
******************************************************************************/
#include "wpa2APP.h"

/*******************************************************************************
* 名称: wave_player_Enable
* 功能: 使能语音播报
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void wave_player_Enable(void)
{
	int ft = open("/sys/class/gpio/gpio40/value", O_RDWR | O_NOCTTY);
	if(ft > 0)
	{
		write(ft,"1",1);
		close(ft);
	}
	else
	{
		PRINTF("gpio open error");
	}
}

/*******************************************************************************
* 名称: wave_player_Disable
* 功能: 失能语音播报
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void wave_player_Disable(void)
{
	int ft = open("/sys/class/gpio/gpio40/value", O_RDWR | O_NOCTTY);
	if(ft > 0)
	{
		write(ft,"0",1);
		close(ft);
	}
	else
	{
		printf("gpio open error\n");
	}
}


//static int gs_unidoli_audio_msg_status = 0;
//static pthread_t gs_unidoli_wpa_pthread_id;
/******************************************************************************
* function :  unidoli_wave_player_service
* 
******************************************************************************/
HI_S32 unidoli_wave_player_service(char *wave_file)
{
	HI_S32 s32Ret = HI_SUCCESS;
	VB_CONF_S stVbConf;

	wave_player_Enable();
	usleep(1000);
	memset(&stVbConf, 0, sizeof(VB_CONF_S));
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
        printf("%s: system init failed with %d!\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

	/* create ai service */
	unidoli_audio_service_config(SAMPLE_AUDIO_AI_DEV, SAMPLE_AUDIO_AO_DEV, AUDIO_SAMPLE_RATE_8000, SAMPLE_AUDIO_PTNUMPERFRM*2);

    /* monitor pthread running... */
    uint8_t buf[4096];
    FILE *fp = fopen(wave_file, "r");
    fseek(fp, 54, SEEK_SET);
	do {
		int r_len = fread(buf, 1, 1024, fp);
		if(r_len < 1) {
			break;
		}
		unidoli_aduio_send(SAMPLE_AUDIO_AO_DEV, 0, buf, r_len, 1000);
	}while(1);
	fclose(fp);

	/* wait player end */
	sleep(4);
	
    s32Ret |= unidoli_stop_ai(SAMPLE_AUDIO_AI_DEV, 1, HI_FALSE);
    s32Ret |= unidoli_stop_ao(SAMPLE_AUDIO_AO_DEV, 1, HI_FALSE);

    SAMPLE_COMM_SYS_Exit();
    wave_player_Disable();

    return s32Ret;
}

/******************************************************************************
* function :  unidoli_audio_config_service
* 
******************************************************************************/
int unidoli_audio_config_service(char *ssid, char *pwd)
{
	HI_S32 s32Ret = HI_SUCCESS;
	int return_val = 0;
	VB_CONF_S stVbConf;
	audio_transfer_init(49);

	memset(&stVbConf, 0, sizeof(VB_CONF_S));
	s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
    if (HI_SUCCESS != s32Ret)
    {
    	PRINTF("system init failed with %d!\n",s32Ret);
        return HI_FAILURE;
    }

	/* create ai service */
	unidoli_audio_service_config(SAMPLE_AUDIO_AI_DEV, SAMPLE_AUDIO_AO_DEV, AUDIO_SAMPLE_RATE_44100, 98);

    /* monitor pthread running... */
    uint8_t buf[512];
    uint8_t dat[512];
    uint32_t pts;
    uint32_t frame_id;
    int len_d;
    int ssid_flag = 0, pwd_flag = 0;

	while(1) {
		int ret = unidoli_aduio_read(SAMPLE_AUDIO_AI_DEV, 0, buf, 4096, &pts, &frame_id);
		if(ret < 0) {
			return_val = -1;
			break;
		}
		/* FFT Analysis */
		int len_1 = audio_transfer_analysis(44100, (short *)buf, 49, dat);
		int len_2 = audio_transfer_analysis(44100, ((short *)buf + 49), 49, dat);
		if(len_1 > 0) {
			len_d = len_1;
		} else if(len_2 > 0) {
			len_d = len_2;
		} else {
			continue;
		}

		dat[len_d] = '\0';

		if(dat[0] == 'S') {
			strcpy(ssid, (char *)&dat[1]);
			ssid_flag = 1;
		} else if(dat[0] == 'P') {
			strcpy(pwd, (char *)&dat[1]);
			pwd_flag = 1;
		} else {
			PRINTF("r data failed.");
		}

		if(ssid_flag && pwd_flag) {
			break;
		}
		//printf("read id:%d len:%d pts:%d\n", frame_id, r_len, pts);
	}

	audio_transfer_exit();
    s32Ret |= unidoli_stop_ai(SAMPLE_AUDIO_AI_DEV, 1, HI_FALSE);
    s32Ret |= unidoli_stop_ao(SAMPLE_AUDIO_AO_DEV, 1, HI_FALSE);
    SAMPLE_COMM_SYS_Exit();

    return return_val;
}

int set_wifi_config(char *ssid, char *passwd)
{
	size_t len, status;
	char buf[4096];
	/* 设置WIFI的SSID */
	printf(GREEN_FONT, "[DEBUG]");
	printf("the ssid is [%s].\n", ssid);
	struct wpa_ctrl *ctrl = wpa_ctrl_open("/etc/wpa_supplicant/wlan0");
	if(ctrl == NULL) {
		printf(RED_FONT, "[ERROR]");
		printf("enten wpa_ctrl_open failed.\n");
		return -1;
	}
	len=4095;
	char set_network[100];
	int set_len = sprintf(set_network, "SET_NETWORK 0 ssid \"%s\"", ssid);
	status = wpa_ctrl_request(ctrl, set_network, set_len, buf, &len, NULL);
	if(status != 0) {
		printf(RED_FONT, "[ERROR]");
		printf("wpa request failed.\n");
		wpa_ctrl_close(ctrl);
		return -1;
	}
	buf[len] = '\0';
	printf(GREEN_FONT, "[DEBUG]");
	printf("set ssid msg : %s.\n", buf);
	/* 设置WIFI的密码 */
	if(*passwd == '\0'){
		printf(YELLOW_FONT, "[WARN]");
		printf("no set passwd.\n");
		len=4095;
		char set_pwd[100];
		int set_len = sprintf(set_pwd, "SET_NETWORK 0 key_mgmt NONE");
		status = wpa_ctrl_request(ctrl, set_pwd, set_len, buf, &len, NULL);
		if(status != 0) {
			printf(RED_FONT, "[ERROR]");
			printf("wpa request failed.\n");
			wpa_ctrl_close(ctrl);
			return -1;
		}
		buf[len] = '\0';
		printf(GREEN_FONT, "[DEBUG]");
		printf("set key_mgmt msg : %s.\n", buf);
	}else{
		printf(GREEN_FONT, "[DEBUG]");
		printf("the passwd is [%s].\n", passwd);
		len=4095;
		char set_pwd[100];
		int set_len = sprintf(set_pwd, "SET_NETWORK 0 key_mgmt WPA-PSK");
		status = wpa_ctrl_request(ctrl, set_pwd, set_len, buf, &len, NULL);
		if(status != 0) {
			printf(RED_FONT, "[ERROR]");
			printf("wpa request failed.\n");
			wpa_ctrl_close(ctrl);
			return -1;
		}
		buf[len] = '\0';
		printf(GREEN_FONT, "[DEBUG]");
		printf("set key_mgmt msg : %s.\n", buf);
		len=4095;
		set_len = sprintf(set_pwd, "SET_NETWORK 0 psk \"%s\"", passwd);
		status = wpa_ctrl_request(ctrl, set_pwd, set_len, buf, &len, NULL);
		if(status != 0) {
			printf(RED_FONT, "[ERROR]");
			printf("wpa request failed.\n");
			wpa_ctrl_close(ctrl);
			return -1;
		}
		buf[len] = '\0';
		printf(GREEN_FONT, "[DEBUG]");
		printf("set psk msg : %s.\n", buf);
	}
	/* 保存配置 */
	len=4095;
	char *save_net = "SAVE_CONFIG";
	status = wpa_ctrl_request(ctrl, save_net, strlen(save_net), buf, &len, NULL);
	if(status != 0) {
		printf(RED_FONT, "[ERROR]");
		printf("wpa request failed.\n");
		wpa_ctrl_close(ctrl);
		return -1;
	}
	printf(GREEN_FONT, "[DEBUG]");
	printf("save network config.\n");
	/* 启用WIFI配置 */
	len=4095;
	char *enable_net = "ENABLE_NETWORK 0";
	status = wpa_ctrl_request(ctrl, enable_net, strlen(enable_net), buf, &len, NULL);
	if(status != 0) {
		printf(RED_FONT, "[ERROR]");
		printf("wpa request failed.\n");
		wpa_ctrl_close(ctrl);
		return -1;
	}
	printf(GREEN_FONT, "[DEBUG]");
	printf("enable network.\n");
	wpa_ctrl_close(ctrl);
	return 0;
}


/*
 * Split the string
 */
char *strtok_b(char *p, const char dt)
{
	while(*p != dt && *p != '\0') p++;
	if(*p == dt){
		*p = '\0';
		return p + 1;
	}else{
		return NULL;
	}
}

/*
function: get_wifi_message

bssid=d0:ae:ec:96:1a:a4
ssid=101
id=0
mode=station
pairwise_cipher=CCMP
group_cipher=TKIP
key_mgmt=WPA2-PSK
wpa_state=COMPLETED
address=08:57:00:1b:78:62

*/
int get_wifi_message(char *cmd, char *ret_v)
{
	char ret[4096];
	size_t len;
	struct wpa_ctrl *ctrl = wpa_ctrl_open("/etc/wpa_supplicant/wlan0");
	if(ctrl == NULL) {
		printf(RED_FONT, "[ERROR]");
		printf("enten wpa_ctrl_open failed.\n");
		return -1;
	}
	len=4095;
	int status = wpa_ctrl_request(ctrl, "STATUS", 6, ret, &len, NULL);
	if(status != 0){
		printf(RED_FONT, "[ERROR]");
		printf("wpa request failed.\n");
		wpa_ctrl_close(ctrl);
		return -1;
	}
	ret[len] = '\0';
	char *p_next;
	char *p = ret;
	do{
		p_next = strtok_b(p, '\n');
		char *value = strtok_b(p, '=');
		if(strcmp(cmd, p) == 0){
			strcpy(ret_v, value);
			wpa_ctrl_close(ctrl);
			return 0;
		}
	}while((p = p_next) != NULL);
	*ret_v = '\0';
	wpa_ctrl_close(ctrl);
	return -1;
}

void configureWIFI(void)
{
	int ret;
	char wifi_ssid[512];
	char wifi_pwd[512];
	ret = unidoli_wave_player_service("/root/wave/unidoli_001.wav");
	if(ret < 0)
	{
		PRINTF("unidoli_wave_player_service failed.");
	}
	memset(wifi_ssid, 0, 512);
	memset(wifi_pwd, 0, 512);
	ret = unidoli_audio_config_service(wifi_ssid, wifi_pwd);
	if(ret < 0)
	{
		PRINTF("unidoli_audio_config_service failed.");
	}
	PRINTF("CONF:%s %s", wifi_ssid, wifi_pwd);
	ret = set_wifi_config(wifi_ssid, wifi_pwd);
	if(ret < 0)
	{
		PRINTF("set_wifi_config failed.");
	}
	ret = unidoli_wave_player_service("/root/wave/unidoli_002.wav");
	if(ret < 0)
	{
		PRINTF("unidoli_wave_player_service failed.");
	}

	system("reboot");
	while(1)
	{
		PRINTF(YELLOW_FONT, "[WARN]");
		PRINTF("reboot");
		sleep(1);
	}
}

int get_nwtwork_status()
{
	char r_buf[4096];
	uint8_t count = 10;
	do
	{
		get_wifi_message("wpa_state", r_buf);
		printf("wpa_state: %s\n", r_buf);
		count--;
		sleep(1);
	}
	while(strcmp("COMPLETED", r_buf) != 0 && count);
	if(strcmp("COMPLETED", r_buf) != 0)
	{
		/* 密码错误，请重新输入 */
		return -1;
	}
	else
	{
		return 0;
	}
}


void reportWIFIConnetStatus(void)
{
	int ret;
	if(get_nwtwork_status() < 0)
	{
		//net failed!
		PRINTF(YELLOW_FONT, "[WARN]");
		PRINTF("network failed.");
		ret = unidoli_wave_player_service("/root/wave/unidoli_005.wav");
		if(ret < 0)
		{
			PRINTF("unidoli_wave_player_service failed.");
		}
	}
	else
	{
		//net success!
		PRINTF(GREEN_FONT, "[DEBUG]");
		PRINTF("network success.");
		ret = unidoli_wave_player_service("/root/wave/unidoli_004.wav");
		if(ret < 0)
		{
			PRINTF("unidoli_wave_player_service failed.");
		}
	}
}

void delspace(char * p)
{
	int i,j=0;
	for ( i = 0;p[i]!='\0';i ++ )
	{
		if(p[i] != ' ' && p[i] != '\n' && p[i] != '\t' && p[i] != '\"')
			p[j++] = p[i];
	}
	p[j] = '\0';
}

int get_conf_value(char *file_path, char *key_name, char *value)
{
    FILE *fp = NULL;
	char *line = NULL, *substr = NULL;
	size_t len = 0, tlen = 0,n = 0;

    if(file_path == NULL || key_name == NULL || value == NULL)
    {
        printf("paramer is invaild!\n");
        return -1;
    }
	fp = fopen(file_path, "r");
	if (fp == NULL)
    {
        printf("open config file is error!\n");
        return -1;
    }

	while ((len = getline(&line, &n, fp)) != -1)
    {
		delspace(line);
		len = strlen(line);
        substr = strstr(line, key_name);
        if(substr == NULL)
        {
            continue;
        }
        else
        {
            tlen = strlen(key_name);
            if(line[tlen] == '=')
            {
                strncpy(value, &line[tlen+1], len-tlen-1);
                break;
            }
            else
            {
            	PRINTF("config file format is invaild tlen is %d len is %d", tlen, len);
                fclose(fp);
                return -2;
            }
        }
	}
    if(substr == NULL)
    {
    	PRINTF("key: %s is not in config file!", key_name);
        fclose(fp);
        return -1;
    }

	free(line);
    fclose(fp);
    return 0;
}

char getWIFIConfigureStasus(void)
{
    char getva[25] = {0};
    char ssid[] = "ssid";
    char passwk[] = "psk";
    char ret = get_conf_value(FILEPATH, ssid, getva);
    if(ret == 0)
    {
    	if(strcmp(getva,"unidoli") == 0)
    	{
    		PRINTF("ssid : %s",getva);

    		ret = get_conf_value(FILEPATH, passwk, getva);
			if(ret == 0)
			{
				if(strcmp(getva,"unidoli888") == 0)
				{
					PRINTF("passwk : %s",getva);
					return ret;
				}
			}
    	}
    }
    return -1;
}

char getKeyStatus(void)
{
	int len;
	tlv_alys tlv;
	int fd;
	char buf[16];

	YDLModbusInit(MODBUS_ADDR);
	memset(buf,0,16);
	fd = SerialInit(SERIAL_PATH,9600,1);
	if(fd > 0)
	{
		len = YDLModbusPack((u8*)buf,0x01,CMD_READ_KEY,0);
		tcflush(fd,TCIOFLUSH);     //刷清输入、输出队列
		modbusSend(fd,buf,len);
		len = SerialReadUnblock(fd,buf,REV_MAX_LEN,1000);
		if(len > 0)
		{
			if(YDLModbusAnalysis((u8*)buf,len,&tlv)!=0)
			{
				if(tlv.memb[0].tag == 0x09)
				{
					SerialClose(fd);
					return *(tlv.memb[0].val);
				}
			}
		}
		SerialClose(fd);
	}
	return 0;
}
