#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"
#include "ite/ith.h"
#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"

static int clk = 0;
static int cnt = 0;

unsigned int PalGetClock(void)
{
    return xTaskGetTickCount();
}

unsigned long PalGetDuration(unsigned int clock)
{
    return (xTaskGetTickCount() - clock) / portTICK_PERIOD_MS;
}

void timer_isr(void* data)
{
    uint32_t timer = (uint32_t)data;
    
    ithPrintf("\tTimer%d Interrupt occur, clk=%d, IntrState=0x%08X, cnt=%d\n", timer + 1, PalGetDuration(clk), ithTimerGetIntrState(), ++cnt);
    //ithPrintf("T%d\n", timer+1);
    //ithPrintf("\tTimer%d isr, cnt=%d\n", timer + 1, ++cnt);
    //printf("\tTimer%d Interrupt occur, clk=%d, IntrState=0x%08X, cnt=%d\n", timer + 1, PalGetDuration(clk), ithTimerGetIntrState(), ++cnt);
    clk = PalGetClock();
    ithTimerClearIntr(timer);

}

void* TestFunc_count(void* arg)
{
    /* Basic test*/
    /*
    teset timer count us sleeptime
    up count the counter
    ithTimerGetTime() get time
    count sleeptime=1000000 use time2-time1
    test 10 time
    */
    int i;
    for(i=ITH_TIMER1; i<=ITH_TIMER8; i++)
    {        
    
        int status;
        int n = 0;
        unsigned int time1=0;
        unsigned int time2=0;
        int sleeptime=1000000;
    
        if (i == ITH_TIMER6){
            printf("###system clock Timer%d : we dont test timer%d\n",(i+1),(i+1));
            continue;
        }
    
    
        printf("Timer%d test start\n",(i+1));
    
        ithTimerReset(i);//reset timer
        ithTimerCtrlEnable(i, ITH_TIMER_UPCOUNT);//up count the counter
        ithTimerSetCounter(i, 0x0);//init counter to 0
        ithTimerSetLoad(i,0x0);//set reload

        printf("ithTimerGetCounter = %d\n",ithTimerGetCounter(i));
        ithTimerEnable(i);//enabel time
        while(1){
            time1=ithTimerGetTime(i);
            ithDelay(sleeptime);
            time2=ithTimerGetTime(i);
            printf("sleep interval = %d us ,timercounter = %d\n",time2-time1,ithTimerGetCounter(i));
            if(n==5)
                break;
            else
                n++;
        }
        ithTimerCtrlDisable(i, ITH_TIMER_EN);
        ithTimerReset(i);
    
        printf("###Timer%d test end\n",(i+1));
    
    }
    printf("=======end test======\n");
    return ;
 
}

void* TestFunc_timeout(void* arg)
{
    /* Basic test*/
    int status;
    int i = 0;
    int j = 0;
    int us = 0;
    int clock = 0;

    for (us=10000; us<=10000000; us*=10)
    {
        // Interrupt test
        printf("\n###### %dms timeout testing ######\n", us/1000);

        for(i=ITH_TIMER1; i<=ITH_TIMER8; i++)
        {
            if (/*i == ITH_TIMER1 || i == USEDTIMER || */i == ITH_TIMER6)
                continue;
                
            ithTimerReset(i);
            ithTimerClearIntr(i);
            ithTimerSetTimeout(i, us);
            clk = PalGetClock();
            ithTimerEnable(i);

            clock = PalGetClock();

            while(1)
            {
                status = ithTimerGetIntrState();
                if(status & 0x7<<(i*4))
                {
                    printf("[TIMER%d] intr gap time = %d ms status = 0x%08x \n",i+1, PalGetDuration(clock), status);
                    //clock = PalGetClock();
                    break;

                }
                //clock = PalGetClock();

                usleep(200);
            }

            printf("Timer%d leaving\n", i+1);
            ithTimerCtrlDisable(i, ITH_TIMER_EN);
            ithTimerReset(i);
            ithTimerClearIntr(i);

        }
    }
    printf("**********end***************\n");
}

