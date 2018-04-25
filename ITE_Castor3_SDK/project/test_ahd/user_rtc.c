#include "user_function.h"

bool user_rtc_init = false;

timer_t rtc_update_TimerId;		//RTC更新时间
#define	 rtc_update_Time		180
struct tm tInfo;
uint8_t pt7c4337_i2c_read(uint8_t slaveAddr,uint8_t regAddr)
{
	uint8_t val;
	mmpIicReceiveData(IIC_PORT_1, IIC_MASTER_MODE, slaveAddr>>1, &regAddr, 1,&val, 1);
	usleep(10);
	return val;
}
void pt7c4337_i2c_write(uint8_t slaveAddr,uint8_t regAddr,uint8_t val)
{
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, slaveAddr>>1, regAddr, &val,1);
	usleep(10);
	return;
}

static uint8_t ITE_RTC(uint8_t byte)
{
	uint8_t B1 = (byte>>4);
	uint8_t B2 = (byte&0xF);

	return (B1*10) + B2;
}

static uint8_t RTC_ITE(uint8_t dec)
{
	uint8_t B1 = dec/10;
	uint8_t B2 = dec%10;

	return (B1<<4) | B2;
}

static long UserRtcGetTime(void)
{
	uint8_t data[7];
	long rtcSec=0;
	int i;
	for(i = 0; i < 0x7; i++)
	{
		data[i] = pt7c4337_i2c_read(0xD0,0x00+i);
		//printf("----------------------->0x%x\n",data[i]);
	}

	tInfo.tm_sec = RTC_ITE(data[0] & 0x7F);	//sec:0~59
	tInfo.tm_min = RTC_ITE(data[1] & 0x7F);	//min:0~59
	tInfo.tm_hour = RTC_ITE(data[2] & 0x3F);	//hour:0~23
	tInfo.tm_wday = (data[3] & 0x07) - 1;			//week:1~7 (1:sun, 2:mon, 3:tue,..., 7:sat)(but linux week define is 0~6)
	tInfo.tm_mday = RTC_ITE(data[4] & 0x3F);	//day:1~31 (without 0)
	tInfo.tm_mon = RTC_ITE(data[5] & 0x1F) - 1;//mon:1~12 (1:jan, 2:feb, 3:mar..., 12:Dec)(without 0, but linux month define is 0~11)
	tInfo.tm_year = RTC_ITE(data[6])+100;	//year:2000~2099
	if (data[5] & 0x80)
	{
		tInfo.tm_year += 100;					//reg05 bit7 0&1 (0:year=19XX, 1:year=20XX)
	}

	rtcSec = mktime((struct tm*)&tInfo);
	return rtcSec;
}

void UserRtcSetTime(long sec, long usec)
{
	uint8_t data[7];
	int     i;
	struct  tm *t = localtime(&sec);

	ithEnterCritical();
#if 1
	memset(data, 0, sizeof(data));

	data[0] = ITE_RTC(t->tm_sec);
	data[1] = ITE_RTC(t->tm_min);
	data[2] = ITE_RTC(t->tm_hour);
	data[3] = t->tm_wday%7 + 1;
	data[4] = ITE_RTC(t->tm_mday);
	data[5] = ITE_RTC(t->tm_mon + 1);
	data[6] = ITE_RTC(t->tm_year % 100);
	if(t->tm_year > 199)
		data[5] |= 0x80;
	else
		data[5] &= 0x7F;

	for(i = 0; i < 7; i++)
		pt7c4337_i2c_write(0xD0,0x00+i,data[i]);
#endif 
	ithExitCritical();
}

void rtc_update_start()
{
	struct itimerspec value;

	value.it_value.tv_sec   = rtc_update_Time;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(rtc_update_TimerId, 0, &value, NULL);
}

void rtc_update_end(timer_t timerid, int arg)
{
	printf("update system time------------------------------------>\n");
	//assistRtcSetTime(UserRtcGetTime(),0);
	rtc_update_start();
}

void _user_rtc_init()
{
	timer_create(CLOCK_REALTIME, NULL, &rtc_update_TimerId);
	timer_connect(rtc_update_TimerId, rtc_update_end, 0);

	if(theConfig.firstboot)
	{
		UserRtcSetTime(CFG_RTC_DEFAULT_TIMESTAMP,0);
		theConfig.firstboot = 0;
		ConfigSave();
	}
	//assistRtcSetTime(UserRtcGetTime(),0);
	rtc_update_start();
}




