#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "project.h"
#include "scene.h"

#include "user_function.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"
#define TEST_PORT ITP_DEVICE_UART0
static sem_t UartSem;
static void UartCallback(void* arg1, uint32_t arg2)
{
	sem_post(&UartSem);
}

void* TestFunc_t(void* arg)
{
	int i;
	char getstr[256];
	char sendtr[256] = "hello SAT!\n";
	char sendtr_1[256] = "hello SAT!\n";
	int len = 0;
	int count =0;

	printf("Start uart test!\n");
	/*
		sem_init(&UartSem, 0, 0);

		itpRegisterDevice(TEST_PORT, &itpDeviceUart0);
		ioctl(TEST_PORT, ITP_IOCTL_INIT, NULL);
	#ifdef CFG_UART0_ENABLE
		ioctl(TEST_PORT, ITP_IOCTL_RESET, CFG_UART0_BAUDRATE);
	#endif
		ioctl(TEST_PORT, ITP_IOCTL_REG_UART_CB, (void*)UartCallback);
	*/
	memset(getstr, 0, 256);
	char aaa = 0x55;
	char bbb = 0;
	char ccc = 0;
	while(1)
	{
		while(1)
		{
			bbb = 0;
			//read(ITP_DEVICE_UART0, &bbb, 1);
			write(ITP_DEVICE_UART0, &aaa, 1);
			printf("-------------write\n");
			usleep(1000*1000);
			if(bbb)
				printf("------------->0x%2x\n",bbb);
		}
	}
}

int SDL_main(int argc, char *argv[])
{
	int ret = 0;

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
	int restryCount = 0;

	//user_gpio_init();
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

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());

	ConfigInit();
#ifdef CFG_NET_ENABLE
	NetworkInit();
#endif
	SceneInit();
	AudioInit();
	SceneLoad();
	
	flash_file_init();
	PhotoInit();
	StorageInit();
	image_memo_init();
	video_memo_init();
	user_gpio_init();
	user_timer_init();
	user_pr2000_init();
	user_signal_control_init();
	user_call_init();
	user_uart_init();
	//_user_rtc_init();
	user_cam_detect();
	user_time_init();
	user_auto_init();
	user_ring_play();
	image_copy_sd();
	while(1)
	{
		if(cam_detect_over)
			break;
		else
			usleep(1000);
	}
	//TestFunc_t(NULL);
	
	ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.language, 0, 0);
	ituSceneUpdate(&theScene, ITU_EVENT_LAYOUT, 0, 0, 0);
	ret = SceneRun();

	SceneExit();
	AudioExit();
	ConfigExit();
	SDL_Quit();

end:
	ret = UpgradeProcess(ret);
	exit(ret);
	return ret;
}