#if 1
#define BEEP_PIN	34//63//22 //PACK beep ÊÇ63
#define BEEP_PIN_MODE	ITH_GPIO_MODE2//	ITH_GPIO_MODE3		// 
#define BEEP_PWM 		ITH_PWM6	//ITH_PWM1			// 
#define BEEP_FREQ			2700				// 
#define BEEP_DUTY_CYCLE	80//50					// 
#define BEEP_INTERVAL (10 * 1000000)        //10ms

#define BEEP_USE_SW_PWM
//#define SW_PWM_DUTYCYCLE_ADJUSTABLE

static timer_t beep_timer = 0;
struct itimerspec value_beep;
static uint8_t bBeepBusy = 0;

#if defined(BEEP_USE_SW_PWM)
static bool isGpioHigh = 0;
static pthread_mutex_t            beepPlayMutex;

#if defined(SW_PWM_DUTYCYCLE_ADJUSTABLE)
typedef struct {
    uint32_t TimerN;    //ITH_TIMER1~ITH_TIMER8
    uint32_t Periodus; //us
    uint32_t Valid;
    uint32_t Invalid;
    uint8_t  DutyCycleLevel;//0~100
}SWPWM;
static SWPWM *swpwm = {0};

void _TimerIsr(void* data)
{
    SWPWM *in  = (SWPWM*) data;
	//
    //ithPrintf("in->TimerN = %d in->Periodus = %d in->DutyCycleLevel = %d\n",in->TimerN,in->Periodus,in->DutyCycleLevel);
	if(isGpioHigh)
	{
		ithGpioClear(BEEP_PIN);
        ithTimerSetTimeout(in->TimerN, in->Invalid);
        //ithPrintf("in->DutyCycleLevel = %d swpwm->Valid = %d\n",in->Valid);
	}
	else
	{
		ithGpioSet(BEEP_PIN);
        ithTimerSetTimeout(in->TimerN, in->Valid);
        //ithPrintf("in->DutyCycleLevel = %d swpwm->Valid = %d\n",in->Invalid);
	}
	isGpioHigh = !isGpioHigh;
}

static void _TimerInit(SWPWM *SWpwm)
{
    printf("SWpwm->TimerN = %d SWpwm->Periodus = %d(us) SWpwm->DutyCycleLevel = %d\n",SWpwm->TimerN,SWpwm->Periodus,SWpwm->DutyCycleLevel);
	//initial GPIO
	ithGpioSetOut(BEEP_PIN);
	ithGpioSetMode(BEEP_PIN, ITH_GPIO_MODE0);
	
	// Reset Timer7
	ithTimerReset(SWpwm->TimerN);

	// Initialize Timer IRQ
	ithIntrDisableIrq(ITH_INTR_TIMER1+SWpwm->TimerN);
	ithIntrClearIrq(ITH_INTR_TIMER1+SWpwm->TimerN);
    
	// register Timer Handler to IRQ
	ithIntrRegisterHandlerIrq(ITH_INTR_TIMER1+SWpwm->TimerN, _TimerIsr, (void*)SWpwm);

	// set Timer IRQ to edge trigger
	ithIntrSetTriggerModeIrq(ITH_INTR_TIMER1+SWpwm->TimerN, ITH_INTR_EDGE);

	// set Timer IRQ to detect rising edge
	ithIntrSetTriggerLevelIrq(ITH_INTR_TIMER1+SWpwm->TimerN, ITH_INTR_HIGH_RISING);

	// Enable Timer IRQ
	ithIntrEnableIrq(ITH_INTR_TIMER1+SWpwm->TimerN);
    
    //set Timeout
    ithTimerSetTimeout(SWpwm->TimerN, SWpwm->Valid);
	//ithTimerCtrlEnable(ITH_TIMER2, ITH_TIMER_UPCOUNT);

	// set Timer periodic
	//ithTimerCtrlEnable(ITH_TIMER2, ITH_TIMER_PERIODIC);
	//ithTimerSetCounter(ITH_TIMER2, 0);
	//ithTimerSetMatch(ITH_TIMER2, configCPU_CLOCK_HZ / configTICK_RATE_HZ);

	ithTimerCtrlEnable(SWpwm->TimerN, ITH_TIMER_EN);
}

static void _TimerTerminate(SWPWM *SWpwm)
{
    ithTimerCtrlDisable(SWpwm->TimerN, ITH_TIMER_EN);
    ithTimerReset(SWpwm->TimerN);

    ithIntrDisableIrq(ITH_INTR_TIMER1+SWpwm->TimerN);
    ithIntrClearIrq(ITH_INTR_TIMER1+SWpwm->TimerN);
}

