#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "scene.h"

int SDL_main(int argc, char *argv[])
{
    int ret = 0;

    ret = UpgradeInit();
    if (ret)
        goto end;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());

    ConfigLoad();
    AddressBookInit();
    DeviceInfoInit();
    NetworkInit();
    EventInit();
    WebServerInit();
    CallLogInit();
    LinphoneInit();
    ScreenInit();
    GuardInit();
    RemoteInit();
    VideoRecordInit();
    CaptureInit();
    UserListInit();

    SceneInit();
    SceneLoad();
    ret = SceneRun();

    SceneExit();

    UserListExit();
    CaptureExit();
    VideoRecordExit();
    RemoteExit();
    AudioExit();
    GuardExit();
	LinphoneExit();
    CallLogExit();

    if (ret != QUIT_UPGRADE_WEB)
        WebServerExit();

    NetworkExit();
    EventExit();
    AddressBookExit();
    SDL_Quit();

end:
    ret = UpgradeProcess(ret);
    itp_codec_standby();
    exit(ret);
    return ret;
}
