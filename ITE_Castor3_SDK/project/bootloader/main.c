#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "ite/ite_usbex.h"
#include "bootloader.h"
#include "config.h"
#include "ite/itp.h"
#include "ite/itu.h"
#include "itu_private.h"

static bool blLcdOn = false;
static bool blLcdConsoleOn = false;
static bool stop_led = false;

#if defined (CFG_ENABLE_UART_CLI)
static bool boot = false;
static bool bootstate = false;
char tftppara[128] = "tftp://192.168.1.20/doorbell_indoor2.bin";

#if defined (CFG_UART0_ENABLE) && defined(CFG_DBG_UART0)
#define UART_PORT ":uart0"
#elif defined(CFG_UART1_ENABLE) && defined(CFG_DBG_UART1)
#define UART_PORT ":uart1"
#elif defined(CFG_UART2_ENABLE) && defined(CFG_DBG_UART2)
#define UART_PORT ":uart2"
#elif defined(CFG_UART3_ENABLE) && defined(CFG_DBG_UART3)
#define UART_PORT ":uart3"
#endif

#endif //end of #if defined (CFG_ENABLE_UART_CLI)

#ifdef CFG_MSC_ENABLE
static bool usbInited = false;
#endif

#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for TARGET_BOARD_G
#define TARGET_BOARD_G
#define TARGET_BOARD_G_V03

#if defined(TARGET_BOARD_G_V03)
#define CAM_VOL_PIN  	63//32
#define BL_VOL_PIN  	36//34
#define BL_GPIO_PIN  	64//35

#define PR2000_MPP3 	72//73
#define PR2000_MPP4 	73//72
#else
#define CAM_VOL_PIN  	32
#define BL_VOL_PIN  	34
#define BL_GPIO_PIN  	35

#define PR2000_MPP3 	73
#define PR2000_MPP4 	72
#endif