#else

void _TimerIsr(void* data)
{
    uint32_t timer = (uint32_t)data;
	//
	if(isGpioHigh)
	{
		ithGpioClear(BEEP_PIN);
	}
	else
	{
		ithGpioSet(BEEP_PIN);
	}
	isGpioHigh = !isGpioHigh;
}

static void _TimerInit(uint8_t timerNo, uint32_t freq, uint32_t duty)
{
	//initial GPIO
    int us = (1000000/freq)/2;//2.7KHz = 370us = one duty cycle, 1/2 duty cycle = 185us; 500 = 0.5ms
	ithGpioSetOut(BEEP_PIN);
	ithGpioSetMode(BEEP_PIN, ITH_GPIO_MODE0);
	
	// Reset Timer
	ithTimerReset(timerNo);

	// Initialize Timer IRQ
	ithIntrDisableIrq(ITH_INTR_TIMER1+timerNo);
	ithIntrClearIrq(ITH_INTR_TIMER1+timerNo);

	// register Timer Handler to IRQ
	ithIntrRegisterHandlerIrq(ITH_INTR_TIMER1+timerNo, _TimerIsr, (void*)0);

	// set Timer IRQ to edge trigger
	ithIntrSetTriggerModeIrq(ITH_INTR_TIMER1+timerNo, ITH_INTR_EDGE);

	// set Timer IRQ to detect rising edge
	ithIntrSetTriggerLevelIrq(ITH_INTR_TIMER1+timerNo, ITH_INTR_HIGH_RISING);

	// Enable Timer IRQ
	ithIntrEnableIrq(ITH_INTR_TIMER1+timerNo);

	ithTimerCtrlEnable(timerNo, ITH_TIMER_UPCOUNT);

	// set Timer periodic
	ithTimerCtrlEnable(timerNo, ITH_TIMER_PERIODIC);
	ithTimerSetCounter(timerNo, 0);
	ithTimerSetMatch(timerNo, us*(configCPU_CLOCK_HZ /(configTICK_RATE_HZ * 1000)));

	ithTimerCtrlEnable(timerNo, ITH_TIMER_EN);
}

static void _TimerTerminate(uint8_t timerNo)
{
	ithTimerCtrlDisable(timerNo, ITH_TIMER_EN);
}
#endif
#endif

static void BeepPlayHandler(timer_t timerid, int arg)
{
	printf("BeepPlayHandler:\r\n");
#if defined(BEEP_USE_SW_PWM)
#if defined(SW_PWM_DUTYCYCLE_ADJUSTABLE)
	_TimerTerminate(swpwm);
#else
	_TimerTerminate(BEEP_PWM);
#endif
    pthread_mutex_unlock(&beepPlayMutex);	
#else
   	ithPwmDisable(BEEP_PWM, BEEP_PIN);
#endif
	bBeepBusy = 0;
}

void BuzzerInit(void)
{
	BuzzerExit();

#if defined(BEEP_USE_SW_PWM)
    pthread_mutex_init(&beepPlayMutex, NULL);	
#else
	ithPwmInit(BEEP_PWM, BEEP_FREQ, BEEP_DUTY_CYCLE);		// beep init
	ithPwmReset(BEEP_PWM, BEEP_PIN, BEEP_PIN_MODE);
	ithPwmDisable(BEEP_PWM, BEEP_PIN);	
#endif

	beep_timer = 0;
	bBeepBusy = 0;
	timer_create(CLOCK_REALTIME, NULL, &beep_timer);
	timer_connect(beep_timer, BeepPlayHandler, 0);
}

void BuzzerExit(void)
{
	if(beep_timer)
	{
	    timer_delete(beep_timer);
	    beep_timer = 0;
		bBeepBusy = 0;
	}
#if defined(BEEP_USE_SW_PWM)
	pthread_mutex_destroy(&beepPlayMutex);
#endif
}

