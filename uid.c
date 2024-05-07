/*-------------------------------------------------------------------------
   uid.c: Main and only firmware source file for RIPPLE_UID project

   Copyright (C) 2024, Abhijit A. Bokil / abhijit.bokil@gmail.com

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

-------------------------------------------------------------------------*/
#include <8052.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "uid.h"

// Microcontroller port pins for I2C and LED
__sbit __at (0xB6) SCL;  // P3_6;
__sbit __at (0xB7) SDA;  // P3_7;
__sbit __at (0xA0) LED; // P2_0

// Specified device criteria stored in ROM
__code uint8_t __at (0x1f00) const_device_testline = ROM_CONST_DEVICE_TESTLINE;
__code uint8_t __at (0x1f01) const_device_charax_code = ROM_CONST_DEVICE_CHARAX_CODE;
__code uint8_t __at (0x1f02) const_year_manuf = ROM_CONST_YEAR_OF_MANUFACTURE;
__code uint8_t __at (0x1f03) const_month_manuf = ROM_CONST_MONTH_OF_MANUFACTURE;
__code uint8_t __at (0x1f04) const_week_manuf = ROM_CONST_WEEK_OF_MANUFACTURE;
__code uint8_t __at (0x1f05) const_year_tested = ROM_CONST_YEAR_OF_TEST;
__code uint8_t __at (0x1f06) const_month_tested = ROM_CONST_MONTH_OF_TEST;
__code uint8_t __at (0x1f07) const_date_tested = ROM_CONST_DATE_OF_TEST;

//  Main function
void main(void)
{
	uuid_t uuid;
	uint8_t byte_cntr;
	uint64_t temp=0, temp_data=0, uuid_partA, uuid_partB;
	uint8_t uuid_array[16];

    // Clear LED
    LED = 0;

    // Initialize the onboard UART
    init_UART();

    // Initialize the I2C link
    init_I2C();
 
	// Read current datetime from RTC (convert BCD to decimal)
	uuid.ts_year =	bcd_to_decimal(I2C_Read(0xD0, 0x06));
	uuid.ts_month = bcd_to_decimal(I2C_Read(0xD0, 0x05));
	uuid.ts_date =  bcd_to_decimal(I2C_Read(0xD0, 0x04));
	uuid.ts_hours = bcd_to_decimal(I2C_Read(0xD0, 0x02));
	uuid.ts_mins = 	bcd_to_decimal(I2C_Read(0xD0, 0x01));
	uuid.ts_secs = 	bcd_to_decimal(I2C_Read(0xD0, 0x00));
	// Load testline index from ROM
	uuid.testline = const_device_testline;
	// Load device characteristics code from ROM
	uuid.device_charax = const_device_charax_code;
	// Load manufacturing timedata from ROM
	uuid.manuf_year = const_year_manuf;
    uuid.manuf_month = const_month_manuf;
    uuid.manuf_week = const_week_manuf;
	// Load test timedata from ROM
	uuid.test_year = const_year_tested;
    uuid.test_month = const_month_tested;
    uuid.test_date = const_date_tested;

	// Load random data into random part of UUID
	// Use timestamp seconds value as seed
	srand(uuid.ts_secs);
	for(byte_cntr=0;byte_cntr<7;byte_cntr++){
		uuid.random_bytes[byte_cntr] = rand()%256;
	}

	// Build partA of the final UUID
	temp = uuid.ts_year; 
	temp_data = temp << 57;	// 7 bits
	temp = uuid.ts_month;  
 	temp_data = (temp << 53) | temp_data;	// 4 bits
	temp = uuid.ts_date;
	temp_data = (temp << 48) | temp_data;//5
	temp = uuid.ts_hours;
	temp_data = (temp << 43) | temp_data;//5
	temp = uuid.ts_mins;
	temp_data = (temp << 37) | temp_data;//6
	temp = uuid.ts_secs;
	temp_data = (temp << 31) | temp_data;//6
	temp = uuid.testline;
	temp_data = (temp << 28) | temp_data;//3
	temp = uuid.device_charax;
	temp_data = (temp << 25) | temp_data;//3
	temp = uuid.manuf_year;
	temp_data = (temp << 18) | temp_data;//7
	temp = uuid.manuf_month;
	temp_data = (temp << 14) | temp_data;//4
	temp = uuid.manuf_week;
	temp_data = (temp << 8) | temp_data;//6
	// Reserved bit is always zero- //1
	temp = uuid.test_year;
	temp_data = (temp) | temp_data; // 7
	
	uuid_partA = temp_data;

	//temp_data = 0;
	temp = uuid.test_month;
	temp_data = (temp << 60); //4
	temp = uuid.test_date;
	temp_data = (temp << 55) | temp_data; //5

	for(byte_cntr=0;byte_cntr<7;byte_cntr++){
		temp = uuid.random_bytes[byte_cntr];
		temp_data = (temp << (48 - (8*byte_cntr))) | temp_data; // note..the LSb of test_date is unharmed
	}
	
	uuid_partB = temp_data;

	// Load the lower 8 & upper 8 bytes (simultaneously) of the uuid_array
	for(byte_cntr=0;byte_cntr<8;byte_cntr++){
		temp = uuid_partA >> (56 - (8*byte_cntr));
		uuid_array[byte_cntr] = (uint8_t)temp;
		temp = uuid_partB >> (56 - (8*byte_cntr));
		uuid_array[byte_cntr+8] = (uint8_t)temp;
	}

	// Push UUID to testhost
	for(byte_cntr=0;byte_cntr<16;byte_cntr++){
		write_UART((uchar)uuid_array[byte_cntr]);
	}

	LED = 1;
	while(1);
}

