#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"
#include "user_capture.h"

#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
#if defined(TARGET_BOARD_G)
#define CAM_VOL_PIN  	63//32
#define BL_VOL_PIN  	36//34
#define BL_GPIO_PIN  	64//35

#define PR2000_MPP3 	72//73
#define PR2000_MPP4 	73//72
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
}
#elif defined(TARGET_BOARD_S)
#define DVDD_COUNT			63
#define BL_EN_PWM			64
#define LCD_LD_EN			65
#define CD4051_A				91
#define CD4051_B				92
#define DOOR_HOOK			90

static int door_gpio[DOOR_MAX][2] =  {{DOOR_1_ON,DOOR_1_CALL},
								{DOOR_2_ON,DOOR_2_CALL},
								{DOOR_3_ON,DOOR_3_CALL},
								{DOOR_4_ON,DOOR_4_CALL}};

static void user_gpio_init()
{	
	ithGpioSetMode(DVDD_COUNT, ITH_GPIO_MODE0);
	ithGpioSetOut(DVDD_COUNT);	
	ithGpioSet(DVDD_COUNT);	
	ithGpioEnable(DVDD_COUNT);	
	
	ithGpioSetMode(BL_EN_PWM, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_EN_PWM);
	ithGpioSet(BL_EN_PWM);	
	ithGpioEnable(BL_EN_PWM);	

	ithGpioSetMode(CD4051_A, ITH_GPIO_MODE0);
	ithGpioSetOut(CD4051_A);	
	ithGpioSet(CD4051_A);	
	ithGpioEnable(CD4051_A);	

	ithGpioSetMode(CD4051_B, ITH_GPIO_MODE0);
	ithGpioSetOut(CD4051_B);	
	ithGpioSet(CD4051_B);	
	ithGpioEnable(CD4051_B);	

	ithGpioSetMode(DOOR_HOOK, ITH_GPIO_MODE0);				//����GPIOģʽ
	ithGpioSetIn(DOOR_HOOK);									//�������		
	ithGpioCtrlEnable(DOOR_HOOK, ITH_GPIO_PULL_ENABLE);		//�ڲ�����(����)ʹ��
	ithGpioCtrlEnable(DOOR_HOOK, ITH_GPIO_PULL_UP);			//�ڲ�����
	ithGpioEnable(DOOR_HOOK);								//ʹ��GPIO	
	
}

static void *_call_int(int  val)	//�ж��¼�
{
	//ithPrintf("call_int----------------->%d\n",val);

	//ithPrintf("call_int----------------->over\n");
}
static void call_event_1(void *val)
{
	if(!DETECT_TYPE)
		_call_int(1);
}
static void call_event_2()
{
	if(!DETECT_TYPE)
		_call_int(2);
}
static void call_event_3()
{
	if(!DETECT_TYPE)
		_call_int(3);
}
static void call_event_4()
{
	if(!DETECT_TYPE)
		_call_int(4);
}
	


static void user_call_init()
{
	int i = 0 ;

	for(i = 0;i<DOOR_MAX;i++)		//�����CALL�����GPIO��ʼ��
	{
		ithGpioSetMode(door_gpio[i][CALL], ITH_GPIO_MODE0);				//����GPIOģʽ
		ithGpioSetIn(door_gpio[i][CALL]);									//�������		
		ithGpioCtrlEnable(door_gpio[i][CALL], ITH_GPIO_PULL_ENABLE);			//�ڲ�����(����)ʹ��
		ithGpioCtrlEnable(door_gpio[i][CALL], ITH_GPIO_PULL_UP);				//�ڲ�����
		ithGpioEnable(door_gpio[i][CALL]);									//ʹ��GPIO	
		
		ithIntrEnableIrq(ITH_INTR_GPIO); 												//���� GPIO ���Д๦��(�Д������)

		ithEnterCritical();
		ithGpioClearIntr(door_gpio[i][CALL]); 											//�����ǰ���Д��¼�
		if(i == 0)
			ithGpioRegisterIntrHandler(door_gpio[i][CALL], call_event_1, (void*)door_gpio[i][CALL]); 	//�]�Դ� GPIO pin �� ISR
		else if(i == 1)
			ithGpioRegisterIntrHandler(door_gpio[i][CALL], call_event_2, (void*)door_gpio[i][CALL]); 	
		else if(i == 2)
			ithGpioRegisterIntrHandler(door_gpio[i][CALL], call_event_3, (void*)door_gpio[i][CALL]); 	
		else if(i == 3)
			ithGpioRegisterIntrHandler(door_gpio[i][CALL], call_event_4, (void*)door_gpio[i][CALL]); 	
		
		ithGpioCtrlDisable(door_gpio[i][CALL], ITH_GPIO_INTR_LEVELTRIGGER);				//�O���Д� trigger ��ʽ��ʹ�� edge trigger
		ithGpioCtrlDisable(door_gpio[i][CALL], ITH_GPIO_INTR_BOTHEDGE); 					//�O���Д� edge trigger ��ʽ��ʹ�� both edge
		ithGpioCtrlEnable(door_gpio[i][CALL], ITH_GPIO_INTR_TRIGGERFALLING);				//
		ithIntrEnableIrq(ITH_INTR_GPIO); 												//���� GPIO ���Д๦��(�Д������)
		ithGpioEnableIntr(door_gpio[i][CALL]); 											//���Ӵ� GPIO pin ���Д๦��
		ithExitCritical(); 	
	}	
	for(i = 0;i<DOOR_MAX;i++)		//���������GPIO��ʼ��
	{
		ithGpioSetMode(door_gpio[i][ON], ITH_GPIO_MODE0);
		ithGpioSetOut(door_gpio[i][ON]);	
		ithGpioClear(door_gpio[i][ON]);	
		ithGpioEnable(door_gpio[i][ON]);	
	}

	//���������GPIO��ʼ��
	ithGpioSetMode(DOOR_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_OPEN);	
	ithGpioClear(DOOR_OPEN);	
	ithGpioEnable(DOOR_OPEN);
	
	ithGpioSetMode(DOOR_1_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_1_OPEN);	
	ithGpioClear(DOOR_1_OPEN);	
	ithGpioEnable(DOOR_1_OPEN);	
	
	ithGpioSetMode(DOOR_2_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_2_OPEN);
	ithGpioClear(DOOR_2_OPEN);	
	ithGpioEnable(DOOR_2_OPEN);
}