static void user_gpio_init()
{
	ithGpioSetMode(BL_GPIO_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_GPIO_PIN);
	ithGpioEnable(BL_GPIO_PIN); 
	ithGpioSet(BL_GPIO_PIN);	
	
	ithGpioSetMode(BL_VOL_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_VOL_PIN);
	ithGpioEnable(BL_VOL_PIN);			
	ithGpioSet(BL_VOL_PIN);
	
#if 0
	ithGpioSetMode(CAM_VOL_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(CAM_VOL_PIN);
	ithGpioEnable(CAM_VOL_PIN); 
	ithGpioSet(CAM_VOL_PIN);
	
	ithGpioSetMode(PR2000_MPP3, ITH_GPIO_MODE0);
	ithGpioSetOut(PR2000_MPP3);
	ithGpioEnable(PR2000_MPP3); 
	ithGpioClear(PR2000_MPP3);	

	ithGpioSetMode(PR2000_MPP4, ITH_GPIO_MODE0);
	ithGpioSetOut(PR2000_MPP4);
	ithGpioEnable(PR2000_MPP4); 
	ithGpioClear(PR2000_MPP4);
#endif
}
#endif

//#define LCD_SPI_INIT_ENALBE
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

	LCD_DelayMS(50);

	Write_COM(0xFF);Write_DATA(0xFF);Write_DATA(0x98);Write_DATA(0x06);Write_DATA(0x04);Write_DATA(0x01);     // Change to Page 1

	Write_COM(0x08);Write_DATA(0x10);                 // output SDA

	Write_COM(0x20);Write_DATA(0x01);                 // 0:DE mode,1:SYNC mode

	Write_COM(0x21);Write_DATA(1<<3|1<<2|1<<1|0<<0);                 // DE = 1 Active

	Write_COM(0x25);Write_DATA(0x14);				//VFP
	
	Write_COM(0x26);Write_DATA(0x14); 				//VBP

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

#define CS_H ithGpioSet(CS_PIN)
#define CS_L ithGpioClear(CS_PIN)
#define SCL_H ithGpioSet(SCL_PIN)
#define SCL_L ithGpioClear(SCL_PIN)
#define SDA_H ithGpioSet(SDA_PIN)
#define SDA_L ithGpioClear(SDA_PIN)

static void SPI_WriteComm(unsigned char reg)
{
	int i;
	unsigned short usValue = 0;
	unsigned short usMask = 0x100;

	usValue |= reg;//00reg
	ithPrintf("usValue:0x%02x, %s, %d.\n", usValue, __FUNCTION__, __LINE__);

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
	ithPrintf("usValue:0x%02x, %s, %d.\n", usValue, __FUNCTION__, __LINE__);

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


void lcd_init_cmd(void) 
{
	int nLoop = 0;

	printf("CFG_LCD_HEIGHT:%d\n", CFG_LCD_HEIGHT);
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


#ifdef CFG_UPGRADE_USB_DEVICE

static bool DetectUsbDeviceMode(void)
{
    bool ret = false;
    LOG_INFO "Detect USB device mode...\r\n" LOG_END
    
    // init usb
#if defined(CFG_USB0_ENABLE) || defined(CFG_USB1_ENABLE)
    itpRegisterDevice(ITP_DEVICE_USB, &itpDeviceUsb);
    if (ioctl(ITP_DEVICE_USB, ITP_IOCTL_INIT, NULL) != -1)
        usbInited = true;    
#endif

    // init usb device mode as mass storage device
#ifdef CFG_USBD_FILE_STORAGE
    if (usbInited)
    {
        int timeout = CFG_UPGRADE_USB_DETECT_TIMEOUT;
        
        itpRegisterDevice(ITP_DEVICE_USBDFSG, &itpDeviceUsbdFsg);
        ioctl(ITP_DEVICE_USBDFSG, ITP_IOCTL_INIT, NULL);
        
        while (!ret)
        {
            if (ioctl(ITP_DEVICE_USBDFSG, ITP_IOCTL_IS_CONNECTED, NULL))
            {
                ret = true;
                break;
            }

            timeout -= 10;
            if (timeout <= 0)
            {
                LOG_INFO "USB device mode not connected.\n" LOG_END
                break;
            }
            usleep(10000);
        }
    }
#endif // CFG_USBD_FILE_STORAGE
    return ret;
}
#endif // CFG_UPGRADE_USB_DEVICE

static void InitFileSystem(void)
{
    // init card device
#if  !defined(_WIN32) && (defined(CFG_SD0_ENABLE) || defined(CFG_SD1_ENABLE) || defined(CFG_CF_ENABLE) || defined(CFG_MS_ENABLE) || defined(CFG_XD_ENABLE) || defined(CFG_MSC_ENABLE) || defined(CFG_RAMDISK_ENABLE))
    itpRegisterDevice(ITP_DEVICE_CARD, &itpDeviceCard);
    ioctl(ITP_DEVICE_CARD, ITP_IOCTL_INIT, NULL);
#endif

    // init usb
#ifdef CFG_MSC_ENABLE
    if (!usbInited)
    {
        itpRegisterDevice(ITP_DEVICE_USB, &itpDeviceUsb);
        if (ioctl(ITP_DEVICE_USB, ITP_IOCTL_INIT, NULL) != -1)
            usbInited = true;
    }
#endif

    // init fat
#ifdef CFG_FS_FAT
    itpRegisterDevice(ITP_DEVICE_FAT, &itpFSDeviceFat.dev);
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_ENABLE, NULL);
#endif

    // init drive table
#if defined(CFG_FS_FAT) || defined(CFG_FS_NTFS)
    itpRegisterDevice(ITP_DEVICE_DRIVE, &itpDeviceDrive);
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_INIT, NULL);

#ifdef CFG_MSC_ENABLE
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_ENABLE, NULL);
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_INIT_TASK, NULL);
#endif // CFG_MSC_ENABLE
#endif // defined(CFG_FS_FAT) || defined(CFG_FS_NTFS)

    // mount disks on booting
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_MOUNT, NULL);

#ifdef CFG_MSC_ENABLE
    // wait msc is inserted
    if (usbInited)
    {
        ITPDriveStatus* driveStatusTable;
        ITPDriveStatus* driveStatus = NULL;
        int i, timeout = CFG_UPGRADE_USB_DETECT_TIMEOUT;
        bool found = false;
        
        while (!found)
        {
            for (i = 0; i < 1; i++)
            {
                if (ioctl(ITP_DEVICE_USB, ITP_IOCTL_IS_CONNECTED, (void*)(USB0 + i)))
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                break;
            }
            else
            {
                timeout -= 10;
                if (timeout <= 0)
                {
                    LOG_INFO "USB device not found.\n" LOG_END
                    return;
                }
                usleep(10000);
            }
        }

        found = false;
        timeout = CFG_UPGRADE_USB_TIMEOUT;

        ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);
        
        while (!found)
        {
            for (i = 0; i < ITP_MAX_DRIVE; i++)
            {
                driveStatus = &driveStatusTable[i];
                if (driveStatus->disk >= ITP_DISK_MSC00 && driveStatus->disk <= ITP_DISK_MSC17 && driveStatus->avail)
                {
                    LOG_DBG "drive[%d]:usb disk=%d\n", i, driveStatus->disk LOG_END
                    found = true;
                }
            }
            if (!found)
            {
                timeout -= 100;
                if (timeout <= 0)
                {
                    LOG_INFO "USB disk not found.\n" LOG_END
                    break;
                }
                usleep(100000);
            }
        }
    }
