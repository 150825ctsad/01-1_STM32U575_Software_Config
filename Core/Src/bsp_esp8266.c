/**
  ******************************************************************************
  * @file   bsp_esp8266.c
  * @brief  wifi模组ESP-12F的驱动程序
  * 
  ******************************************************************************
  */
#include "bsp_esp8266.h"
#include <stdarg.h>
#include "usart.h"

#include "string.h"
#include "stdlib.h"
#include <string.h>

#include "cJSON.h"
#include "cmsis_os2.h"

struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct = { 0 };  //定义了一个数据帧结构体
extern osSemaphoreId_t sem_TakePhoto; 
//初始化波特率
void ESP8266_Init(UART_HandleTypeDef *huart, uint8_t *DataBuf,uint32_t bound)
{
	//设置波特率
  huart->Init.BaudRate = bound;
	//初始化配置
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    Error_Handler();
  }	
	//开启串口接收与空闲中断
	HAL_UART_Receive_IT(huart,(uint8_t *)DataBuf, 1);	//开启接收中断	
	__HAL_UART_CLEAR_IDLEFLAG(huart);			//清除空闲中断标志							
	__HAL_UART_ENABLE_IT(huart,UART_IT_IDLE);	//开启空闲中断	
}
//对ESP8266模块发送AT指令
// cmd 待发送的指令
// ack1,ack2;期待的响应，为NULL表不需响应，两者为或逻辑关系
// time 等待响应时间
//返回1发送成功， 0失败
bool ESP8266_Send_AT_Cmd(char *cmd,char *ack1,char *ack2,uint32_t time)
{ 
    ESP8266_Fram_Record_Struct .InfBit .FramLength = 0;		//重新接收新的数据包
    ESP8266_USART("%s\r\n", cmd);
    if(ack1==0&&ack2==0)		//不需要接收数据
    {
    return true;
    }
    HAL_Delay(time);	//延时
    ESP8266_Fram_Record_Struct.Data_RX_BUF[ESP8266_Fram_Record_Struct.InfBit.FramLength ] = '\0';
		//
    printf("%s",ESP8266_Fram_Record_Struct .Data_RX_BUF);
		//
    if((ack1!=0) && (ack2!=0))
    {
        return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack1 ) || 
                         ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack2 ) );
    }
    else if( ack1 != 0 )  //strstr(s1,s2);检测s2是否为s1的一部分，是返回该位置，否则返回false，它强制转换为bool类型了
        return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack1 ) );
    else
        return ( ( bool ) strstr ( ESP8266_Fram_Record_Struct .Data_RX_BUF, ack2 ) );
}
//发送恢复出厂默认设置指令将模块恢复成出厂设置
void ESP8266_AT_Test(void)
{
    char count=0;
    HAL_Delay(100); 
    while(count < 10)
    {
        if(ESP8266_Send_AT_Cmd("AT+RESTORE","OK",NULL,1000)) 
        {
            printf("OK\r\n");
            return;
        }
        ++ count;
    }
}
//选择ESP8266的工作模式
// enumMode 模式类型
//成功返回true，失败返回false
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
    switch ( enumMode )
    {
        case STA:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 

        case AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 

        case STA_AP:
            return ESP8266_Send_AT_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 

        default:
          return false;
    }       
}
//ESP8266连接外部的WIFI
//pSSID WiFi帐号
//pPassWord WiFi密码
//设置成功返回true 反之false
bool ESP8266_JoinAP(char * pSSID, char * pPassWord)
{
    char cCmd [120];
		//
    sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 5000 );
}
//
static char *itoa(int value, char *string, int radix )    //把一整数转换为字符串。
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;

    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */
//
void USART_printf(UART_HandleTypeDef * USARTx, char * Data, ... )
{
    const char *s;
    int d;   
    char buf[16];
	  char singleBuff[1];
		//
    va_list ap;
    va_start(ap, Data);
		//
    while ( * Data != 0 )     // 判断数据是否到达结束符
    {                                         
        if ( * Data == 0x5c )  //'\'
        {                                     
            switch ( *++Data )
            {
                case 'r':                                     //回车符
								singleBuff[0] = 0x0d;    
								__HAL_UART_CLEAR_FLAG(USARTx, UART_CLEAR_TCF);/* Clear the TC flag in the ICR register */
								HAL_UART_Transmit(USARTx, (void*)singleBuff, 1, 1);//阻塞式发送数据
                Data ++;
                break;

                case 'n':                                     //换行符
								singleBuff[0] = 0x0a;    
								__HAL_UART_CLEAR_FLAG(USARTx, UART_CLEAR_TCF);/* Clear the TC flag in the ICR register */
								HAL_UART_Transmit(USARTx, (void*)singleBuff, 1, 1);//阻塞式发送数据
                Data ++;
                break;

                default:
                Data ++;
                break;
            }            
        }
        else if ( * Data == '%')
        {                                     
            switch ( *++Data )
            {               
                case 's':                                         //字符串
                s = va_arg(ap, const char *);
                for ( ; *s; s++) 
                {
									singleBuff[0] = *s;    
									HAL_UART_Transmit(USARTx, (void*)singleBuff, 1, 1);//阻塞式发送数据
                  while(__HAL_UART_GET_FLAG(USARTx, UART_FLAG_TXE) == RESET);
                }
                Data++;
                break;

                case 'd':           
                d = va_arg(ap, int);
                itoa(d, buf, 10);
                for (s = buf; *s; s++) 
                {
									singleBuff[0] = *s;    
									HAL_UART_Transmit(USARTx, (void*)singleBuff, 1, 1);//阻塞式发送数据
                  while(__HAL_UART_GET_FLAG(USARTx, UART_FLAG_TXE) == RESET);
                }
                     Data++;
                     break;
                default:
                     Data++;
                     break;
            }        
        }
        else
				{
					singleBuff[0] = *Data++;    
					__HAL_UART_CLEAR_FLAG(USARTx, UART_CLEAR_TCF);/* Clear the TC flag in the ICR register */
					HAL_UART_Transmit(USARTx, (void*)singleBuff, 1, 1);//阻塞式发送数据
				}		
        while (__HAL_UART_GET_FLAG(USARTx, UART_FLAG_TXE) == RESET);
    }
}