void BuzzerPlay(uint32_t msBeepInterval)
{
	if (!bBeepBusy&&beep_timer)
	{
		bBeepBusy = 1;
        value_beep.it_value.tv_sec   = 0;
        value_beep.it_value.tv_nsec  = 1000000 * msBeepInterval;			//BEEP_INTERVAL, 10ms
        value_beep.it_interval.tv_sec = value_beep.it_interval.tv_nsec = 0;		
		timer_settime(beep_timer, 0, &value_beep, NULL);
		
#if defined(BEEP_USE_SW_PWM)
		pthread_mutex_lock(&beepPlayMutex);
#if defined(SW_PWM_DUTYCYCLE_ADJUSTABLE)
		swpwm->TimerN		 = ITH_TIMER2;//Timer set
		swpwm->Periodus 	 = 1000000/BEEP_FREQ;//1000;	  //Periods set
		swpwm->DutyCycleLevel= BEEP_DUTY_CYCLE;		  //dutycucle set
		swpwm->Valid		 = swpwm->Periodus * swpwm->DutyCycleLevel / 100;
		swpwm->Invalid		 = swpwm->Periodus * (100-swpwm->DutyCycleLevel) / 100;
		_TimerInit(swpwm);
#else
		_TimerInit(BEEP_PWM, BEEP_FREQ, BEEP_DUTY_CYCLE);
#endif
#else
		ithPwmEnable(BEEP_PWM, BEEP_PIN,BEEP_PIN_MODE);
#endif
	}
}

#else
#define TEST_GOIO_PIN	34
static bool isGpioHigh = 0;
void _TimerIsr(void* data)
{
    uint32_t timer = (uint32_t)data;
	//
	if(isGpioHigh)
	{
		ithGpioClear(TEST_GOIO_PIN);
	}
	else
	{
		ithGpioSet(TEST_GOIO_PIN);
	}
	isGpioHigh = !isGpioHigh;
    //ithPrintf("isGpioHigh = %d\n", isGpioHigh);
#if 0    
    ithPrintf("\tTimer%d Interrupt occur, clk=%d, IntrState=0x%08X, cnt=%d\n", timer + 1, PalGetDuration(clk), ithTimerGetIntrState(), ++cnt);
    //ithPrintf("T%d\n", timer+1);
    //ithPrintf("\tTimer%d isr, cnt=%d\n", timer + 1, ++cnt);
    //printf("\tTimer%d Interrupt occur, clk=%d, IntrState=0x%08X, cnt=%d\n", timer + 1, PalGetDuration(clk), ithTimerGetIntrState(), ++cnt);
    clk = PalGetClock();
    ithTimerClearIntr(timer);
#endif

}

static void _TimerInit(void)
{
	//initial GPIO
    int us = 185;//0.5ms 
	ithGpioSetOut(TEST_GOIO_PIN);
	ithGpioSetMode(TEST_GOIO_PIN, ITH_GPIO_MODE0);
	
	// Reset Timer7
	ithTimerReset(ITH_TIMER2);

	// Initialize Timer IRQ
	ithIntrDisableIrq(ITH_INTR_TIMER2);
	ithIntrClearIrq(ITH_INTR_TIMER2);

	// register Timer Handler to IRQ
	ithIntrRegisterHandlerIrq(ITH_INTR_TIMER2, _TimerIsr, (void*)0);

	// set Timer IRQ to edge trigger
	ithIntrSetTriggerModeIrq(ITH_INTR_TIMER2, ITH_INTR_EDGE);

	// set Timer IRQ to detect rising edge
	ithIntrSetTriggerLevelIrq(ITH_INTR_TIMER2, ITH_INTR_HIGH_RISING);

	// Enable Timer IRQ
	ithIntrEnableIrq(ITH_INTR_TIMER2);

	ithTimerCtrlEnable(ITH_TIMER2, ITH_TIMER_UPCOUNT);

	// set Timer periodic
	ithTimerCtrlEnable(ITH_TIMER2, ITH_TIMER_PERIODIC);
	ithTimerSetCounter(ITH_TIMER2, 0);
	ithTimerSetMatch(ITH_TIMER2, us*(configCPU_CLOCK_HZ /(configTICK_RATE_HZ * 1000)));

	ithTimerCtrlEnable(ITH_TIMER2, ITH_TIMER_EN);
}

static void _TimerTerminate(void)
{
	ithTimerCtrlDisable(ITH_TIMER2, ITH_TIMER_EN);
}
#endif

