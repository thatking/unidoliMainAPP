/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "iot.h"

#define EXAMPLE_TRACE(fmt, args...)  \
    do { \
        printf("%s|%03d :: ", __func__, __LINE__); \
        printf(fmt, ##args); \
        printf("%s", "\r\n"); \
    } while(0)


char loop_mqtt = 1;

void mqttStop(int signo)
{
	loop_mqtt = 0;
}
void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECVEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

static void controlrrived(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

	char *payload = (char*)ptopic_info->payload;
	int payload_len   = (int)ptopic_info->payload_len;
	panMsgDeal(payload,payload_len);
}

static void updateArrived(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	int ret = -1;
	char down_cmd[64];
	iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

	char *payload   = (char*)ptopic_info->payload;
	int payload_len = (int)ptopic_info->payload_len;

	EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",payload_len,payload,payload_len);
	payload[payload_len] = '\0';
	cJSON* json = cJSON_Parse(payload);
	if(json == NULL)
		goto RET;
	ret = 1;
	cJSON* cmd = cJSON_GetObjectItem(json,"cmd");
	cJSON* url = cJSON_GetObjectItem(json,"url");
	if((cmd == NULL) || (url == NULL))
		goto RET;
	ret = 2;
	PRINTF("cmd:%s",cmd->valuestring);
	PRINTF("url:%s",url->valuestring);

	sprintf(down_cmd,"wget %s -O update.sh",url->valuestring);
	system(down_cmd);
	sleep(2);
	system("chmod +x update.sh");
	system("./update.sh");
	ret = 3;
RET:
	printf("function : %s, line : %d, error code : %d",__FUNCTION__, __LINE__,ret);
}

/*******************************************************************************
* 名称:ConfigArrived
* 功能:处理mqtt接收到的配置信息
* 形参:
* 返回:
* 说明:
*******************************************************************************/
static void configArrived(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
	int ret = -1;
	char buf[64];
	unsigned char flag = 0x00;
	int fd;
	char readBuffer[MSG_LEN_MAX];
	char *p = readBuffer;
	int readNumOfByte = 0;
	int tryReadNumOfByte = 12;
	int sumRead = 0;
	iotx_mqtt_topic_info_pt ptopic_info = (iotx_mqtt_topic_info_pt) msg->msg;

	char *payload   = (char *)ptopic_info->payload;
	int payload_len = (int)ptopic_info->payload_len;

	EXAMPLE_TRACE("Payload: '%.*s' (Length: %d)",payload_len,payload,payload_len);
	payload[payload_len] = '\0';
	cJSON* json = cJSON_Parse(payload);
	if(json == NULL)
		goto RET;
	ret = 1;
	cJSON* resolution = cJSON_GetObjectItem(json,"resolution");
	if(resolution != NULL)
		flag |= 0x80;
	cJSON* frameRate = cJSON_GetObjectItem(json,"frameRate");
	if(frameRate != NULL)
		flag |= 0x40;
	cJSON* bitRate = cJSON_GetObjectItem(json,"bitRate");
	if(bitRate != NULL)
		flag |= 0x20;
	cJSON* urlFrom = cJSON_GetObjectItem(json,"urlFrom");
	if(urlFrom != NULL)
		flag |= 0x10;
	cJSON* rtmpUrl = cJSON_GetObjectItem(json,"rtmpUrl");
	if(rtmpUrl != NULL)
		flag |= 0x08;
	if(!(flag))
		goto RET;
	ret = 2;

	/* 打开源文件 */
	if((fd=open("./unidoli.conf",O_RDWR))==-1)
	{
		printf("open error");
		exit(1);
	}
	do
	{
		p += readNumOfByte;
		readNumOfByte = read(fd,p,tryReadNumOfByte);
		if(readNumOfByte == -1)
		{
			printf("read error");
		}
		sumRead += readNumOfByte;
	}
	while(readNumOfByte == tryReadNumOfByte);
	readBuffer[sumRead] = '\0';
	PRINTF("read : \n%s",readBuffer);
	cJSON* jsonFiel = cJSON_Parse(readBuffer);
	if(json == NULL)
		goto RET;
	ret = 3;
	if(flag & 0x80)
	{
		cJSON_ReplaceItemInObject(jsonFiel,"resolution",cJSON_CreateNumber(resolution->valueint));
	}
	if(flag & 0x40)
	{
		cJSON_ReplaceItemInObject(jsonFiel,"frameRate",cJSON_CreateNumber(frameRate->valueint));
	}
	if(flag & 0x20)
	{
		cJSON_ReplaceItemInObject(jsonFiel,"bitRate",cJSON_CreateNumber(bitRate->valueint));
	}
	if(flag & 0x10)
	{
		cJSON_ReplaceItemInObject(jsonFiel,"urlFrom",cJSON_CreateString(urlFrom->valuestring));
	}
	if(flag & 0x08)
	{
		cJSON_ReplaceItemInObject(jsonFiel,"rtmpUrl",cJSON_CreateString(rtmpUrl->valuestring));
	}
	p = cJSON_Print(jsonFiel);
	PRINTF("jsonFiel : %s",p);

	lseek(fd,0,SEEK_SET);
	int writeNumOfFile = write(fd,p,strlen(p));
	truncate("./unidoli.conf", writeNumOfFile);
	raise(SIGCHLD);

	ret = 30;
RET:
	close(fd);
	payload_len = sprintf(buf,"{\"receive code\":%d}",ret);
}

int mqtt_client(void)
{
    int rc = 0, msg_len;
    void *pclient;
    iotx_conn_info_pt pconn_info;
    iotx_mqtt_param_t mqtt_params;
    iotx_mqtt_topic_info_t topic_msg;
    char msg_pub[128];
    char *msg_buf = NULL, *msg_readbuf = NULL;
    char device_name[24];
    char device_secret[64];
    char topic_update[64],topic_config[64],topic_control[64],topic_state[64];

    if (NULL == (msg_buf = (char *)HAL_Malloc(MSG_LEN_MAX))) {
        EXAMPLE_TRACE("not enough memory");
        rc = -1;
        goto do_exit;
    }

    if (NULL == (msg_readbuf = (char *)HAL_Malloc(MSG_LEN_MAX))) {
        EXAMPLE_TRACE("not enough memory");
        rc = -1;
        goto do_exit;
    }

    int fd_dv;
    int rdOrWrNum;
    char dvcSecretBuff[36];
    getClientID(device_name);
	if((fd_dv=open("./dvcSecret",O_RDWR | O_CREAT)) == -1)
	{
		PRINTF("open dvcSecret error");
		exit(1);
	}
	rdOrWrNum = read(fd_dv,dvcSecretBuff,36);
	dvcSecretBuff[rdOrWrNum] = '\0';
	if(rdOrWrNum == -1)
	{
		PRINTF("read dvcSecret error");
	}
	if(rdOrWrNum != 32)
	{
		getSecret(device_secret,device_name);
		lseek(fd_dv,0,SEEK_SET);
		rdOrWrNum = write(fd_dv,device_secret,strlen(device_secret));
		truncate("./dvcSecret", rdOrWrNum);
		close(fd_dv);
	}
	else
	{
		strcpy(device_secret,dvcSecretBuff);
	}
	if(snprintf(topic_update, sizeof(topic_update),"/%s/%s/update", PRODUCT_KEY,device_name) < 0)
	{
		PRINTF("topic too long!");
		return -1;
	}
	if(snprintf(topic_config, sizeof(topic_config),"/%s/%s/config", PRODUCT_KEY,device_name) < 0)
	{
		PRINTF("topic too long!");
		return -1;
	}
	if(snprintf(topic_control, sizeof(topic_control),"/%s/%s/control", PRODUCT_KEY,device_name) < 0)
	{
		PRINTF("topic too long!");
		return -1;
	}
	if(snprintf(topic_state, sizeof(topic_state),"/%s/%s/state", PRODUCT_KEY,device_name) < 0)
	{
			PRINTF("topic too long!");
			return -1;
	}
    /* Device AUTH */
    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, device_name, device_secret, (void **)&pconn_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        rc = -1;
        goto do_exit;
    }

    /* Initialize MQTT parameter */
    memset(&mqtt_params, 0x0, sizeof(mqtt_params));

    mqtt_params.port = pconn_info->port;
    mqtt_params.host = pconn_info->host_name;
    mqtt_params.client_id = pconn_info->client_id;
    mqtt_params.username = pconn_info->username;
    mqtt_params.password = pconn_info->password;
    mqtt_params.pub_key = pconn_info->pub_key;

    mqtt_params.request_timeout_ms = 2000;
    mqtt_params.clean_session = 0;
    mqtt_params.keepalive_interval_ms = 60000;
    mqtt_params.pread_buf = msg_readbuf;
    mqtt_params.read_buf_size = MSG_LEN_MAX;
    mqtt_params.pwrite_buf = msg_buf;
    mqtt_params.write_buf_size = MSG_LEN_MAX;

    mqtt_params.handle_event.h_fp = event_handle;
    mqtt_params.handle_event.pcontext = NULL;


    /* Construct a MQTT client with specify parameter */
    pclient = IOT_MQTT_Construct(&mqtt_params);
    if (NULL == pclient) {
        EXAMPLE_TRACE("MQTT construct failed");
        rc = -1;
        goto do_exit;
    }

    /* Subscribe the specific topic */
    rc = IOT_MQTT_Subscribe(pclient, topic_config, IOTX_MQTT_QOS1, configArrived, NULL);
    if (rc < 0) {
        IOT_MQTT_Destroy(&pclient);
        EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
        rc = -1;
        goto do_exit;
    }
    /* Subscribe the specific topic */
	rc = IOT_MQTT_Subscribe(pclient, topic_update, IOTX_MQTT_QOS1, updateArrived, NULL);
	if (rc < 0) {
		IOT_MQTT_Destroy(&pclient);
		EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
		rc = -1;
		goto do_exit;
	}
	/* Subscribe the specific topic */
	rc = IOT_MQTT_Subscribe(pclient, topic_control, IOTX_MQTT_QOS1, controlrrived, NULL);
	if (rc < 0) {
		IOT_MQTT_Destroy(&pclient);
		EXAMPLE_TRACE("IOT_MQTT_Subscribe() failed, rc = %d", rc);
		rc = -1;
		goto do_exit;
	}

    HAL_SleepMs(1000);

    /* open the version record file*/
    int fd_ver;
    char rd_buf[64];
    char *p = rd_buf;
    int rdNum  = 8;
    int rRdNum = 0;
    int rdSum  = 0;

	if((fd_ver=open("./unidoli.ver",O_RDONLY)) == -1)
	{
		PRINTF("open unidoli.ver error \n");
		exit(1);
	}
	do
	{
		p += rRdNum;
		rRdNum = read(fd_ver,p,rdNum);
		if(rRdNum == -1)
		{
			PRINTF("read unidoli.ver error\n");
		}
		rdSum += rRdNum;
	}
	while(rRdNum == rdNum);
	rd_buf[rdSum] = '\0';
	close(fd_ver);

	PRINTF("unidoli.ver : %s",rd_buf);

    /* Initialize topic information */
    memset(&topic_msg, 0x0, sizeof(iotx_mqtt_topic_info_t));
    snprintf(msg_pub, sizeof(msg_pub), "{\"version\":\"%s\"}", rd_buf);
    topic_msg.qos = IOTX_MQTT_QOS1;
    topic_msg.retain = 0;
    topic_msg.dup = 0;
    topic_msg.payload = (void *)msg_pub;
    topic_msg.payload_len = strlen(msg_pub);

    rc = IOT_MQTT_Publish(pclient, topic_state, &topic_msg);
    EXAMPLE_TRACE("rc = IOT_MQTT_Publish() = %d", rc);

    do
    {
    	if(0)    //turn of loop publish
    	{
			/* Generate topic message */
			msg_len = snprintf(msg_pub, sizeof(msg_pub), "{\"attr_name\":\"temperature\", \"attr_value\":\" \"}");
			if (msg_len < 0) {
				EXAMPLE_TRACE("Error occur! Exit program");
				rc = -1;
				break;
			}

			topic_msg.payload = (void *)msg_pub;
			topic_msg.payload_len = msg_len;

			rc = IOT_MQTT_Publish(pclient, topic_state, &topic_msg);
			if (rc < 0) {
				EXAMPLE_TRACE("error occur when publish");
				rc = -1;
				break;
			}
			#ifdef MQTT_ID2_CRYPTO
				EXAMPLE_TRACE("packet-id=%u, publish topic msg='0x%02x%02x%02x%02x'...",
							  (uint32_t)rc,
							  msg_pub[0], msg_pub[1], msg_pub[2], msg_pub[3]
							 );
			#else
				EXAMPLE_TRACE("packet-id=%u, publish topic msg=%s", (uint32_t)rc, msg_pub);
			#endif
    	}
        /* handle the MQTT packet received from TCP or SSL connection */
        IOT_MQTT_Yield(pclient, 200);

    } while (loop_mqtt);
    loop_mqtt = 1;

    IOT_MQTT_Unsubscribe(pclient, topic_config);
    IOT_MQTT_Unsubscribe(pclient, topic_update);
    IOT_MQTT_Unsubscribe(pclient, topic_control);
    HAL_SleepMs(200);
    IOT_MQTT_Destroy(&pclient);
do_exit:
    if (NULL != msg_buf) {
        HAL_Free(msg_buf);
    }

    if (NULL != msg_readbuf) {
        HAL_Free(msg_readbuf);
    }
    return rc;
}

/*******************************************************************************
* 名称: MqttPthread
* 功能: mqtt进程
* 形参:
* 返回:
* 说明: 无
*******************************************************************************/
void *MqttPthread(void *arg)
{
	signal(SIGUSR2, mqttStop);
    mqtt_client();
    EXAMPLE_TRACE("mqtt terminated success!");

	return EXIT_SUCCESS;
}
