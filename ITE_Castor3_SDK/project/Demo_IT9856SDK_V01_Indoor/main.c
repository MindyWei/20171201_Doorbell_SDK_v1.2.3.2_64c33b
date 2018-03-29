#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "scene.h"

extern pthread_mutex_t cfgMutex;

int SDL_main(int argc, char *argv[])
{
    int ret = 0;
    
#ifdef BACKUP_FLAG
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

#ifdef	CFG_DYNAMIC_LOAD_TP_MODULE
	//This function must be in front of SDL_Init().
	DynamicLoadTpModule();
#endif

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
#ifdef CFG_HDMI_OUT_ENABLE
	HdmiInit();
#endif //CFG_HDMI_OUT_ENABLE
    StorageInit();
    AudioInit();
    MessageInit();
    QRCodeInit();
    GuardInit();
	PeripheralInit();
    RemoteInit();
#ifdef CFG_SIP_PROXD_TEST
	SiproxdInit();
#endif


#ifdef CFG_CARDLIST_ENABLE
	
    CardListInit();
#endif // CFG_CARDLIST_ENABLE

    HouseInit();
    AudioRecordInit();
    VoiceMemoInit();

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        VideoRecordInit();
        VideoMemoInit();
    }

    CaptureInit();
    WeatherInit();
    PhotoInit();

    SceneInit();
    SceneLoad();
    ret = SceneRun();

    SceneExit();

    PhotoExit();
    WeatherExit();
    CaptureExit();

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        VideoMemoExit();
        VideoRecordExit();
    }
    VoiceMemoExit();
    AudioRecordExit();
    HouseExit();

#ifdef CFG_CARDLIST_ENABLE
	CardListExit();
#endif

    RemoteExit();
    AudioExit();
    GuardExit();
    QRCodeExit();
	MessageExit();
	LinphoneExit();
    CallLogExit();

    if (ret != QUIT_UPGRADE_WEB)
        WebServerExit();

    EventExit();
    NetworkExit();
    AddressBookExit();
    SDL_Quit();

end:
    pthread_mutex_lock(&cfgMutex);
    ret = UpgradeProcess(ret);
    pthread_mutex_unlock(&cfgMutex);
    itp_codec_standby();
    exit(ret);
    return ret;
}
