#include <unistd.h>
#include "SDL/SDL.h"
#include "doorbell.h"
#include "scene.h"
#include "video_encoder/video_encoder_it9910.h"

#define PULL_DOWN_NONUSE_GPIO 1
#define GPIO_BASE               0xDE000000

int SDL_main(int argc, char *argv[])
{
    int ret = 0;

    ret = UpgradeInit();
    if (ret)
        goto end;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());

#ifndef _WIN32
    VideoEncoder_Init();
#endif
#ifdef PULL_DOWN_NONUSE_GPIO
	printf("### Disable no use pin\n");
	//ithWriteRegMaskH(0x64, 0x1 << 15, 0x1 << 15);
	//GPIO30 PWDN
    //Set GPIO 0.1.2.3.5.6 to Low
    AHB_WriteRegisterMask(GPIO_BASE + 0x10, (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 5)|(1 << 6), (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 5)|(1 << 6));
    //Set GPIO 0.1.2.3.5.6 to Output Mode
    AHB_WriteRegisterMask(GPIO_BASE + 0x8, (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 5)|(1 << 6), (1 << 0)|(1 << 1)|(1 << 2)|(1 << 3)|(1 << 5)|(1 << 6));
    //Set GPIO 0.1.2.3.5.6 to Mode0
    AHB_WriteRegisterMask(GPIO_BASE + 0x90, (0x0 << (0 * 2))|(0x0 << (1 * 2))|(0x0 << (2 * 2))|(0x0 << (3 * 2))|(0x0 << (5 * 2))|(0x0 << (6 * 2)), (0x3 << (0 * 2))|(0x3 << (1 * 2))|(0x3 << (2 * 2))|(0x3 << (3 * 2))|(0x3 << (5 * 2))|(0x3 << (6 * 2)));
#endif
    ConfigLoad();
    AddressBookInit();
    DeviceInfoInit();
    NetworkInit();
    EventInit();
    WebServerInit();
    LinphoneInit();
    PeripheralInit();
    SnapshotInit();

#ifdef CFG_CARDLIST_ENABLE
    CardListInit();
#endif

    SceneInit();
    ret = SceneRun();
    
    SceneExit();

#ifdef CFG_CARDLIST_ENABLE
	CardListExit();
#endif
    
    SnapshotInit();
	LinphoneExit();

    if (ret != QUIT_UPGRADE_WEB)
        WebServerExit();

    EventExit();
    AddressBookExit();

end:
    ret = UpgradeProcess(ret);
    exit(ret);
    return ret;
}
