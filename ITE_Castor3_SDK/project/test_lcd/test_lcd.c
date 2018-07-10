#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"

#define LCD_SPI_INIT_ENALBE
#if defined(LCD_SPI_INIT_ENALBE)
/*
GPIO33 -> CS
GPIO35 -> CLK
GPIO34 -> DATA
GPIO32 -> RESET
*/
#define CS_PIN 		72
#define SCL_PIN 	70
#define SDA_PIN 	71
#define RST_PIN 	33


volatile int delay_cnt = 0;
static void delay(int count)
{
    int i;
    ithEnterCritical();
    for (i = 0; i<count; i++)
        delay_cnt++;
    ithExitCritical();
}

static void LCD_DelayUS(int cout)
{
    int i, j;
    //ithEnterCritical();
    for (i = 0; i < cout; i++)
    {
        for (j = 0; j < 25; j++)
        {
            delay_cnt++;
        }
    }
    delay_cnt = 0;
    //ithExitCritical();
}

static void LCD_DelayMS(int cout)
{
    int i, j;
    //ithEnterCritical();
    for (i = 0; i < cout; i++)
    {
        for (j = 0; j < 25000; j++)
        {
            delay_cnt++;
        }
    }
    delay_cnt = 0;
    //ithExitCritical();
}

static void SPI_SendData(uint8_t raw, bool dc)
{
    int i;
    uint16_t data = 0;
    if (dc)
        data |= 0x0100; //transfer data
    data |= raw;
     ithGpioClear(SCL_PIN);
    for (i = 0; i < 9; i++)
    {
        if (data & 0x100){
            ithGpioSet(SDA_PIN);
        }
        else{
            ithGpioClear(SDA_PIN);
        }
        LCD_DelayUS(20);
        ithGpioClear(SCL_PIN);
        LCD_DelayUS(20);
        ithGpioSet(SCL_PIN);
        LCD_DelayUS(20);

        data <<= 1;
    }

//    ithGpioClear(SCL_PIN);
}

static int Write_COM(uint8_t command)
{
    int result = 0;
    int i;
    printf("Write_COM:0x%x\r\n", command);
    //pull low cs to start
    ithGpioClear(CS_PIN);
    LCD_DelayUS(10);

    //transfer command, msb first
    SPI_SendData(command, false);

    //pull high cs to stop
    ithGpioSet(CS_PIN);
    LCD_DelayUS(2);

    return result;
}

static int Write_DATA(uint8_t data)
{
    int result = 0;
    int i;
    printf("Write_DATA:0x%x\r\n", data);



    //pull low cs to start
    ithGpioClear(CS_PIN);
    LCD_DelayUS(10);



    //transfer data, msb first
    SPI_SendData(data, true);

    //pull high cs to stop
    ithGpioSet(CS_PIN);
    LCD_DelayUS(2);

    return result;
}

#define SDA_IN ithGpioSetIn(SDA_PIN)
#define SDA_OUT ithGpioSetOut(SDA_PIN)
//#define SDA_GET {return ithGpioGet(SDA_PIN);}
#define CS_H ithGpioSet(CS_PIN)
#define CS_L ithGpioClear(CS_PIN)
#define SCL_H ithGpioSet(SCL_PIN)
#define SCL_L ithGpioClear(SCL_PIN)
#define SDA_H ithGpioSet(SDA_PIN)
#define SDA_L ithGpioClear(SDA_PIN)

static unsigned char  Read_DATA(bool RisingEdge)
{
	unsigned char i,data=0;

	SDA_IN;

	for(i=0;i<8;i++)
	{
		data<<=1;

		if(RisingEdge)
		{
			SCL_L;
			SCL_H;
		}
		else
		{
			SCL_H;
			SCL_L;
		}

		if(ithGpioGet(SDA_PIN))
		{
			data++;
		}

	}

	SDA_OUT;
	SDA_H;
	return data;
}

static unsigned char Read_REG(unsigned char addr)
{
	unsigned char data=0;
	CS_H;
	CS_L;
	LCD_DelayUS(1);
	Write_COM(addr);
	data= Read_DATA(false);
	CS_H;
	return data;
}