#endif // CFG_MSC_ENABLE
}

#if !defined(CFG_LCD_ENABLE) && defined(CFG_CHIP_PKG_IT9910)
static void* UgLedTask(void* arg)
{
    int gpio_pin = 20;
    ithGpioSetOut(21);
    ithGpioSetMode(21,ITH_GPIO_MODE0);
    ithGpioSetOut(20);
    ithGpioSetMode(20,ITH_GPIO_MODE0);
	stop_led = false;
	
    for(;;)
    {
    	if(stop_led == true)
    	{
    		ithGpioSet(20); 
			ithGpioSet(21);
			while(1)
				usleep(500000);
    	}
        ithGpioClear(gpio_pin);
        if(gpio_pin==21)
            gpio_pin = 20;
        else
            gpio_pin = 21;
        ithGpioSet(gpio_pin); 
        usleep(500000);
    }
}
#endif

#if defined(CFG_CHIP_PKG_IT9910)
static void DetectKey(void)
{
    int ret;
    int phase = 0;
    int time_counter = 0;
    int key_counter = 0;
    bool key_pressed;
    bool key_released;
    ITPKeypadEvent ev;

    while (1)
    {
        key_pressed = key_released = false;
        ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_PROBE, NULL);
        if (read(ITP_DEVICE_KEYPAD, &ev, sizeof (ITPKeypadEvent)) == sizeof (ITPKeypadEvent))
        {
            if (ev.code == 0)
            {
                if (ev.flags & ITP_KEYPAD_DOWN)
                    key_pressed = true;
                else if (ev.flags & ITP_KEYPAD_UP)
                    key_released = true;
            }
        }

        if (phase == 0)
        {
            if (key_pressed)
            {
                printf("key detected\n");
                phase = 1;
            }
            else
                break;
        }
        else if (phase == 1)
        {
            if (key_released)
                break;
            if (time_counter > 100)
            {
                phase = 2;
                ithGpioSetOut(21);
                ithGpioSetMode(21, ITH_GPIO_MODE0);
                ithGpioSetOut(20);
                ithGpioSetMode(20, ITH_GPIO_MODE0);
            }
        }
        else if (phase == 2)
        {
            if (key_pressed)
            {
                ithGpioSet(20);
                key_counter++;
            }
            if (key_released)
                ithGpioClear(20);

            if (time_counter > 200)
            {
                ithGpioSet(21);
                ithGpioClear(20);
                phase = 3;
            }

            // blink per 6*50000 us
            if ((time_counter/6)%2)
                ithGpioSet(21);
            else
                ithGpioClear(21);
        }
        else if (phase == 3)
        {
            printf("key_counter: %d\n", key_counter);
            if (key_counter == 1)
            {
                // do reset
                InitFileSystem();
                ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_MOUNT, NULL);
                ret = ugResetFactory();
				#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
				ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
				#endif
                exit(ret);
            }
            if (key_counter == 2)
            {
                // dump addressbook.xml
                InitFileSystem();
                ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_MOUNT, NULL);
                CopyUclFile();
            }
            ithGpioClear(21);
            break;
        }

        usleep(50000);
        time_counter++;
    }
}
#endif

