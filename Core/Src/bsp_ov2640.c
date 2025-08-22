#include "bsp_ov2640_config.h"
#include "bsp_ov2640.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"
#include "i2c.h"
#include "cmsis_os2.h"

OV2640_IDTypeDef OV2640ID;
uint16_t I2C_ADDRESS=0x60;

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

void OV2640_Reset(void)
{
	/*OV2640有两组寄存器，设置0xFF寄存器的值为0或为1时可选择使用不同组的寄存器*/
  i2c_sent(0xFF, 0x01);
  i2c_sent(0x12, 0x80);
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

void OV2640_Config(void)
{
	OV2640_JPEGConfig(JPEG_320x240);

	OV2640_BrightnessConfig(0x20);
	OV2640_AutoExposure(0);
}


void OV2640_Init()
{
	//OV2640_Reset();
	OV2640_ReadID(&OV2640ID);

	OV2640_Config();
}
