#include "bsp_ov2640_config.h"
#include "bsp_ov2640.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "i2c.h"

static uint8_t OV2640_ReadID(OV2640_IDTypeDef* OV2640ID);
static void i2c_sent(uint8_t sub_address,uint8_t data);
static uint8_t i2c_receive(uint8_t sub_address);
static void SCCB_WriteRegs(const uint8_t* pbuf);
static void OV2640_AutoExposure(uint8_t level);
static void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat);
static void OV2640_BrightnessConfig(uint8_t Brightness);
static void OV2640_Config(void);


OV2640_IDTypeDef OV2640ID;
uint16_t I2C_ADDRESS=0x60;
uint16_t lcd_width=320, lcd_height=240;//液晶屏的分辨率，用来计算地址偏移
uint16_t img_width=320, img_height=240;//摄像头采集图像的大小

void OV2640_Init()
{
	OV2640_ReadID(&OV2640ID);
	OV2640_Config();
	OV2640_UXGAConfig();

}

uint8_t OV2640_ReadID(OV2640_IDTypeDef* OV2640ID)
{
	i2c_sent(OV2640_DSP_RA_DLMT, 0x01);

	OV2640ID->Manufacturer_ID1 =i2c_receive(OV2640_SENSOR_MIDH);

	OV2640ID->Manufacturer_ID2 =i2c_receive(OV2640_SENSOR_MIDL);

	OV2640ID->Version =i2c_receive(OV2640_SENSOR_PIDH);

	OV2640ID->PID =i2c_receive(OV2640_SENSOR_PIDL);
  
  printf("OV2640 ID Information:\n");
  printf("  Manufacturer ID: 0x%02X%02X\n",
           OV2640ID->Manufacturer_ID1,
           OV2640ID->Manufacturer_ID2);
  printf("  Version: 0x%02X\n", OV2640ID->Version);
  printf("  Product ID: 0x%02X\n", OV2640ID->PID);


	return 0;
}

void i2c_sent(uint8_t sub_address,uint8_t data)
{
	uint8_t sent_data[2];
	sent_data[0]=sub_address;
	sent_data[1]=data;
	HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)I2C_ADDRESS, (uint8_t*)sent_data, 2, 10000);
}

uint8_t i2c_receive(uint8_t sub_address)
{
	uint8_t data;
	HAL_I2C_Mem_Read(&hi2c2,(uint16_t)I2C_ADDRESS,sub_address,1,(uint8_t*)&data,1,10000);
	return data;
}

/**
  * @brief  Resets the OV2640 camera.
  * @param  None
  * @retval None
  */
void OV2640_Reset(void)
{
	/*OV2640有两组寄存器，设置0xFF寄存器的值为0或为1时可选择使用不同组的寄存器*/
  i2c_sent(0xFF, 0x01);
  i2c_sent(0x12, 0x80);
}

void OV2640_Config(void)
{
	//OV2640_JPEGConfig(JPEG_320x240);
	OV2640_BrightnessConfig(0x20);
	OV2640_AutoExposure(0);

	//OV2640_UXGAConfig();
	OV2640_QQVGAConfig();

}