#if defined (CFG_ENABLE_UART_CLI)
static int parseCommand(char* str, int strlength)
{

	int ret;
	
	if (strncmp(str, "boot", 4) == 0)
	{
		printf("going to boot procedure\n");
		boot = true;
	}

	if (strncmp(str, "update", 6) == 0)
	{
		ITCStream* upgradeFile;
		uint8_t* imagebuf;
		uint32_t imagesize;

		upgradeFile = OpenRecoveryPackage();	
		if (upgradeFile)
		{			
			BootBin(upgradeFile);
			return 1;
		}
	}

	if (strncmp(str, "upgrade", 7) == 0)
	{
		ITCStream* upgradeFile;
		
		upgradeFile = OpenRecoveryPackage();
        if (upgradeFile)
        {
            if (ugCheckCrc(upgradeFile, NULL))
                LOG_ERR "Upgrade failed.\n" LOG_END
            else
                ret = ugUpgradePackage(upgradeFile);

            #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                LOG_INFO "Flushing NOR cache...\n" LOG_END
                ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
            #endif

            if (ret)
                LOG_INFO "Upgrade failed.\n" LOG_END
            else
                LOG_INFO "Upgrade finished.\n" LOG_END

            exit(ret);
        }
        else
        {
        #ifdef CFG_UPGRADE_RECOVERY_LED
            ioctl(fd, ITP_IOCTL_OFF, NULL);
        #endif
        }
		while (1);
	}

	if (strncmp(str, "setenv", 6) == 0)
	{
		char para[128] = {0};
		int i = 0;
		
		strncpy(para, str+7, strlength -7);		
		memset(tftppara, 0 , 128);
		strcpy(tftppara, para);
		printf("\ntftppara=%s\n", tftppara);
	}

	
	return 0;
}

static void CommandReciver(void)
{
	char rcvbuf[128], cmdbuf[128];
	static int wp = 0;
	int fd, len, i, strlength;	

#if defined (CFG_UART0_ENABLE) && defined(CFG_DBG_UART0)
	itpRegisterDevice(ITP_DEVICE_UART0, &itpDeviceUart0);
	ioctl(ITP_DEVICE_UART0, ITP_IOCTL_RESET, (void*)CFG_UART0_BAUDRATE);
#elif defined (CFG_UART1_ENABLE) && defined(CFG_DBG_UART1)
	itpRegisterDevice(ITP_DEVICE_UART1, &itpDeviceUart1);
	ioctl(ITP_DEVICE_UART1, ITP_IOCTL_RESET, (void*)CFG_UART1_BAUDRATE);
#elif defined (CFG_UART2_ENABLE) && defined(CFG_DBG_UART2)
	itpRegisterDevice(ITP_DEVICE_UART2, &itpDeviceUart2);
	ioctl(ITP_DEVICE_UART2, ITP_IOCTL_RESET, (void*)CFG_UART2_BAUDRATE);
#elif defined (CFG_UART3_ENABLE) && defined(CFG_DBG_UART3)
	itpRegisterDevice(ITP_DEVICE_UART3, &itpDeviceUart3);
	ioctl(ITP_DEVICE_UART3, ITP_IOCTL_RESET, (void*)CFG_UART3_BAUDRATE);
#endif

	fd = open(UART_PORT, O_RDONLY);
	
	if(fd < 0)	
		return;

	for(;;)
	{
	
		memset(rcvbuf, 0, 128);
		len = read(fd, rcvbuf, 128);

		if (len)
		{
			for (i = 0; i < len; i++)
			{			
				cmdbuf[wp] = rcvbuf[i];								
				wp++;					
				if (rcvbuf[i] == '\0')
				{					
					strlength = strlen(cmdbuf);					
					parseCommand(cmdbuf, strlength);
					memset(cmdbuf, 0, 128);
					wp = 0;						
				}		
			}
		}
		if(boot)
			break;
			
	}
	printf("Exit CommandReciver\n");
}
#endif
static void DoUpgrade(void)
{
    ITCStream* upgradeFile;

    LOG_INFO "Do Upgrade...\r\n" LOG_END

    upgradeFile = OpenUpgradePackage();
    if (upgradeFile)
    {
        int ret;

    #if !defined(CFG_LCD_ENABLE) && defined(CFG_CHIP_PKG_IT9910)
        //---light on red/green led task
        pthread_t task;
        pthread_create(&task, NULL, UgLedTask, NULL);
        //------
    #endif    
        // output messages to LCD console
    #if defined(CFG_LCD_ENABLE) && defined(CFG_BL_LCD_CONSOLE)
        if (!blLcdOn)
        {
        #if !defined(CFG_BL_SHOW_LOGO)
            extern uint32_t __lcd_base_a;
            extern uint32_t __lcd_base_b;
            extern uint32_t __lcd_base_c;
        
            itpRegisterDevice(ITP_DEVICE_SCREEN, &itpDeviceScreen);
            ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESET, NULL);
            ithLcdSetBaseAddrA((uint32_t) &__lcd_base_a);
            ithLcdSetBaseAddrB((uint32_t) &__lcd_base_b);        
        
        #ifdef CFG_BACKLIGHT_ENABLE
            itpRegisterDevice(ITP_DEVICE_BACKLIGHT, &itpDeviceBacklight);
            ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_INIT, NULL);
        #endif // CFG_BACKLIGHT_ENABLE        
        
        #endif // !defined(CFG_BL_SHOW_LOGO)
            
            ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
            ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
            blLcdOn = true;
        }
        if (!blLcdConsoleOn)
        {
            itpRegisterDevice(ITP_DEVICE_STD, &itpDeviceLcdConsole);
            itpRegisterDevice(ITP_DEVICE_LCDCONSOLE, &itpDeviceLcdConsole);
            ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_INIT, NULL);
            ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_CLEAR, NULL);
            blLcdConsoleOn = true;
        }
    #endif // defined(CFG_LCD_ENABLE) && defined(BL_LCD_CONSOLE)

        if (ugCheckCrc(upgradeFile, NULL))
            LOG_ERR "Upgrade failed.\n" LOG_END
        else
            ret = ugUpgradePackage(upgradeFile);

    #ifdef CFG_UPGRADE_DELETE_PKGFILE_AFTER_FINISH
        DeleteUpgradePackage();
    #endif

        #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
            LOG_INFO "Flushing NOR cache...\n" LOG_END
            ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
        #endif

        if (ret)
            LOG_INFO "Upgrade failed.\n" LOG_END
        else
        {
        	stop_led = true;
            LOG_INFO "Upgrade finished.\n" LOG_END
        }
    #if defined(CFG_UPGRADE_DELAY_AFTER_FINISH) && CFG_UPGRADE_DELAY_AFTER_FINISH > 0
        sleep(CFG_UPGRADE_DELAY_AFTER_FINISH);
    #endif

        exit(ret);
        while (1);
    }
}

