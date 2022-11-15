#include <INA226.h>

//SlowSoftI2CMaster arciic = SlowSoftI2CMaster(19, 18);

//读取ina226寄存器子函数
short INA226_read(unsigned char reg)
{
	unsigned short RegVal = 0;
  //写入设备地址时需要写入8位0x40需要左移一位变成0x80，以此将最后一位作为写入/读取标志位
	arciic.i2c_start_wait(0x80);
  //写入设备地址后可以直接写入需要读取的寄存器地址，无需左移
	arciic.i2c_start_wait(reg);

	arciic.i2c_stop();
	
	delayMicroseconds(10);
	
  //与上1相当于将最后一位置1，意为读取
	arciic.i2c_start_wait(0x81);
	
	RegVal = arciic.i2c_read(false);
	RegVal <<= 8;
	RegVal |= arciic.i2c_read(true);
	arciic.i2c_stop();
	return RegVal;
}

//写入ina226寄存器子函数，reg_addr：寄存器地址，reg_data：要写入的数据。
short INA226_Write(uint8_t reg_addr,uint16_t reg_data)
{        
	uint8_t data_high=(uint8_t)((reg_data&0xFF00)>>8);
	uint8_t data_low=(uint8_t)reg_data&0x00FF;

	arciic.i2c_start_wait(0x80);   

	arciic.i2c_start_wait(reg_addr);    
       
	arciic.i2c_start_wait(data_high);
        
	arciic.i2c_start_wait(data_low);
              
	arciic.i2c_stop();
	delayMicroseconds(10);
	return 1;
}