void* TestFunc_IRQ(void* arg)
{
    int status;
    int i = 0;
    int us = 0;    
#if 1
#if defined(BEEP_USE_SW_PWM)
	BuzzerInit();

	printf("first BuzzerPlay\r\n");
	BuzzerPlay(10000);
	for(i = 0; i < 10; i++)
	{
		usleep(1000*1000);
	}
	printf("second BuzzerPlay\r\n");
	BuzzerPlay(60000);
#else
	printf("first counting\r\n");
	_TimerInit();//my.wei add for test
	for(i = 0; i < 10; i++)
	{
		usleep(1000*1000);
	}
	_TimerTerminate();

	for(i = 0; i < 5; i++)
	{
		usleep(1000*1000);
	}

	printf("second counting\r\n");
	_TimerInit();//my.wei add for test
	for(i = 0; i < 10; i++)
	{
		usleep(1000*1000);
	}
	_TimerTerminate();	
#endif	
#else
    for (us=10000; us<=1000000; us*=10)
    {
        // Interrupt test
        printf("\n###### %dms timeout testing ######\n", us/1000);
        for(i=ITH_TIMER1; i<=ITH_TIMER8; i++)
        {
            // timer used in : ITE9850
			// 1: stc : itp_stc.c
			// 2: free
			// 3: VCD_TIMER :itp_stats_tascvcd.c
			// 4: RTC in alpha :itp_rtc_assist.c
			// 5: backlight : itp_backlight.c
			// 6: Operation System
            // 7: free
            // 8: free
            if (/*i == ITH_TIMER1 || i == USEDTIMER || */i == ITH_TIMER6)
                continue;
                
            ithTimerReset(i);

            {
                // Initialize Timer IRQ
                ithIntrDisableIrq(ITH_INTR_TIMER1 + i);
                ithIntrClearIrq(ITH_INTR_TIMER1 + i);

                // register Timer Handler to IRQ
                ithIntrRegisterHandlerIrq(ITH_INTR_TIMER1 + i, timer_isr, (void*)i);

                // set Timer IRQ to edge trigger
                ithIntrSetTriggerModeIrq(ITH_INTR_TIMER1 + i, ITH_INTR_EDGE);

                // set Timer IRQ to detect rising edge
                ithIntrSetTriggerLevelIrq(ITH_INTR_TIMER1 + i, ITH_INTR_HIGH_RISING);

                // Enable Timer IRQ
                ithIntrEnableIrq(ITH_INTR_TIMER1 + i);
            }

            ithTimerSetTimeout(i, us);
            clk = PalGetClock();
            ithTimerCtrlEnable(i, ITH_TIMER_EN);

            usleep(us*10); // goal: trigger 10 times

            printf("###### Timer%d leaving\n", i+1);
            ithTimerCtrlDisable(i, ITH_TIMER_EN);
            ithTimerReset(i);

            ithIntrDisableIrq(ITH_INTR_TIMER1 + i);
            ithIntrClearIrq(ITH_INTR_TIMER1 + i);
        }
    }
#endif		
    printf("end IRQ TEST\n");
    
    return NULL;
}

void* TestFunc_PWM(void* arg)
{
    /*
    default set
    backlight gpio pin : 64
    GPIO mode : 2
    ITH_PWM : ITH_PWM5
    CFG_BACKLIGHT_DUTY_CYCLES = {95, 92, 88, 82, 75, 65, 55, 40, 25, 5}
    */
    unsigned int blDutyCycleTable[] = { CFG_BACKLIGHT_DUTY_CYCLES };  
    int gpio_pin = 64;
    int PWM = ITH_PWM5;
    int mode = 2;
    int count;

    itpInit();

    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);

    for(count = 0 ; count <= 100 ; count++){
        ithPwmSetDutyCycle(ITH_PWM5,blDutyCycleTable[count%10]);
        sleep(1);
        printf("ITH_PWM%d enable ,blDutyCycleTable = %d\n",ITH_PWM5+1,blDutyCycleTable[count%10]);
        ithPwmEnable(ITH_PWM5, gpio_pin, mode);
        sleep(1);
        printf("ITH_PWM%d disable\n",ITH_PWM5+1);
        ithPwmDisable(ITH_PWM5,gpio_pin);     
    }
    return NULL;
}