void* BootloaderMain(void* arg)
{
    int ret;

#if defined(CFG_UPGRADE_PRESSKEY) || defined(CFG_UPGRADE_RESET_FACTORY) || defined(CFG_UPGRADE_RECOVERY)
    ITPKeypadEvent ev;
#endif

#ifdef CFG_WATCHDOG_ENABLE
    ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_DISABLE, NULL);
#endif

    ithMemDbgDisable(ITH_MEMDBG0);
    ithMemDbgDisable(ITH_MEMDBG1);

#if defined(SPI_INIT)
	ithEnterCritical();
	lcd_init_cmd();
	ithExitCritical();
#endif
#if defined(LCD_SPI_INIT_ENALBE)
	InitLcdSPIData();
#endif

#ifdef CFG_BL_SHOW_LOGO
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ShowLogo();
#ifdef CFG_BACKLIGHT_ENABLE
    usleep(100000);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
#endif // CFG_BACKLIGHT_ENABLE
    blLcdOn = true;
#endif // CFG_BL_SHOW_LOGO

#if defined(CFG_CAPTURE_MODULE_ENABLE)
#if defined(TARGET_BOARD_G)
	user_gpio_init();
#endif
#endif

#ifdef CFG_UPGRADE_USB_DEVICE
    if (DetectUsbDeviceMode())
    {
		ITCStream* upgradeFile = OpenUsbDevicePackage();
        if (upgradeFile)
        {
            if (ugCheckCrc(upgradeFile, NULL))
                LOG_ERR "Upgrade failed.\n" LOG_END
            else
                ret = ugUpgradePackage(upgradeFile);

            #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                LOG_INFO "Flushing NOR cache...\n" LOG_END
                ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
            #endif

            if (ret)
                LOG_INFO "Upgrade failed.\n" LOG_END
            else
                LOG_INFO "Upgrade finished.\n" LOG_END

            exit(ret);
        }
    }
#endif // CFG_UPGRADE_USB_DEVICE