/*-----------------------------I2C Function Definitions-------------------------------------*/
void init_I2C(void){
    // Disable the Clock halt bit in reg0 of the RTC
    //I2C_Write(0xD0, 0x00, 0x80);
}

// Start I2C communication
void Start(void)
{
	SDA	= 1;
	SCL = 1;
	delay_ms(1);
	SDA	= 0;
	delay_ms(1);
	SCL = 0;
	delay_ms(1);
}

// Stop I2C communication
void Stop(void)
{
	SDA	= 0;
	delay_ms(1);
	SCL	= 1;
	delay_ms(1);
	SDA = 1;
	delay_ms(1);
}

// I2C slave NACK
void NoAck(void)
{
	SDA = 1;
	delay_ms(1);
	SCL = 1;
	delay_ms(1);
	SCL = 0;
	delay_ms(1);
}

// I2C slave acknowledge
uchar SlaveAck(void)
{
	uchar val;

	SCL	= 1;
	delay_ms(1);

	while(SDA != 0);
   
	val = SDA;
	SCL = 0;
	delay_ms(1);

	return(val);
}

// Write a single byte over I2C
void Wbyte(uchar x)
{
	uchar k;
	for(k=0; k<8; k++){
		if(x & (0x80 >> k)){
			SDA = 1;								
		}else{
			SDA = 0;								
		}
		delay_ms(1);
		SCL	= 1;
		delay_ms(1);
		SCL	= 0;
	}
}

// Read a single byte over I2C
uchar Rbyte(void)
{
	uchar k;
	uchar rb;

	rb=0;
	SCL	= 0;					
	for(k=0; k<8; k++){
		SCL = 1;									
		delay_ms(1);
		if(SDA){
			rb |= (0x80 >> k);						
		}
		delay_ms(1);
		SCL	= 0;									
		delay_ms(1);
	}
	
	return(rb);
}

// Writing device regs over I2C						
void I2C_Write(uchar Slave_addr, uchar addr, uchar dat)
{
	Start();
	Wbyte(Slave_addr);
	SlaveAck(); 
	Wbyte(addr);
	SlaveAck();
	Wbyte(dat);
	SlaveAck();
	Stop();	
	return;
}

// Reading device regs over I2C
uchar I2C_Read(uchar Slave_addr,uchar addr)
{
	uchar byte;

	Start();
	Wbyte(Slave_addr);
	SlaveAck();
	Wbyte(addr);
	SlaveAck();
	delay_ms(1);
	Start();
	Wbyte((Slave_addr|0x01));          
	SlaveAck();                     
	byte = Rbyte();
	NoAck();
	Stop();

	return(byte);
}

// Millisecond delay function
void delay_ms(unsigned int ms)
{
  unsigned int t1, t2;

   for(t1=0; t1<ms; t1++){
      for(t2=0; t2<334; t2++);
   }
} 

// Initialization of UART
void init_UART(void){

    SCON =0x50;
    TMOD = 0x20;
    TH1 = 0xFD;
    TR1  = 1;
}

// Function to transmit byte
void write_UART(uchar byte_to_tx){

    SBUF = byte_to_tx;
    while(TI == 0);
    TI = 0;
}

// Function to receive byte
uchar read_UART(void){

    __asm__("CLR RI");
    while(RI == 0);
    RI = 0;
    return(SBUF);
}

int bcd_to_decimal(uchar x){
    return x - 6 * (x >> 4);
}

uint8_t decimal_to_bcd(int y){
    
    uint8_t bcd_output;
    unsigned int units_place, tens_place;

    unsigned int i = y;
    tens_place = i/10;
    units_place = i%10;
    
    bcd_output = tens_place;
    bcd_output = (bcd_output << 4) + units_place;
   
    return(bcd_output);

}