unsigned long ILI9806E_read_id(void)
{
	unsigned long DeviceCode=0;

	unsigned char  id= 0x00;
	Write_COM(0xFF);
	Write_DATA(0xFF);
	Write_DATA(0x98);
	Write_DATA(0x06);
	Write_DATA(0x04);
	Write_DATA(0x01);

	LCD_DelayMS(10);

	Write_COM(0x08);
	Write_DATA(0x10);//or 08h

	Write_COM(0xfe);
	Write_DATA(0x81);//;


	id= Read_REG(0x00);
	DeviceCode= id<<16;

	id= Read_REG(0x01);
	DeviceCode= (DeviceCode |id<<8);

	id= Read_REG(0x02);
	DeviceCode= (DeviceCode |id);

	printf("ILI9806E_read_id() DeviceCode=0x%x\r\n",DeviceCode);

	Write_COM(0xfe);
	Write_DATA(0x00);

	return DeviceCode;
}

static void LCD_InitGPIO()
{
	ithGpioSetOut(CS_PIN);
	ithGpioEnable(CS_PIN);

	ithGpioSetOut(SCL_PIN);
	ithGpioEnable(SCL_PIN);

	ithGpioSetOut(SDA_PIN);
	ithGpioEnable(SDA_PIN);



	ithGpioSetOut(RST_PIN);
	ithGpioEnable(RST_PIN);
}

static void LCD_Reset()
{
    ithGpioSet(CS_PIN);
    ithGpioSet(SCL_PIN);

    ithGpioSet(RST_PIN);
    LCD_DelayUS(100);
    ithGpioClear(RST_PIN);
    LCD_DelayUS(150);
    ithGpioSet(RST_PIN);
    LCD_DelayUS(80);

}

static void InitLcdSPIData()
{
	LCD_InitGPIO();
	LCD_DelayMS(10);
	LCD_Reset();

	//LCD_DelayMS(50);
	//ILI9806E_read_id();

	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x01);     // Change to Page 1

	Write_COM(0x08);Write_DATA(0x10);                 // output SDA

	Write_COM(0x20);Write_DATA(0x00);      //0x01           // 0:DE mode,1:SYNC mode

	Write_COM(0x21);Write_DATA(1<<3|1<<2|1<<1|0<<0);                 // DE = 1 Active

	Write_COM(0x30);Write_DATA(0x01);                 // 480 X 854

	Write_COM(0x31);Write_DATA(0x02);                 // 2-dot Inversion