#if defined(CFG_UPGRADE_PRESSKEY) || defined(CFG_UPGRADE_RESET_FACTORY) || defined(CFG_UPGRADE_RECOVERY)
    ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_PROBE, NULL);
    if (read(ITP_DEVICE_KEYPAD, &ev, sizeof (ITPKeypadEvent)) == sizeof (ITPKeypadEvent))
    {
        int key = ev.code, delay = 0;

#endif // defined(CFG_UPGRADE_PRESSKEY) || defined(CFG_UPGRADE_RESET_FACTORY) || defined(CFG_UPGRADE_RECOVERY)

    #ifdef CFG_UPGRADE_RECOVERY
        if (key == CFG_UPGRADE_RECOVERY_PRESSKEY_NUM)
        {
            struct timeval time = ev.time;

            // detect key pressed time
            for (;;)
            {
                if (ev.flags & ITP_KEYPAD_UP)
                    break;

                ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_PROBE, NULL);
                if (read(ITP_DEVICE_KEYPAD, &ev, sizeof (ITPKeypadEvent)) == 0)
                    continue;

                delay += itpTimevalDiff(&time, &ev.time);
                time = ev.time;

                LOG_DBG "recovery key: time=%ld.%ld,code=%d,down=%d,up=%d,repeat=%d,delay=%d\r\n",
                    ev.time.tv_sec,
                    ev.time.tv_usec / 1000,
                    ev.code,
                    (ev.flags & ITP_KEYPAD_DOWN) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_UP) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_REPEAT) ? 1 : 0,
                    delay
                LOG_END

                if (delay >= CFG_UPGRADE_RECOVERY_PRESSKEY_DELAY)
                    break;
            };

            if (delay >= CFG_UPGRADE_RECOVERY_PRESSKEY_DELAY)
            {
                ITCStream* upgradeFile;
            #ifdef CFG_UPGRADE_RECOVERY_LED
                int fd = open(":led:" CFG_UPGRADE_RECOVERY_LED_NUM, O_RDONLY);
                ioctl(fd, ITP_IOCTL_FLICKER, (void*)500);
            #endif

                // output messages to LCD console
            #if defined(CFG_LCD_ENABLE) && defined(CFG_BL_LCD_CONSOLE)
                if (!blLcdConsoleOn)
                {
                    itpRegisterDevice(ITP_DEVICE_STD, &itpDeviceLcdConsole);
                    itpRegisterDevice(ITP_DEVICE_LCDCONSOLE, &itpDeviceLcdConsole);
                    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_INIT, NULL);
                    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_CLEAR, NULL);
                    blLcdConsoleOn = true;
                }
                if (!blLcdOn)
                {
                    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
                    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
                    blLcdOn = true;
                }
            #endif // defined(CFG_LCD_ENABLE) && defined(BL_LCD_CONSOLE)

                LOG_INFO "Do Recovery...\r\n" LOG_END

                InitFileSystem();

                upgradeFile = OpenRecoveryPackage();
                if (upgradeFile)
                {
                    if (ugCheckCrc(upgradeFile, NULL))
                        LOG_ERR "Recovery failed.\n" LOG_END
                    else
                        ret = ugUpgradePackage(upgradeFile);

                    #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                        LOG_INFO "Flushing NOR cache...\n" LOG_END
                        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
                    #endif

                    if (ret)
                        LOG_INFO "Recovery failed.\n" LOG_END
                    else
                        LOG_INFO "Recovery finished.\n" LOG_END

                    exit(ret);
                }
                else
                {
                #ifdef CFG_UPGRADE_RECOVERY_LED
                    ioctl(fd, ITP_IOCTL_OFF, NULL);
                #endif
                }
                while (1);
            }
        }
    #endif // CFG_UPGRADE_RECOVERY

    #ifdef CFG_UPGRADE_RESET_FACTORY
        if (key == CFG_UPGRADE_RESET_FACTORY_PRESSKEY_NUM)
        {
            struct timeval time = ev.time;

            // detect key pressed time
            for (;;)
            {
                if (ev.flags & ITP_KEYPAD_UP)
                    break;

                ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_PROBE, NULL);
                if (read(ITP_DEVICE_KEYPAD, &ev, sizeof (ITPKeypadEvent)) == 0)
                    continue;

                delay += itpTimevalDiff(&time, &ev.time);
                time = ev.time;

                LOG_DBG "reset key: time=%ld.%ld,code=%d,down=%d,up=%d,repeat=%d,delay=%d\r\n",
                    ev.time.tv_sec,
                    ev.time.tv_usec / 1000,
                    ev.code,
                    (ev.flags & ITP_KEYPAD_DOWN) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_UP) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_REPEAT) ? 1 : 0,
                    delay
                LOG_END

                if (delay >= CFG_UPGRADE_RESET_FACTORY_PRESSKEY_DELAY)
                    break;
            };

            if (delay >= CFG_UPGRADE_RESET_FACTORY_PRESSKEY_DELAY)
            {
                // output messages to LCD console
            #if defined(CFG_LCD_ENABLE) && defined(CFG_BL_LCD_CONSOLE)
                if (!blLcdConsoleOn)
                {
                    itpRegisterDevice(ITP_DEVICE_STD, &itpDeviceLcdConsole);
                    itpRegisterDevice(ITP_DEVICE_LCDCONSOLE, &itpDeviceLcdConsole);
                    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_INIT, NULL);
                    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_CLEAR, NULL);
                    blLcdConsoleOn = true;
                }
                if (!blLcdOn)
                {
                    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
                    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
                    blLcdOn = true;
                }
            #endif // defined(CFG_LCD_ENABLE) && defined(BL_LCD_CONSOLE)

                LOG_INFO "Do Reset...\r\n" LOG_END

                InitFileSystem();
                ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_MOUNT, NULL);

                ret = ugResetFactory();

            #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                LOG_INFO "Flushing NOR cache...\n" LOG_END
                ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
            #endif

                if (ret)
                    LOG_INFO "Reset failed.\n" LOG_END
                else
                    LOG_INFO "Reset finished.\n" LOG_END

                exit(ret);
                while (1);
            }
        }
    #endif // CFG_UPGRADE_RESET_FACTORY

    #ifdef CFG_UPGRADE_PRESSKEY
        if (key == CFG_UPGRADE_PRESSKEY_NUM)
        {
            struct timeval time = ev.time;

            // detect key pressed time
            for (;;)
            {
                if (ev.flags & ITP_KEYPAD_UP)
                    break;

                ioctl(ITP_DEVICE_KEYPAD, ITP_IOCTL_PROBE, NULL);
                if (read(ITP_DEVICE_KEYPAD, &ev, sizeof (ITPKeypadEvent)) == 0)
                    continue;

                delay += itpTimevalDiff(&time, &ev.time);
                time = ev.time;

                LOG_DBG "upgrade key: time=%ld.%ld,code=%d,down=%d,up=%d,repeat=%d,delay=%d\r\n",
                    ev.time.tv_sec,
                    ev.time.tv_usec / 1000,
                    ev.code,
                    (ev.flags & ITP_KEYPAD_DOWN) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_UP) ? 1 : 0,
                    (ev.flags & ITP_KEYPAD_REPEAT) ? 1 : 0,
                    delay
                LOG_END

                if (delay >= CFG_UPGRADE_PRESSKEY_DELAY)
                    break;
            };

            if (delay >= CFG_UPGRADE_PRESSKEY_DELAY)
            {
                InitFileSystem();
                DoUpgrade();
            }
        }
    #endif // CFG_UPGRADE_PRESSKEY
