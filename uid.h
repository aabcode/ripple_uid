/*-------------------------------------------------------------------------
   uid.h: Include file for RIPPLE_UID project firmware

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

#ifndef UID_H
#define UID_H

#define uchar unsigned char

// The UID structure
// All fields are bytes in this structure (total 22), 
// whereas final UUID generated has only 16 total bytes (=128 bits) 
typedef struct{
    uint8_t ts_year;
    uint8_t ts_month;
    uint8_t ts_date;
    uint8_t ts_hours;
    uint8_t ts_mins;
    uint8_t ts_secs;
    uint8_t testline;
    uint8_t device_charax;
    uint8_t manuf_year;
    uint8_t manuf_month;
    uint8_t manuf_week;     
    uint8_t reserved_bit;
    uint8_t test_year;
    uint8_t test_month;
    uint8_t test_date;
    uint8_t random_bytes[7];
}uuid_t;

// Test line on which device tested & ID'd
#define ROM_CONST_DEVICE_TESTLINE       02  // 0-3 only
// Device characteristics code
#define ROM_CONST_DEVICE_CHARAX_CODE    07  // 0-7 only

// Given manufacturing data (assumed to be in ROM)
#define ROM_CONST_YEAR_OF_MANUFACTURE   24  // Only last 2 digits
#define ROM_CONST_MONTH_OF_MANUFACTURE  05  // 01-12 only
#define ROM_CONST_WEEK_OF_MANUFACTURE   17  // 01-53 only

// Test QC time parameters
#define ROM_CONST_YEAR_OF_TEST          24  // Only last 2 digits        
#define ROM_CONST_MONTH_OF_TEST         05  // 01-12 only
#define ROM_CONST_DATE_OF_TEST          04  // 01-31 only

// ------------------ Function prototypes

// I2C function prototypes
void init_I2C(void);
void Start(void);								
void Stop(void); 
unsigned char SlaveAck(void);
void NoAck(void);
void Wbyte(uchar x);
unsigned char Rbyte(void);
unsigned char I2C_Read(uchar Slave_addr, uchar addr);
void I2C_Write(uchar Slave_addr, uchar addr, uchar dat);

// UART functions
void    init_UART(void);
void    write_UART(uchar byte_to_tx);
uchar   read_UART(void);

// Miscellaneous functions
void delay_ms(unsigned int ms);
int bcd_to_decimal(uchar x);

#endif // UID_H