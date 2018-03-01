#include "ite/itu.h"

extern void MainReset(void);
extern void OpenDoorReset(void);
extern void ScreenSaverReset(void);
extern void UpgradeReset(void);

extern void UserReset(void);
extern void CallingReset(void);
extern void CalledReset(void);
extern void VoiceMemoReset(void);
extern void PasswordReset(void);

extern void SettingReset(void);
extern void SettingDataSyncReset(void);
extern void SettingLocalPasswordReset(void);
extern void SettingDoorPasswordReset(void);
extern void SettingGuardReset(void);
extern void SettingTimeReset(void);
extern void SettingInfoReset(void);
extern void SettingLocalPositionReset(void);
extern void SettingScreenSaverReset(void);
extern void SettingAudioVolumeReset(void);
extern void SettingCustomReset(void);
extern void SettingSystemReset(void);

void resetScene(void)
{
	MainReset();
	OpenDoorReset();
	ScreenSaverReset();
    UpgradeReset();

    UserReset();
	CallingReset();
	CalledReset();
	VoiceMemoReset();
	PasswordReset();

	SettingReset();
	SettingDataSyncReset();
	SettingLocalPasswordReset();
	SettingDoorPasswordReset();
	SettingGuardReset();
	SettingTimeReset();
    SettingInfoReset();
	SettingLocalPositionReset();
	SettingScreenSaverReset();
	SettingAudioVolumeReset();
	SettingCustomReset();
    SettingSystemReset();
}