#if 1
	Write_COM(0x40);Write_DATA(0x18);                // DDVDH/L BT 17 -10 5.94 -5.47 -2.3 1.6 43.4 21.88 0.17

	Write_COM(0x41);Write_DATA(0x66);              // DDVDH/L CLAMP 

	Write_COM(0x42);Write_DATA(0x02);                 // VGH/VGL 

	Write_COM(0x43);Write_DATA(0x0A);                 // VGH/VGL 

	Write_COM(0x50);Write_DATA(0x78);                 // VGMP 4.6 

	Write_COM(0x51);Write_DATA(0x78);                 // VGMN 4.6 

	Write_COM(0x52);Write_DATA(0x00);                 // Flicker 

	Write_COM(0x53);Write_DATA(0x1F);                 // Flicker 

	Write_COM(0x57);Write_DATA(0x50);

	Write_COM(0x60);Write_DATA(0x07);                 // SDTI

	Write_COM(0x61);Write_DATA(0x01);                 // CRTI 00

	Write_COM(0x62);Write_DATA(0x07);                 // EQTI 07

	Write_COM(0x63);Write_DATA(0x00);                 // PCTI  00


	//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x01);     // Change to Page 1

	Write_COM(0xA0);Write_DATA(0x00);  // Gamma 0 

	Write_COM(0xA1);Write_DATA(0x0A);  // Gamma 4 

	Write_COM(0xA2);Write_DATA(0x12);  // Gamma 8

	Write_COM(0xA3);Write_DATA(0x0D);  // Gamma 16

	Write_COM(0xA4);Write_DATA(0x07);  // Gamma 24

	Write_COM(0xA5);Write_DATA(0x09);  // Gamma 52

	Write_COM(0xA6);Write_DATA(0x06); // Gamma 80

	Write_COM(0xA7);Write_DATA(0x03);  // Gamma 108

	Write_COM(0xA8);Write_DATA(0x09);  // Gamma 147

	Write_COM(0xA9);Write_DATA(0x0D);  // Gamma 175

	Write_COM(0xAA);Write_DATA(0x15);  // Gamma 203

	Write_COM(0xAB);Write_DATA(0x07);  // Gamma 231

	Write_COM(0xAC);Write_DATA(0x0D);  // Gamma 239

	Write_COM(0xAD);Write_DATA(0x0D);  // Gamma 247

	Write_COM(0xAE);Write_DATA(0x06);  // Gamma 251

	Write_COM(0xAF);Write_DATA(0x00);  // Gamma 255

	///==============Nagitive
	Write_COM(0xC0);Write_DATA(0x00);  // Gamma 0 

	Write_COM(0xC1);Write_DATA(0x0A);  // Gamma 4

	Write_COM(0xC2);Write_DATA(0x12);  // Gamma 8

	Write_COM(0xC3);Write_DATA(0x0E);  // Gamma 16

	Write_COM(0xC4);Write_DATA(0x07);  // Gamma 24

	Write_COM(0xC5);Write_DATA(0x09);  // Gamma 52

	Write_COM(0xC6);Write_DATA(0x07);  // Gamma 80

	Write_COM(0xC7);Write_DATA(0x03);  // Gamma 108

	Write_COM(0xC8);Write_DATA(0x09);  // Gamma 147

	Write_COM(0xC9);Write_DATA(0x0C);  // Gamma 175

	Write_COM(0xCA);Write_DATA(0x15);  // Gamma 203

	Write_COM(0xCB);Write_DATA(0x07);  // Gamma 231

	Write_COM(0xCC);Write_DATA(0x0C);  // Gamma 239

	Write_COM(0xCD);Write_DATA(0x0D); // Gamma 247

	Write_COM(0xCE);Write_DATA(0x06); // Gamma 251

	Write_COM(0xCF);Write_DATA(0x00);  // Gamma 255


	//****************************************************************************//
	//****************************** Page 6 Command ******************************//
	//****************************************************************************//
	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x06);     // Change to Page 6

	Write_COM(0x00);Write_DATA(0x21);

	Write_COM(0x01);Write_DATA(0x06);

	Write_COM(0x02);Write_DATA(0xA0);   

	Write_COM(0x03);Write_DATA(0x02);

	Write_COM(0x04);Write_DATA(0x01);

	Write_COM(0x05);Write_DATA(0x01);

	Write_COM(0x06);Write_DATA(0x80);   

	Write_COM(0x07);Write_DATA(0x04);

	Write_COM(0x08);Write_DATA(0x00);

	Write_COM(0x09);Write_DATA(0x80);    

	Write_COM(0x0A);Write_DATA(0x00);    

	Write_COM(0x0B);Write_DATA(0x00);    

	Write_COM(0x0C);Write_DATA(0x2c);

	Write_COM(0x0D);Write_DATA(0x2c);

	Write_COM(0x0E);Write_DATA(0x1c);

	Write_COM(0x0F);Write_DATA(0x00);

	Write_COM(0x10);Write_DATA(0xFF);

	Write_COM(0x11);Write_DATA(0xF0);

	Write_COM(0x12);Write_DATA(0x00);

	Write_COM(0x13);Write_DATA(0x00);

	Write_COM(0x14);Write_DATA(0x00);

	Write_COM(0x15);Write_DATA(0xC0);

	Write_COM(0x16);Write_DATA(0x08);

	Write_COM(0x17);Write_DATA(0x00);

	Write_COM(0x18);Write_DATA(0x00);

	Write_COM(0x19);Write_DATA(0x00);

	Write_COM(0x1A);Write_DATA(0x00);

	Write_COM(0x1B);Write_DATA(0x00);

	Write_COM(0x1C);Write_DATA(0x00);

	Write_COM(0x1D);Write_DATA(0x00);


	Write_COM(0x20);Write_DATA(0x01);

	Write_COM(0x21);Write_DATA(0x23);

	Write_COM(0x22);Write_DATA(0x45);

	Write_COM(0x23);Write_DATA(0x67);

	Write_COM(0x24);Write_DATA(0x01);

	Write_COM(0x25);Write_DATA(0x23);

	Write_COM(0x26);Write_DATA(0x45);

	Write_COM(0x27);Write_DATA(0x67);


	Write_COM(0x30);Write_DATA(0x12);

	Write_COM(0x31);Write_DATA(0x22);

	Write_COM(0x32);Write_DATA(0x22);

	Write_COM(0x33);Write_DATA(0x22);

	Write_COM(0x34);Write_DATA(0x87);

	Write_COM(0x35);Write_DATA(0x96);

	Write_COM(0x36);Write_DATA(0xAA);

	Write_COM(0x37);Write_DATA(0xDB);

	Write_COM(0x38);Write_DATA(0xCC);

	Write_COM(0x39);Write_DATA(0xBD);

	Write_COM(0x3A);Write_DATA(0x78);

	Write_COM(0x3B);Write_DATA(0x69);

	Write_COM(0x3C);Write_DATA(0x22);

	Write_COM(0x3D);Write_DATA(0x22);

	Write_COM(0x3E);Write_DATA(0x22);

	Write_COM(0x3F);Write_DATA(0x22);

	Write_COM(0x40);Write_DATA(0x22);

	Write_COM(0x52);Write_DATA(0x10);

	Write_COM(0x53);Write_DATA(0x10);


	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x07);    // Change to Page 7

	Write_COM(0x17);Write_DATA(0x22);

	Write_COM(0x18);Write_DATA(0x1D);

	Write_COM(0x02);Write_DATA(0x77);

	Write_COM(0xE1);Write_DATA(0x79);

	Write_COM(0x06);Write_DATA(0x11);