static void HOOK_ON_init()
{
	ithGpioSetMode(DOOR_HOOK, ITH_GPIO_MODE0);				//����GPIOģʽ
	ithGpioSetIn(DOOR_HOOK);									//�������		
	ithGpioCtrlEnable(DOOR_HOOK, ITH_GPIO_PULL_ENABLE);		//�ڲ�����(����)ʹ��
	ithGpioCtrlEnable(DOOR_HOOK, ITH_GPIO_PULL_UP);			//�ڲ�����
	ithGpioEnable(DOOR_HOOK);								//ʹ��GPIO	
}
#endif
#endif

int SDL_main(int argc, char *argv[])
{
    int ret = 0;
    int restryCount = 0;
    
#ifdef CFG_LCD_MULTIPLE
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESET, (void*)0);
#endif

#if !defined(CFG_LCD_INIT_ON_BOOTING) && !defined(CFG_BL_SHOW_LOGO)
    ScreenClear();
#endif

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    BackupInit();
retry_backup:
    ret = UpgradeInit();
    if (ret)
    {
        if (++restryCount > 2)
        {
            printf("cannot recover from backup....\n");
            goto end;
        }
        BackupRestore();
        goto retry_backup;
    }
    BackupSyncFile();
#else
    ret = UpgradeInit();
    if (ret)
        goto end;
#endif

#ifdef CFG_LCD_MULTIPLE
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    usleep(100000);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
#endif // CFG_LCD_MULTIPLE

#ifdef	CFG_DYNAMIC_LOAD_TP_MODULE
	//This function must be in front of SDL_Init().
	DynamicLoadTpModule();
#endif

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());

    ConfigInit();

#ifdef CFG_NET_ENABLE
    NetworkInit();
    #ifdef CFG_NET_WIFI

    #else 
        WebServerInit();
    #endif
#endif // CFG_NET_ENABLE

    ScreenInit();
    ExternalInit();
    StorageInit();
    AudioInit();
    PhotoInit();

#if defined(CFG_CAPTURE_MODULE_ENABLE)
	user_gpio_init();
	user_pr2000_init();
	user_TestFunc_init();
#if defined(TARGET_BOARD_S)
	user_call_init();
	HOOK_ON_init();
#endif
#endif

    SceneInit();
    SceneLoad();
    ret = SceneRun();

    SceneExit();

    PhotoExit();
    AudioExit();
    ExternalExit();

#ifdef CFG_NET_ENABLE
    if (ret != QUIT_UPGRADE_WEB)
        WebServerExit();
#endif // CFG_NET_ENABLE

    ConfigExit();
    SDL_Quit();

#if 0
    {
        static signed char buf[2048];
        vTaskList(buf);
        puts(buf);

    #ifdef CFG_DBG_RMALLOC
        Rmalloc_stat(__FILE__);
    #endif
    }
#endif

end:
    ret = UpgradeProcess(ret);
    itp_codec_standby();
    exit(ret);
    return ret;
}
