#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "project.h"
#include "scene.h"

#include "user_function.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"

int SDL_main(int argc, char *argv[])
{
	int ret = 0;

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
	int restryCount = 0;

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
	
	ScreenInit();
	PhotoInit();
	StorageInit();
	//image_memo_init();
	//video_memo_init();
	user_gpio_init();
	UserTimerInit();
	user_pr2000_init();
	user_signal_control_init();
	UserCameraDetect();
	user_time_init();
	image_copy_sd();
	while(1)
	{
		if(cam_detect_over)
			break;
		else
			usleep(1000);
	}
	
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