#endif
	//****************************************************************************//
	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x00);     // Change to Page 0

	Write_COM(0x3A);Write_DATA(0x70);

	Write_COM(0x11);Write_DATA(0x00);                 // Sleep-Out
	LCD_DelayUS(120);
	Write_COM(0x29);Write_DATA(0x00);                 // Display On
}
#endif //defined(LCD_SPI_INIT_ENALBE)

//#define SPI_INIT
#if defined(SPI_INIT)
#if 1
#define CS_PIN 		72
#define SCL_PIN 	70
#define SDA_PIN 	71
#define RST_PIN 	33//69
#else
#define CS_PIN 		71//72
#define SCL_PIN 	69//70
#define SDA_PIN 	70//71
#define RST_PIN 	28//69
#endif

static void LCD_InitGPIO()
{
	ithGpioSetMode(CS_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(CS_PIN);
	ithGpioEnable(CS_PIN);
	ithGpioClear(CS_PIN);

	ithGpioSetMode(SCL_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(SCL_PIN);
	ithGpioEnable(SCL_PIN);

	ithGpioSetMode(SDA_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(SDA_PIN);	
	ithGpioEnable(SDA_PIN);
	
	ithGpioSetMode(RST_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(RST_PIN);	
	ithGpioEnable(RST_PIN);
	
}

#define SDA_IN ithGpioSetIn(SDA_PIN)
#define SDA_OUT ithGpioSetOut(SDA_PIN)
#define SDA_GET {return ithGpioGet(SDA_PIN);}
#define CS_H ithGpioSet(CS_PIN)
#define CS_L ithGpioClear(CS_PIN)
#define SCL_H ithGpioSet(SCL_PIN)
#define SCL_L ithGpioClear(SCL_PIN)
#define SDA_H ithGpioSet(SDA_PIN)
#define SDA_L ithGpioClear(SDA_PIN)

volatile int delay_cnt = 0;
static void delay(int count)
{
	int i;
	ithEnterCritical();
	for (i = 0; i<count; i++)
		delay_cnt++;
	ithExitCritical();
}

static void LCD_DelayUS(int cout)
{
	int i,j;
	//ithEnterCritical();
	for (i = 0; i < cout; i++) 
	{
		for (j = 0; j < 25; j++) 
		{
			delay_cnt++;
		}
	}
	delay_cnt = 0;
	//ithExitCritical();
}

static void LCD_DelayMS(int cout)
{
	int i,j;
	//ithEnterCritical();
	for (i = 0; i < cout; i++) 
	{
		for (j = 0; j < 25000; j++) 
		{
			delay_cnt++;
		}
	}
	delay_cnt = 0;
	//ithExitCritical();
}

static void SPI_WriteComm(unsigned char reg)
{
	int i;
	unsigned short usValue = 0;
	unsigned short usMask = 0x100;

	usValue |= reg;//00reg
	//ithPrintf("usValue:0x%02x, %s, %d.\n", usValue, __FUNCTION__, __LINE__);

	SCL_H;
    CS_L;//clr_CS;
    LCD_DelayUS(10);
	for(i = 0; i < 9; i++)
	{
		if(usValue & usMask)
		{
			//ithPrintf("1");
			SCL_L;
			LCD_DelayUS(20);
			SDA_H; 
			LCD_DelayUS(20);
			SCL_H;
			LCD_DelayUS(20);
		}
		else
		{
			//ithPrintf("0");
			SCL_L;
			LCD_DelayUS(20);
			SDA_L;
			LCD_DelayUS(20);
			SCL_H; 
			LCD_DelayUS(20);
	   }

		usMask >>= 1;
	}

	CS_H;
	LCD_DelayUS(2);
	//ithPrintf("\n");
}
/////////////////////////////////////////////////////////////////
void SPI_WriteData(unsigned int data)
{
	int i;
	unsigned int usValue = 0xFFFF00;
	unsigned int usMask = 0x100;
	
	usValue |= data;
	//ithPrintf("usValue:0x%02x, %s, %d.\n", usValue, __FUNCTION__, __LINE__);

	SCL_H;
	CS_L;//clr_CS;
	LCD_DelayUS(10);
	
	for(i = 0; i < 9; i++)
	{
		if(usValue & usMask)
		{
			//ithPrintf("1");
			SCL_L;
			LCD_DelayUS(20);
			SDA_H;
			LCD_DelayUS(20);
			SCL_H;
			LCD_DelayUS(20);
		}
		else
		{
			//ithPrintf("0");
			SCL_L;
			LCD_DelayUS(20);
			SDA_L;
			LCD_DelayUS(20);
			SCL_H;
			LCD_DelayUS(20);
		}
		usMask>>=1;
	}

	CS_H;
	LCD_DelayUS(2);
	//ithPrintf("\n");
}

static unsigned char  SPI_ReadData(bool RisingEdge)
{
       unsigned char i,data=0;
  
       SDA_IN;
  
       for(i=0;i<8;i++)
       {
              data<<=1;
  
              if(RisingEdge)
              {
                     SCL_L;
                     SCL_H;
              }
              else
              {
                     SCL_H;
                     SCL_L;
              }
              
              if(SDA_GET)
              {
                     data++;
              }
  
       }
  
       SDA_OUT;
       SDA_H;
       returndata;
}

static unsigned char SPI_ReadReg(unsigned charaddr)
{
       unsigned char data=0;
       CS_H;
       CS_L;
       LCD_DelayUS(1);
       SPI_WriteComm(addr);
       data= SPI_ReadData(false);
       CS_H;
       return data;
}

unsigned long ILI9806E_read_id(void)
{
       unsigned long DeviceCode=0;
       
       unsigned char  id= 0x00;
       SPI_WriteComm(0xFF);
       SPI_WriteData(0xFF);
       SPI_WriteData(0x98);
       SPI_WriteData(0x06);
       SPI_WriteData(0x04);
       SPI_WriteData(0x01);
  
       LCD_DelayMS(10);
       
       SPI_WriteComm(0x08);
       SPI_WriteData(0x10);//or 08h
  
       SPI_WriteComm(0xfe);
       SPI_WriteData(0x81);//;
  
  
       id= SPI_ReadReg(0x00);
       DeviceCode= id<<16;
       
       id= SPI_ReadReg(0x01);
       DeviceCode= (DeviceCode |id<<8);
  
       id= SPI_ReadReg(0x02);
       DeviceCode= (DeviceCode |id);
       
       printf("ILI9806E_read_id() DeviceCode=0x%x\r\n",DeviceCode);
  
       SPI_WriteComm(0xfe);
       SPI_WriteData(0x00);
  
       return DeviceCode;
}


void lcd_init_cmd(void) 
{
	int nLoop = 0;

	//printf("CFG_LCD_HEIGHT:%d\n", CFG_LCD_HEIGHT);
   	LCD_InitGPIO();
#if 1
	ithGpioSet(RST_PIN);
	usleep(5000);
	ithGpioClear(RST_PIN);
	usleep(10*1000);
#endif
	ithGpioSet(RST_PIN);
	usleep(120*1000);
	
	//****************************** Page 1 Command ******************************//
	//****************************************************************************//
	SPI_WriteComm(0xFF);SPI_WriteData(0xFF);SPI_WriteData(0x98);SPI_WriteData(0x06);SPI_WriteData(0x04);SPI_WriteData(0x01);	 // Change to Page 1
	
	SPI_WriteComm(0x08);SPI_WriteData(0x10);				 // output SDA
	
	SPI_WriteComm(0x21);SPI_WriteData(0x01);				 // DE = 1 Active
	
	SPI_WriteComm(0x30);SPI_WriteData(0x00);				 // 480 X 854
	
	SPI_WriteComm(0x31);SPI_WriteData(0x02);				 // 2-dot Inversion
	
	SPI_WriteComm(0x40);SPI_WriteData(0x18);				// DDVDH/L BT 17 -10 5.94 -5.47 -2.3 1.6 43.4 21.88 0.17
	
	SPI_WriteComm(0x41);SPI_WriteData(0x66);			  // DDVDH/L CLAMP 
	
	SPI_WriteComm(0x42);SPI_WriteData(0x02);				 // VGH/VGL 
	
	SPI_WriteComm(0x43);SPI_WriteData(0x0A);				 // VGH/VGL 
	
	SPI_WriteComm(0x50);SPI_WriteData(0x78);				 // VGMP 4.6 
	
	SPI_WriteComm(0x51);SPI_WriteData(0x78);				 // VGMN 4.6 
	
	SPI_WriteComm(0x52);SPI_WriteData(0x00);				 // Flicker 
	
	SPI_WriteComm(0x53);SPI_WriteData(0x1F);				 // Flicker 
	
	SPI_WriteComm(0x57);SPI_WriteData(0x50);
	
	SPI_WriteComm(0x60);SPI_WriteData(0x07);				 // SDTI
	
	SPI_WriteComm(0x61);SPI_WriteData(0x01);				 // CRTI 00
	
	SPI_WriteComm(0x62);SPI_WriteData(0x07);				 // EQTI 07
	
	SPI_WriteComm(0x63);SPI_WriteData(0x00);				 // PCTI  00
	
	
	//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	SPI_WriteComm(0xFF);SPI_WriteData(0xFF);SPI_WriteData(0x98);SPI_WriteData(0x06);SPI_WriteData(0x04);SPI_WriteData(0x01);	 // Change to Page 1
	
	SPI_WriteComm(0xA0);SPI_WriteData(0x00);  // Gamma 0 
	
	SPI_WriteComm(0xA1);SPI_WriteData(0x0A);  // Gamma 4 
	
	SPI_WriteComm(0xA2);SPI_WriteData(0x12);  // Gamma 8
	
	SPI_WriteComm(0xA3);SPI_WriteData(0x0D);  // Gamma 16
	
	SPI_WriteComm(0xA4);SPI_WriteData(0x07);  // Gamma 24
	
	SPI_WriteComm(0xA5);SPI_WriteData(0x09);  // Gamma 52
	
	SPI_WriteComm(0xA6);SPI_WriteData(0x06); // Gamma 80
	
	SPI_WriteComm(0xA7);SPI_WriteData(0x03);  // Gamma 108
	
	SPI_WriteComm(0xA8);SPI_WriteData(0x09);  // Gamma 147
	
	SPI_WriteComm(0xA9);SPI_WriteData(0x0D);  // Gamma 175
	
	SPI_WriteComm(0xAA);SPI_WriteData(0x15);  // Gamma 203
	
	SPI_WriteComm(0xAB);SPI_WriteData(0x07);  // Gamma 231
	
	SPI_WriteComm(0xAC);SPI_WriteData(0x0D);  // Gamma 239
	
	SPI_WriteComm(0xAD);SPI_WriteData(0x0D);  // Gamma 247
	
	SPI_WriteComm(0xAE);SPI_WriteData(0x06);  // Gamma 251
	
	SPI_WriteComm(0xAF);SPI_WriteData(0x00);  // Gamma 255
	
	///==============Nagitive
	SPI_WriteComm(0xC0);SPI_WriteData(0x00);  // Gamma 0 
	
	SPI_WriteComm(0xC1);SPI_WriteData(0x0A);  // Gamma 4
	
	SPI_WriteComm(0xC2);SPI_WriteData(0x12);  // Gamma 8
	
	SPI_WriteComm(0xC3);SPI_WriteData(0x0E);  // Gamma 16
	
	SPI_WriteComm(0xC4);SPI_WriteData(0x07);  // Gamma 24
	
	SPI_WriteComm(0xC5);SPI_WriteData(0x09);  // Gamma 52
	
	SPI_WriteComm(0xC6);SPI_WriteData(0x07);  // Gamma 80
	
	SPI_WriteComm(0xC7);SPI_WriteData(0x03);  // Gamma 108
	
	SPI_WriteComm(0xC8);SPI_WriteData(0x09);  // Gamma 147
	
	SPI_WriteComm(0xC9);SPI_WriteData(0x0C);  // Gamma 175
	
	SPI_WriteComm(0xCA);SPI_WriteData(0x15);  // Gamma 203
	
	SPI_WriteComm(0xCB);SPI_WriteData(0x07);  // Gamma 231
	
	SPI_WriteComm(0xCC);SPI_WriteData(0x0C);  // Gamma 239
	
	SPI_WriteComm(0xCD);SPI_WriteData(0x0D); // Gamma 247
	
	SPI_WriteComm(0xCE);SPI_WriteData(0x06); // Gamma 251
	
	SPI_WriteComm(0xCF);SPI_WriteData(0x00);  // Gamma 255
	
	
	//****************************************************************************//
	//****************************** Page 6 Command ******************************//
	//****************************************************************************//
	SPI_WriteComm(0xFF);SPI_WriteData(0xFF);SPI_WriteData(0x98);SPI_WriteData(0x06);SPI_WriteData(0x04);SPI_WriteData(0x06);	 // Change to Page 6
	
	SPI_WriteComm(0x00);SPI_WriteData(0x21);
	
	SPI_WriteComm(0x01);SPI_WriteData(0x06);
	
	SPI_WriteComm(0x02);SPI_WriteData(0xA0);   
	
	SPI_WriteComm(0x03);SPI_WriteData(0x02);
	
	SPI_WriteComm(0x04);SPI_WriteData(0x01);
	
	SPI_WriteComm(0x05);SPI_WriteData(0x01);
	
	SPI_WriteComm(0x06);SPI_WriteData(0x80);   
	
	SPI_WriteComm(0x07);SPI_WriteData(0x04);
	
	SPI_WriteComm(0x08);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x09);SPI_WriteData(0x80);	
	
	SPI_WriteComm(0x0A);SPI_WriteData(0x00);	
	
	SPI_WriteComm(0x0B);SPI_WriteData(0x00);	
	
	SPI_WriteComm(0x0C);SPI_WriteData(0x2c);
	
	SPI_WriteComm(0x0D);SPI_WriteData(0x2c);
	
	SPI_WriteComm(0x0E);SPI_WriteData(0x1c);
	
	SPI_WriteComm(0x0F);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x10);SPI_WriteData(0xFF);
	
	SPI_WriteComm(0x11);SPI_WriteData(0xF0);
	
	SPI_WriteComm(0x12);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x13);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x14);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x15);SPI_WriteData(0xC0);
	
	SPI_WriteComm(0x16);SPI_WriteData(0x08);
	
	SPI_WriteComm(0x17);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x18);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x19);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x1A);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x1B);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x1C);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x1D);SPI_WriteData(0x00);
	
	
	SPI_WriteComm(0x20);SPI_WriteData(0x01);
	
	SPI_WriteComm(0x21);SPI_WriteData(0x23);
	
	SPI_WriteComm(0x22);SPI_WriteData(0x45);
	
	SPI_WriteComm(0x23);SPI_WriteData(0x67);
	
	SPI_WriteComm(0x24);SPI_WriteData(0x01);
	
	SPI_WriteComm(0x25);SPI_WriteData(0x23);
	
	SPI_WriteComm(0x26);SPI_WriteData(0x45);
	
	SPI_WriteComm(0x27);SPI_WriteData(0x67);
	
	
	SPI_WriteComm(0x30);SPI_WriteData(0x12);
	
	SPI_WriteComm(0x31);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x32);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x33);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x34);SPI_WriteData(0x87);
	
	SPI_WriteComm(0x35);SPI_WriteData(0x96);
	
	SPI_WriteComm(0x36);SPI_WriteData(0xAA);
	
	SPI_WriteComm(0x37);SPI_WriteData(0xDB);
	
	SPI_WriteComm(0x38);SPI_WriteData(0xCC);
	
	SPI_WriteComm(0x39);SPI_WriteData(0xBD);
	
	SPI_WriteComm(0x3A);SPI_WriteData(0x78);
	
	SPI_WriteComm(0x3B);SPI_WriteData(0x69);
	
	SPI_WriteComm(0x3C);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x3D);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x3E);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x3F);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x40);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x52);SPI_WriteData(0x10);
	
	SPI_WriteComm(0x53);SPI_WriteData(0x10);
	
	
	SPI_WriteComm(0xFF);SPI_WriteData(0xFF);SPI_WriteData(0x98);SPI_WriteData(0x06);SPI_WriteData(0x04);SPI_WriteData(0x07);	// Change to Page 0
	
	SPI_WriteComm(0x17);SPI_WriteData(0x22);
	
	SPI_WriteComm(0x18);SPI_WriteData(0x1D);
	
	SPI_WriteComm(0x02);SPI_WriteData(0x77);
	
	SPI_WriteComm(0xE1);SPI_WriteData(0x79);
	
	SPI_WriteComm(0x06);SPI_WriteData(0x11);
	
	
	//****************************************************************************//
	SPI_WriteComm(0xFF);SPI_WriteData(0xFF);SPI_WriteData(0x98);SPI_WriteData(0x06);SPI_WriteData(0x04);SPI_WriteData(0x00);	 // Change to Page 0
	
	SPI_WriteComm(0x3A);SPI_WriteData(0x77);
	SPI_WriteComm(0x20);SPI_WriteData(0x00);
	
	SPI_WriteComm(0x11);SPI_WriteData(0x00);				 // Sleep-Out
	usleep(120);
	SPI_WriteComm(0x29);SPI_WriteData(0x00);				 // Display On
} 
#endif