void OV2640_JPEGConfig(ImageFormat_TypeDef ImageFormat)
{
  uint32_t i;

  //OV2640_Reset();
  //delay_ms(200);

  for(i=0; i<(sizeof(OV2640_JPEG_INIT)/2); i++)
  {
    i2c_sent(OV2640_JPEG_INIT[i][0], OV2640_JPEG_INIT[i][1]);
		//delay_ms(1);
  }


  for(i=0; i<(sizeof(OV2640_YUV422)/2); i++)
  {
    i2c_sent(OV2640_YUV422[i][0], OV2640_YUV422[i][1]);
		//delay_ms(1);
  }

  i2c_sent(0xff, 0x01);
  i2c_sent(0x15, 0x00);


  for(i=0; i<(sizeof(OV2640_JPEG)/2); i++)
  {
    i2c_sent(OV2640_JPEG[i][0], OV2640_JPEG[i][1]);
		//delay_ms(1);
  }

  //delay_ms(100);

  switch(ImageFormat)
  {
    case JPEG_160x120:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
        i2c_sent(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
		//delay_ms(1);
      }
      break;
    }
    case JPEG_176x144:
    {
      for(i=0; i<(sizeof(OV2640_176x144_JPEG)/2); i++)
      {
        i2c_sent(OV2640_176x144_JPEG[i][0], OV2640_176x144_JPEG[i][1]);
      }
      break;
    }
    case JPEG_320x240:
    {
      for(i=0; i<(sizeof(OV2640_320x240_JPEG)/2); i++)
			{
				i2c_sent(OV2640_320x240_JPEG[i][0], OV2640_320x240_JPEG[i][1]);
				//delay_ms(1);
			}
      break;
    }
    case JPEG_352x288:
    {
      for(i=0; i<(sizeof(OV2640_352x288_JPEG)/2); i++)
      {
        i2c_sent(OV2640_352x288_JPEG[i][0], OV2640_352x288_JPEG[i][1]);
      }
      break;
    }
    case JPEG_800x600:
		{
			for(i=0; i<(sizeof(OV2640_800x600_JPEG)/2); i++)
			{
				i2c_sent(OV2640_800x600_JPEG[i][0], OV2640_800x600_JPEG[i][1]);
			//delay_ms(1);
			}
			break;
		}

    default:
    {
      for(i=0; i<(sizeof(OV2640_160x120_JPEG)/2); i++)
      {
        i2c_sent(OV2640_160x120_JPEG[i][0], OV2640_160x120_JPEG[i][1]);
      }
      break;
    }
  }
}

void OV2640_BrightnessConfig(uint8_t Brightness)
{
  i2c_sent(0xff, 0x00);
  i2c_sent(0x7c, 0x00);
  i2c_sent(0x7d, 0x04);
  i2c_sent(0x7c, 0x09);
  i2c_sent(0x7d, Brightness);
  i2c_sent(0x7d, 0x00);
}

void OV2640_AutoExposure(uint8_t level)
{
	switch(level)
	{
		case 0:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
			break;
		case 1:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL1);
			break;
		case 2:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL2);
			break;
		case 3:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL3);
			break;
		case 4:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL4);
			break;
		default:
			SCCB_WriteRegs(OV2640_AUTOEXPOSURE_LEVEL0);
			break;
	}

}
/**
  * @brief  Configures the OV2640 camera in QQVGA mode.
  * @param  None
  * @retval None
  */
void OV2640_QQVGAConfig(void)
{
  uint32_t i;

  OV2640_Reset();

  /* Initialize OV2640 */
  for(i=0; i<(sizeof(OV2640_QQVGA)/2); i++)
  {
    i2c_sent(OV2640_QQVGA[i][0], OV2640_QQVGA[i][1]);
    //Delay(2);
  }
}

/**
  * @brief  设置图像输出大小，OV2640输出图像的大小(分辨率),完全由该函数确定
  * @param  width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
  * @retval 0,设置成功，其他,设置失败
  */
uint8_t OV2640_OutSize_Set(uint16_t width,uint16_t height)
{
	uint16_t outh;
	uint16_t outw;
	uint8_t temp; 
	if(width%4)return 1;
	if(height%4)return 2;
	outw=width/4;
	outh=height/4; 
	i2c_sent(0XFF,0X00);	
	i2c_sent(0XE0,0X04);	
  i2c_sent(0X50,outw&0X00);		//配置v_divider hdivider
	i2c_sent(0X5A,outw&0XFF);		//设置OUTW的低八位
	i2c_sent(0X5B,outh&0XFF);		//设置OUTH的低八位
	temp=(outw>>8)&0X03;
	temp|=(outh>>6)&0X04;
	i2c_sent(0X5C,temp);				//设置OUTH/OUTW的高位 
	i2c_sent(0XE0,0X00);	
	return 0;
}

/**未测试*/
/**
  * @brief  设置图像尺寸大小,也就是所选格式的输出分辨率
  * @param  width,height:图像宽度和图像高度
  * @retval 0,设置成功，其他,设置失败
  */
uint8_t OV2640_ImageSize_Set(uint16_t width,uint16_t height)
{
	uint8_t temp;
	i2c_sent(0XFF,0X00);
	i2c_sent(0XE0,0X04);
	i2c_sent(0XC0,(width)>>3&0XFF);		//设置HSIZE的10:3位
	i2c_sent(0XC1,(height)>>3&0XFF);		//设置VSIZE的10:3位
	temp=(width&0X07)<<3;
	temp|=height&0X07;
	temp|=(width>>4)&0X80;
	i2c_sent(0X8C,temp);
	i2c_sent(0XE0,0X00);
	return 0;
}


