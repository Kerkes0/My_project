/*
 * @Description: In User Settings Edit
 * @Author: Deng Senyu
 * @Date: 2019-08-20 17:14:13
 * @LastEditTime: 2019-08-23 09:08:40
 * @LastEditors: Please set LastEditors
 */
#include "Siic.h"

#define SIIC_WRITE 0x00
#define SIIC_READ 0x01




//产生IIC起始信号
static void Siic_Start(Sciitag *iic)
{
	GPIO_PinOut(iic->sda, 1); 			// SDA=1;
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 1); 	//设置SDA为输出
	GPIO_PinOut(iic->scl, 1);			// SCL=1;
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->sda, 0); 			// SDA=0;
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->scl, 0); 			// SCL=0;
}

//产生IIC停止信号
static void Siic_Stop(Sciitag *iic)
{
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 1); 	//设置SDA为输出
	GPIO_PinOut(iic->sda, 0);		 	// SDA=0;
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->scl, 1); 			// SCL=1;
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->sda, 1); 			// SDA=1;
	delay_us(iic->time_delayus);
}

static void Siic_Write(Sciitag *iic, u8 data)
{
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 1); //设置SDA为输出
	u8 i = 8;
	while (i--)
	{
		GPIO_PinOut(iic->sda, (data >> i) & 0x01);
		delay_us(iic->time_delayus);
		GPIO_PinOut(iic->scl, 1);
		delay_us(iic->time_delayus);
		GPIO_PinOut(iic->scl, 0);
	}
}

static u8 Siic_Read(Sciitag *iic)
{
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 0); //设置SDA为输入
	u8 data, i = 8;
	GPIO_PinOut(iic->sda, 1);//读取SDA电平前，需设置为高电平（？）
	while (i--)
	{
		delay_us(iic->time_delayus);
		GPIO_PinOut(iic->scl, 1);
		delay_us(iic->time_delayus);
//		data <<= 1;
//		if(I2C_read(iic)){
//			data++;
//		}
		data = (data << 1) + GPIO_PinIn(iic->sda);
		GPIO_PinOut(iic->scl, 0);
	}
	delay_us(iic->time_delayus);
	return data;
}

//产生ACK应答
static void Siic_Ack(Sciitag *iic, u8 mode)
{
	
	if (mode)
		GPIO_PinOut(iic->sda, 0);
	else
		GPIO_PinOut(iic->sda, 1);
	
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 1); //设置SDA为输出
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->scl, 1);
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->scl, 0);
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
static u8 Siic_Wait_Ack(Sciitag *iic)
{
	static u8 ret;
//	uint8_t _pin = FastLog2(iic->sda.Pin);
//	PinType(iic->sda.GPIO, _pin, 0); //设置SDA为输入
	GPIO_PinOut(iic->sda, 1);//读取SDA电平前，需设置为高电平（？）//设置SDA为输入
	delay_us(iic->time_delayus);
	GPIO_PinOut(iic->scl, 1);
	delay_us(iic->time_delayus);
	ret = GPIO_PinIn(iic->sda);

	GPIO_PinOut(iic->sda, ret);
//	PinType(iic->sda.GPIO, _pin, 1); //设置SDA为输出

	GPIO_PinOut(iic->scl, 0);
	delay_us(iic->time_delayus);
	return ret;
}

u8 Siic_Send(Sciitag *iic, u8 addr, u8 comm, const u8 data[], u8 len)
{
	Siic_Start(iic);

	Siic_Write(iic, addr | SIIC_WRITE);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}

	Siic_Write(iic, comm);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}

	for (u8 i = 0; i < len; i++)
	{
		Siic_Write(iic, data[i]);
		if (Siic_Wait_Ack(iic))
		{
			Siic_Stop(iic);
			return 1; //发送失败
		}
	}

	Siic_Stop(iic);
	return 0;
}

u8 Siic_Com(Sciitag *iic, u8 addr, const u8 comm[], u8 len)
{
	Siic_Start(iic);

	Siic_Write(iic, addr | SIIC_WRITE);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}

	for (u8 i = 0; i < len; i++)
	{
		Siic_Write(iic, comm[i]);
		if (Siic_Wait_Ack(iic))
		{
			Siic_Stop(iic);
			return 1; //发送失败
		}
	}

	Siic_Stop(iic);
	return 0;
}

u8 Siic_Recv(Sciitag *iic, u8 addr, u8 comm, u8 data[], u8 len)
{
	Siic_Start(iic);

	Siic_Write(iic, addr | SIIC_WRITE);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}

	Siic_Write(iic, comm);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}

	Siic_Start(iic);

	Siic_Write(iic, addr | SIIC_READ);
	if (Siic_Wait_Ack(iic))
	{
		Siic_Stop(iic);
		return 1; //发送失败
	}
	u8 i = 0;
	while (len--)
	{
		data[i++] = Siic_Read(iic);
		Siic_Ack(iic, len);
	}

	Siic_Stop(iic);
	return 0;
}
