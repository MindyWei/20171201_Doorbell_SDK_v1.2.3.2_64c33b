#include <assert.h>
#include "user_function.h"


static bool CallInited, CallQuit, CallLoaded;
static sem_t CallSem;
static pthread_t CallTask;

int CallLoad(int call_cmd)
{
    sem_post(&CallSem);
    return 0;
}

static void* CallLoadTask(void* arg)
{
    for (;;)
    {
        sem_wait(&CallSem);

        if (CallQuit)
            break;
	printf("callloadtask------------------------>\n");
	sleep(3);
        if (CallQuit)
            break;
    }
    return NULL;
}
static void* CallLoadTask(void* arg);

void _user_call_init(void)
{
    CallInited = CallQuit = CallLoaded = false;

    sem_init(&CallSem, 0, 0);

    pthread_create(&CallTask, NULL, CallLoadTask, NULL);

    CallInited = true;
}

/*	中斷 ISR 裡如有印訊息的需求，
	不能使用 printf，
	必須改用 ithPrintf，
	ithPrintf 才是 reentrant function。
*/
void *_call_int(int  val)	//中断事件
{
	event_call_s = val;					//事件标志
	
       test_tick = SDL_GetTicks();
	//g_videoPlayerIsFileEOF = true;
}

void call_event_1(unsigned int pin, void *arg)
{
	//ithPrintf("call_event_1...............................\n");
		_call_int(1);
}

void call_event_2(unsigned int pin, void *arg)
{
	//ithPrintf("call_event_2...............................\n");
		_call_int(2);
}

void user_call_init()
{
	int i = 0 ;
	
	for(i = 0; i < 2; i++)		//户外机CALL机检测GPIO初始化
	{
		ithGpioSetMode(cam_gpio[i][CALL], ITH_GPIO_MODE0);				//设置GPIO模式
		ithGpioSetIn(cam_gpio[i][CALL]);									//输入检测口
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_PULL_ENABLE);			//内部上拉(下拉)使能
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_PULL_UP);				//内部上拉
		ithGpioEnable(cam_gpio[i][CALL]);									//使能GPIO


		ithIntrEnableIrq(ITH_INTR_GPIO); 												//啟動 GPIO 的中斷功能(中斷控制器)

		ithEnterCritical();
		ithGpioClearIntr(cam_gpio[i][CALL]); 											//清除先前的中斷事件
		if(i == 0)
			ithGpioRegisterIntrHandler(cam_gpio[i][CALL], call_event_1, (void*)cam_gpio[i][CALL]); 	//註冊此 GPIO pin 的 ISR
		else if(i == 1)
			ithGpioRegisterIntrHandler(cam_gpio[i][CALL], call_event_2, (void*)cam_gpio[i][CALL]); 	//註冊此 GPIO pin 的 ISR
		ithGpioCtrlDisable(cam_gpio[i][CALL], ITH_GPIO_INTR_LEVELTRIGGER);				//設定中斷 trigger 方式是使用 edge trigger
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_INTR_TRIGGERLOW);				//設定中斷 trigger 方式是使用 edge trigger
		ithIntrEnableIrq(ITH_INTR_GPIO); 												//啟動 GPIO 的中斷功能(中斷控制器)
		ithGpioEnableBounce(cam_gpio[i][CALL]);
		ithGpioEnableIntr(cam_gpio[i][CALL]); 											//啟動此 GPIO pin 的中斷功能
		ithExitCritical();
	}

	for(i = 0; i < 2; i++)		//户外机供电GPIO初始化
	{
		ithGpioSetMode(cam_gpio[i][ON], ITH_GPIO_MODE0);
		ithGpioSetOut(cam_gpio[i][ON]);
		ithGpioSet(cam_gpio[i][ON]);
		ithGpioEnable(cam_gpio[i][ON]);
	}

	ithGpioSetMode(DOOR_1_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_1_OPEN);
	ithGpioSet(DOOR_1_OPEN);
	ithGpioEnable(DOOR_1_OPEN);

	ithGpioSetMode(DOOR_2_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_2_OPEN);
	ithGpioSet(DOOR_2_OPEN);
	ithGpioEnable(DOOR_2_OPEN);
	_user_call_init();
}



