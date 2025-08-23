/**
  ******************************************************************************
  * @file   bsp_esp8266.h
  * @brief  wifi模组ESP-12F的驱动头文件
  * 
  ******************************************************************************
  */
#ifndef __BSP_ESP8266_H__
#define __BSP_ESP8266_H__
//
#include "main.h"
//
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
//
#if defined (__CC_ARM)
#pragma anon_unions
#endif
//
#define User_ESP8266_SSID     "SUMMER"          					//wifi名
#define User_ESP8266_PWD    "00000000"     	 					//wifi密码
//MQTT测试
#define User_ESP8266_client_id    "STM32U575"   				//MQTTclientID 用于标志client身份  最长256字节
#define User_ESP8266_username     ""									//用于登录 MQTT 服务器 的 username, 最长 64 字节	
#define User_ESP8266_password			""        				//用于登录 MQTT 服务器 的 password, 最长 64 字节
#define User_ESP8266_MQTTServer_IP     "10.172.153.56"
//#define User_ESP8266_MQTTServer_IP     "192.168.137.173"	//MQTT本地服务器IP，EMQX本地服务器的IP 
#define User_ESP8266_MQTTServer_PORT   1883     					//服务器端口号

#define User_ESP8266_MQTTServer_Topic "topic"

//ESP8266模式选择
typedef enum{
		STA,
		AP,
		STA_AP  
}ENUM_Net_ModeTypeDef;
//
#define ESP8266_USART(fmt, ...)  USART_printf(&huart5, fmt, ##__VA_ARGS__)    
//
#define RX_BUF_MAX_LEN 1024       //最大字节数
//
extern struct STRUCT_USART_Fram   //数据帧结构体
{
		char Data_RX_BUF[RX_BUF_MAX_LEN];
		union 
		{
			volatile uint16_t InfAll;
			struct 
			{
				volatile uint16_t FramLength       :15;	// 14:0 
				volatile uint16_t FramFinishFlag   :1;	// 15 
			}InfBit;
		}; 	
}ESP8266_Fram_Record_Struct;
//初始化和TCP功能函数
void ESP8266_Init(UART_HandleTypeDef *huart, uint8_t *DataBuf,uint32_t bound);
void ESP8266_AT_Test(void);
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,uint32_t time);
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode);
bool ESP8266_JoinAP(char * pSSID, char * pPassWord);
void USART_printf(UART_HandleTypeDef * USARTx, char * Data, ...);
//MQTT功能函数
bool ESP8266_MQTTUSERCFG(char * pClient_Id, char * pUserName,char * PassWord);
bool ESP8266_MQTTCONN(char * Ip, int Num);
bool ESP8266_MQTTSUB(char * Topic);
bool ESP8266_MQTTPUB(char * Topic,char *temp);
bool ESP8266_MQTTPUBRAW( char * Topic,char *len);
bool ESP8266_MQTTCLEAN(void);
bool MQTT_SendString(char * pTopic,char *temp2);
void ESP8266_STA_MQTTClient(void);

void ESP8266_Json_Parse(char * pData);

#endif