bool ESP8266_MQTTUSERCFG( char * pClient_Id, char * pUserName,char * PassWord)
{
    char cCmd [120];
    sprintf ( cCmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"", pClient_Id,pUserName,PassWord );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}

bool ESP8266_MQTTCONN( char * Ip, int  Num)
{
    char cCmd [120];
    sprintf ( cCmd,"AT+MQTTCONN=0,\"%s\",%d,1", Ip,Num);
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 1000 );
}


bool ESP8266_MQTTSUB(char * Topic)
{
    char cCmd [120];
    sprintf ( cCmd, "AT+MQTTSUB=0,\"%s\",1",Topic );
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}

bool ESP8266_MQTTPUB( char * Topic,char *temp)
{
    char cCmd [120];
    // 使用处理后的数据构造AT指令
    sprintf(cCmd, "AT+MQTTPUB=0,\"%s\",\"%s\",1,0", Topic, temp);
    return ESP8266_Send_AT_Cmd(cCmd, "OK", NULL, 1000);
}
bool ESP8266_MQTTPUBRAW( char * Topic,char *len)
{
    char cCmd [120];
    sprintf(cCmd, "AT+MQTTPUBRAW=0,\"%s\",%s,1,0", Topic, len);
    return ESP8266_Send_AT_Cmd(cCmd, "OK", NULL, 1000);
}

bool ESP8266_MQTTCLEAN(void)
{
    char cCmd [120];
    sprintf ( cCmd, "AT+MQTTCLEAN=0");
    return ESP8266_Send_AT_Cmd( cCmd, "OK", NULL, 500 );
}


bool MQTT_SendString(char * pTopic,char *temp2)
{
    bool bRet = false;
    ESP8266_MQTTPUB(pTopic,temp2);
	HAL_Delay(1000);
    bRet = true;
    return bRet;
}

//
void ESP8266_STA_MQTTClient(void)
{
	printf("***************恢复出厂默认模式***************\r\n");
    //ESP8266_AT_Test();	//恢复出厂默认模式
	printf("***************正在配置MQTT模式***************\r\n");
    HAL_Delay(1000);
    ESP8266_Net_Mode_Choose(STA);
    HAL_Delay(1000);
    ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD);
    HAL_Delay(1000);
	ESP8266_MQTTUSERCFG(User_ESP8266_client_id,User_ESP8266_username,User_ESP8266_password);
	HAL_Delay(1000);
    ESP8266_MQTTCONN( User_ESP8266_MQTTServer_IP, User_ESP8266_MQTTServer_PORT);
	printf("***************MQTT模式配置完成***************\r\n");
}

//Json格式解析
void ESP8266_Json_Parse(char *pData)
{
    if (pData == NULL) return;
    
    // 提取JSON数据部分（跳过MQTT响应头）
    char *json_start = strchr(pData, '{');
    if (json_start == NULL) {
        printf("未找到JSON数据\r\n");
        return;
    }
    
    // 查找JSON结束符
    char *json_end = strrchr(json_start, '}');
    if (json_end == NULL) {
        printf("JSON格式不完整\r\n");
        return;
    }
    
    // 创建临时缓冲区存储完整JSON
    char json_buf[1024];
    size_t json_len = json_end - json_start + 1;
    if (json_len > sizeof(json_buf) - 1) {
        printf("JSON数据过长\r\n");
        return;
    }
    memcpy(json_buf, json_start, json_len);
    json_buf[json_len] = '\0'; // 添加字符串结束符
    
    // 解析JSON字符串
    cJSON *root = cJSON_Parse(json_buf);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("JSON解析错误: %s\r\n", error_ptr);
        }
        // 必须确保root为NULL时不执行后续操作
        return; // 增加return防止空指针访问
    }
    
    // 解析LED状态
    cJSON *led1 = cJSON_GetObjectItem(root, "LED1");
    if (led1 && cJSON_IsNumber(led1))
    {
        uint8_t led1_val = led1->valueint;
        // 添加LED值范围检查
        if (led1_val > 1) {
            printf("LED值超出范围\r\n");
            cJSON_Delete(root); // 提前释放内存
            return;
        }
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, led1_val ? GPIO_PIN_RESET : GPIO_PIN_SET);

        printf("解析成功: LED1=%s\r\n", led1_val?"ON":"OFF");
    }
    // 释放cJSON内存
    cJSON_Delete(root);
    root = NULL; // 避免悬空指针
}