/**未测试*/
/**
  * @brief  设置图像输出窗口
  * @param  sx,sy,起始地址
						width,height:宽度(对应:horizontal)和高度(对应:vertical)
  * @retval 0,设置成功， 其他,设置失败
  */
void OV2640_Window_Set(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
	uint16_t endx;
	uint16_t endy;
	uint8_t temp;
	endx=sx+width/2;	//V*2
 	endy=sy+height/2;

 	i2c_sent(0XFF,0X01);
	temp = i2c_receive(0X03);				//读取Vref之前的值
	temp&=0XF0;
	temp|=((endy&0X03)<<2)|(sy&0X03);
	i2c_sent(0X03,temp);				//设置Vref的start和end的最低2位
	i2c_sent(0X19,sy>>2);			//设置Vref的start高8位
	i2c_sent(0X1A,endy>>2);			//设置Vref的end的高8位

	temp = i2c_receive(0X32);				//读取Href之前的值
	temp&=0XC0;
	temp|=((endx&0X07)<<3)|(sx&0X07);
	i2c_sent(0X32,temp);				//设置Href的start和end的最低3位
	i2c_sent(0X17,sx>>3);			//设置Href的start高8位
	i2c_sent(0X18,endx>>3);			//设置Href的end的高8位
}

//未测试
/**
  * @brief  设置图像开窗大小
						由:OV2640_ImageSize_Set确定传感器输出分辨率从大小.
						该函数则在这个范围上面进行开窗,用于OV2640_OutSize_Set的输出
						注意:本函数的宽度和高度,必须大于等于OV2640_OutSize_Set函数的宽度和高度
						     OV2640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
						     自动计算缩放比例,输出给外部设备.
  * @param  width,height:宽度(对应:horizontal)和高度(对应:vertical),width和height必须是4的倍数
  * @retval 0,设置成功， 其他,设置失败
  */
uint8_t OV2640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t hsize;
	uint16_t vsize;
	uint8_t temp;
	if(width%4)return 1;
	if(height%4)return 2;
	hsize=width/4;
	vsize=height/4;
	i2c_sent(0XFF,0X00);
	i2c_sent(0XE0,0X04);
	i2c_sent(0X51,hsize&0XFF);		//设置H_SIZE的低八位
	i2c_sent(0X52,vsize&0XFF);		//设置V_SIZE的低八位
	i2c_sent(0X53,offx&0XFF);		//设置offx的低八位
	i2c_sent(0X54,offy&0XFF);		//设置offy的低八位
	temp=(vsize>>1)&0X80;
	temp|=(offy>>4)&0X70;
	temp|=(hsize>>5)&0X08;
	temp|=(offx>>8)&0X07;
	i2c_sent(0X55,temp);				//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	i2c_sent(0X57,(hsize>>2)&0X80);	//设置H_SIZE/V_SIZE/OFFX,OFFY的高位
	i2c_sent(0XE0,0X00);
	return 0;
}

/**
  * @brief  配置OV2640为UXGA模式，并设置输出图像大小
  * @param  None
  * @retval None
  */
void OV2640_UXGAConfig(void)
{
  uint32_t i;
	
	/*摄像头复位*/
  OV2640_Reset();

	/*进行三次寄存器写入，确保配置写入正常
	(在使用摄像头长排线时，IIC数据线干扰较大，必须多次写入来保证正常)*/
  /* 写入寄存器配置 */
	for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
	{
		i2c_sent(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

	}
	/* Initialize OV2640 */
	for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
	{
		i2c_sent(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

	}
	  /* Initialize OV2640 */
	for(i=0; i<(sizeof(OV2640_UXGA)/2); i++)
	{
		i2c_sent(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);

	}	
	/*设置输出的图像大小*/
	OV2640_OutSize_Set(img_width,img_height);   
}

void SCCB_WriteRegs(const uint8_t* pbuf)
{
	while(1)
	{
		if((*pbuf == 0) && (*(pbuf + 1) == 0))
		{
			break;
		}
		else
		{
    i2c_sent((uint8_t)*pbuf++, (uint8_t)*pbuf++);
		}
	}
}