void* TestFunc(void* arg)
{
    uint16_t* addr;
    uint32_t col, row, x, y, i = 0;
    const uint16_t colors[] = { ITH_RGB565(255, 0, 0), ITH_RGB565(0, 255, 0), ITH_RGB565(0, 0, 255) };

    itpInit();
#if defined(SPI_INIT)
	lcd_init_cmd();
#endif
#if defined(LCD_SPI_INIT_ENALBE)
	InitLcdSPIData();
	sleep(2);
#endif

    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);

    addr = (uint16_t*) ithLcdGetBaseAddrA();
    col = ithLcdGetPitch() / 2;
    row = ithLcdGetHeight();

    for (;;)
    {
        uint16_t* base = ithMapVram((uint32_t) addr, ithLcdGetPitch() * ithLcdGetHeight(), ITH_VRAM_WRITE);
				uint16_t color = colors[i++ % ITH_COUNT_OF(colors)];
        uint16_t* ptr = base;

        for (y = 0; y < row; y++)
            for (x = 0; x < col; x++)
                *ptr++ = color;
        
        ithFlushDCacheRange(base, row * col * 2);
        sleep(5);
        printf("go=%d\n", i);
		
#if defined(SPI_INIT)
		//lcd_init_cmd();
#endif
    }
    return NULL;
}

