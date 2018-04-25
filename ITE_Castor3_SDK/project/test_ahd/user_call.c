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

/*	�Д� ISR �e����ӡӍϢ������
	����ʹ�� printf��
	��횸��� ithPrintf��
	ithPrintf ���� reentrant function��
*/
void *_call_int(int  val)	//�ж��¼�
{
	event_call_s = val;					//�¼���־
	
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
	
	for(i = 0; i < 2; i++)		//�����CALL�����GPIO��ʼ��
	{
		ithGpioSetMode(cam_gpio[i][CALL], ITH_GPIO_MODE0);				//����GPIOģʽ
		ithGpioSetIn(cam_gpio[i][CALL]);									//�������
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_PULL_ENABLE);			//�ڲ�����(����)ʹ��
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_PULL_UP);				//�ڲ�����
		ithGpioEnable(cam_gpio[i][CALL]);									//ʹ��GPIO


		ithIntrEnableIrq(ITH_INTR_GPIO); 												//���� GPIO ���Д๦��(�Д������)

		ithEnterCritical();
		ithGpioClearIntr(cam_gpio[i][CALL]); 											//�����ǰ���Д��¼�
		if(i == 0)
			ithGpioRegisterIntrHandler(cam_gpio[i][CALL], call_event_1, (void*)cam_gpio[i][CALL]); 	//�]�Դ� GPIO pin �� ISR
		else if(i == 1)
			ithGpioRegisterIntrHandler(cam_gpio[i][CALL], call_event_2, (void*)cam_gpio[i][CALL]); 	//�]�Դ� GPIO pin �� ISR
		ithGpioCtrlDisable(cam_gpio[i][CALL], ITH_GPIO_INTR_LEVELTRIGGER);				//�O���Д� trigger ��ʽ��ʹ�� edge trigger
		ithGpioCtrlEnable(cam_gpio[i][CALL], ITH_GPIO_INTR_TRIGGERLOW);				//�O���Д� trigger ��ʽ��ʹ�� edge trigger
		ithIntrEnableIrq(ITH_INTR_GPIO); 												//���� GPIO ���Д๦��(�Д������)
		ithGpioEnableBounce(cam_gpio[i][CALL]);
		ithGpioEnableIntr(cam_gpio[i][CALL]); 											//���Ӵ� GPIO pin ���Д๦��
		ithExitCritical();
	}

	for(i = 0; i < 2; i++)		//���������GPIO��ʼ��
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



