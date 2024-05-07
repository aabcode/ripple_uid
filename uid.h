/****************************************************************************/
/*	Project:	| RIPPLE_UID												*/
/*	Filename:	| uid.h														*/
/*	Summary:	| Header file for RIPPLE_UID project		                */
/*	Copyright:	| GPL														*/
/* ------------------------------------------------------------------------ */
/* 	Changelog	| Date		| Author	| Version |	Comments				*/
/* ------------------------------------------------------------------------	*/
/*				| 04/05/24	| A. Bokil	|	1.0	  | Creation				*/
/****************************************************************************/

#define uchar unsigned char

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

// Function prototypes
void Start(void);								
void Stop(void); 

unsigned char SlaveAck(void);
void NoAck(void);

void Wbyte(unsigned char x);
unsigned char Rbyte(void);

unsigned char I2C_Read(unsigned char Slave_addr, unsigned char addr);
void I2C_Write(unsigned char Slave_addr, unsigned char addr, unsigned char dat);

void    init_UART(void);
void    write_UART(uchar byte_to_tx);
uchar   read_UART(void);

void delay_ms(unsigned int ms);
void init_I2C(void);

int bcd_to_decimal(unsigned char x);
uint8_t decimal_to_bcd(int y);