#if defined(CFG_UPGRADE_PRESSKEY) || defined(CFG_UPGRADE_RESET_FACTORY) || defined(CFG_UPGRADE_RECOVERY)
    }
#endif

#if defined(CFG_CHIP_PKG_IT9910)
	DetectKey();
#endif

#if !defined(CFG_UPGRADE_PRESSKEY) && defined(CFG_UPGRADE_OPEN_FILE)
    InitFileSystem();
    DoUpgrade();
#if defined (CFG_ENABLE_UART_CLI)	
	CommandReciver();
#endif
#endif

#ifdef CFG_BL_SHOW_VIDEO
    itpInit();
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);

    ituLcdInit(); //init itu
    
#ifdef CFG_M2D_ENABLE
    ituM2dInit();
#ifdef CFG_VIDEO_ENABLE
    ituFrameFuncInit();
#endif // CFG_VIDEO_ENABLE
#else
    ituSWInit();
#endif // CFG_M2D_ENABLE

    PlayVideo();
    WaitPlayVideoFinish();
#endif //CFG_BL_SHOW_VIDEO

    LOG_INFO "Do Booting...\r\n" LOG_END

    BootImage();

    return NULL;
}
/*
#if (CFG_CHIP_PKG_IT9079)
void
ithCodecPrintfWrite(
    char* string,
    int length)
{
    // this is a dummy function for linking with library itp_boot. (itp_swuart_codec.c)
}

#endif
*/
