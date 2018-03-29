#include "ite/itu.h"

//Main
extern bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param);
extern bool MainKeyManagerOnPress(ITUWidget* widget, char* param);

// OpenDoor
extern bool OpenDoorOnEnter(ITUWidget* widget, char* param);

//ScreenSaver
extern bool ScreenSaverKeyOnPress(ITUWidget* widget, char* param);
extern bool ScreenSaverOnEnter(ITUWidget* widget, char* param);
extern bool ScreenSaverOnLeave(ITUWidget* widget, char* param);
extern bool ScreenSaverOnTimer(ITUWidget* widget, char* param);

//Upgrade
extern bool UpgradeOnTimer(ITUWidget* widget, char* param);
extern bool UpgradeOnEnter(ITUWidget* widget, char* param);

//User
extern bool UserKeyStarOnPress(ITUWidget* widget, char* param);
extern bool UserKeySharpOnPress(ITUWidget* widget, char* param);
extern bool UserKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool UserOnEnter(ITUWidget* widget, char* param);
extern bool UserOnLeave(ITUWidget* widget, char* param);
extern bool UserCallIncomingOnCustom(ITUWidget* widget, char* param);

//Calling
bool CallingConnectedOnCustom(ITUWidget* widget, char* param);
bool CallingKeyStarOnPress(ITUWidget* widget, char* param);
bool CallingKeySharpOnPress(ITUWidget* widget, char* param);
bool CallingOnTimer(ITUWidget* widget, char* param);
bool CallingEndOnCustom(ITUWidget* widget, char* param);
bool CallingErrorOnCustom(ITUWidget* widget, char* param);
bool CallingOnEnter(ITUWidget* widget, char* param);
bool CallingOnLeave(ITUWidget* widget, char* param);

//Called
bool CalledConnectedOnCustom(ITUWidget* widget, char* param);
bool CalledEndOnCustom(ITUWidget* widget, char* param);
bool CalledErrorOnCustom(ITUWidget* widget, char* param);
bool CalledKeyStarOnPress(ITUWidget* widget, char* param);
bool CalledKeySharpOnPress(ITUWidget* widget, char* param);
bool CalledOnTimer(ITUWidget* widget, char* param);
bool CalledOnEnter(ITUWidget* widget, char* param);
bool CalledOnLeave(ITUWidget* widget, char* param);

//VoiceMemo
bool VoiceMemoConnectedOnCustom(ITUWidget* widget, char* param);
bool VoiceMemoErrorOnCustom(ITUWidget* widget, char* param);
bool VoiceMemoKeyStarOnPress(ITUWidget* widget, char* param);
bool VoiceMemoKeySharpOnPress(ITUWidget* widget, char* param);
bool VoiceMemoKeyNumberOnPress(ITUWidget* widget, char* param);
bool VoiceMemoOnTimer(ITUWidget* widget, char* param);
bool VoiceMemoOnEnter(ITUWidget* widget, char* param);


//Password
extern bool PasswordKeyStarOnPress(ITUWidget* widget, char* param);
extern bool PasswordKeySharpOnPress(ITUWidget* widget, char* param);
extern bool PasswordKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool PasswordOnEnter(ITUWidget* widget, char* param);

//Setting
extern bool SettingKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingOnEnter(ITUWidget* widget, char* param);

//SettingDataSync
extern bool SettingDataSyncKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingDataSyncKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingDataSyncKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingDataSyncOnEnter(ITUWidget* widget, char* param);

//SettingLocalPassword
extern bool SettingLocalPasswordKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPasswordKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPasswordKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPasswordOnEnter(ITUWidget* widget, char* param);

//SettingDoorPassword
extern bool SettingDoorPasswordKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorPasswordKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorPasswordKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorPasswordOnEnter(ITUWidget* widget, char* param);

//SettingGuard
extern bool SettingGuardKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingGuardKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingGuardKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingGuardOnEnter(ITUWidget* widget, char* param);

//SettingTime
extern bool SettingTimeKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingTimeKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingTimeKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingTimeOnEnter(ITUWidget* widget, char* param);

//SettingLocalPosition
extern bool SettingLocalPositionKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPositionKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPositionKeyManagerOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPositionKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingLocalPositionOnEnter(ITUWidget* widget, char* param);

//SettingScreenSaver
extern bool SettingScreenSaverKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingScreenSaverKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingScreenSaverKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingScreenSaverOnEnter(ITUWidget* widget, char* param);

//SettingAudioVolume
extern bool SettingAudioVolumeKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingAudioVolumeKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingAudioVolumeKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingAudioVolumeOnEnter(ITUWidget* widget, char* param);

//SettingCustom
extern bool SettingCustomKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingCustomKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingCustomKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingCustomOnEnter(ITUWidget* widget, char* param);

//SettingSystem
extern bool SettingSystemKeyStarOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemKeySharpOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemKeyNumberOnPress(ITUWidget* widget, char* param);
extern bool SettingSystemOnEnter(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
	//Main
	"MainNotifyNetworkOnCustom", MainNotifyNetworkOnCustom,
    "MainKeyManagerOnPress", MainKeyManagerOnPress,
    //OpenDoor
    "OpenDoorOnEnter", OpenDoorOnEnter,

	//ScreenSaver
	"ScreenSaverKeyOnPress", ScreenSaverKeyOnPress,
	"ScreenSaverOnEnter", ScreenSaverOnEnter,
    "ScreenSaverOnLeave", ScreenSaverOnLeave,
    "ScreenSaverOnTimer", ScreenSaverOnTimer,
    //Upgrade
	"UpgradeOnTimer", UpgradeOnTimer,
	"UpgradeOnEnter", UpgradeOnEnter,
	//User
	"UserKeyStarOnPress", UserKeyStarOnPress,
	"UserKeySharpOnPress", UserKeySharpOnPress,
	"UserKeyNumberOnPress", UserKeyNumberOnPress,
	"UserOnEnter", UserOnEnter,
    "UserOnLeave", UserOnLeave,
    "UserCallIncomingOnCustom", UserCallIncomingOnCustom,
    //Calling
	"CallingConnectedOnCustom", CallingConnectedOnCustom,
	"CallingKeyStarOnPress", CallingKeyStarOnPress,
	"CallingKeySharpOnPress", CallingKeySharpOnPress,
	"CallingOnTimer", CallingOnTimer,
	"CallingEndOnCustom", CallingEndOnCustom,
	"CallingErrorOnCustom", CallingErrorOnCustom,
	"CallingOnEnter", CallingOnEnter,
    "CallingOnLeave", CallingOnLeave,
    //Called
	"CalledConnectedOnCustom", CalledConnectedOnCustom,
	"CalledEndOnCustom", CalledEndOnCustom,
	"CalledErrorOnCustom", CalledErrorOnCustom,
	"CalledKeyStarOnPress", CalledKeyStarOnPress,
	"CalledKeySharpOnPress", CalledKeySharpOnPress,
	"CalledOnTimer", CalledOnTimer,
	"CalledOnEnter", CalledOnEnter,
    "CalledOnLeave", CalledOnLeave,
    //VoiceMemo
	"VoiceMemoConnectedOnCustom", VoiceMemoConnectedOnCustom,
	"VoiceMemoErrorOnCustom", VoiceMemoErrorOnCustom,
	"VoiceMemoKeyStarOnPress", VoiceMemoKeyStarOnPress,
	"VoiceMemoKeySharpOnPress", VoiceMemoKeySharpOnPress,
	"VoiceMemoKeyNumberOnPress", VoiceMemoKeyNumberOnPress,
	"VoiceMemoOnTimer", VoiceMemoOnTimer,
	"VoiceMemoOnEnter", VoiceMemoOnEnter,
	//Password
	"PasswordKeyStarOnPress", PasswordKeyStarOnPress,
	"PasswordKeySharpOnPress", PasswordKeySharpOnPress,
	"PasswordKeyNumberOnPress", PasswordKeyNumberOnPress,
	"PasswordOnEnter", PasswordOnEnter,
	//Setting
	"SettingKeyStarOnPress", SettingKeyStarOnPress,
	"SettingKeySharpOnPress", SettingKeySharpOnPress,
	"SettingKeyNumberOnPress", SettingKeyNumberOnPress,
	"SettingOnEnter", SettingOnEnter,
	//SettingDataSync
	"SettingDataSyncKeyStarOnPress", SettingDataSyncKeyStarOnPress,
	"SettingDataSyncKeySharpOnPress", SettingDataSyncKeySharpOnPress,
	"SettingDataSyncKeyNumberOnPress", SettingDataSyncKeyNumberOnPress,
	"SettingDataSyncOnEnter", SettingDataSyncOnEnter,
    //SettingLocalPassword
	"SettingLocalPasswordKeyStarOnPress", SettingLocalPasswordKeyStarOnPress,
	"SettingLocalPasswordKeySharpOnPress", SettingLocalPasswordKeySharpOnPress,
	"SettingLocalPasswordKeyNumberOnPress", SettingLocalPasswordKeyNumberOnPress,
	"SettingLocalPasswordOnEnter", SettingLocalPasswordOnEnter,
    //SettingDoorPassword
	"SettingDoorPasswordKeyStarOnPress", SettingDoorPasswordKeyStarOnPress,
	"SettingDoorPasswordKeySharpOnPress", SettingDoorPasswordKeySharpOnPress,
	"SettingDoorPasswordKeyNumberOnPress", SettingDoorPasswordKeyNumberOnPress,
	"SettingDoorPasswordOnEnter", SettingDoorPasswordOnEnter,
    //SettingGuard
	"SettingGuardKeyStarOnPress", SettingGuardKeyStarOnPress,
	"SettingGuardKeySharpOnPress", SettingGuardKeySharpOnPress,
	"SettingGuardKeyNumberOnPress", SettingGuardKeyNumberOnPress,
	"SettingGuardOnEnter", SettingGuardOnEnter,
    //SettingTime
	"SettingTimeKeyStarOnPress", SettingTimeKeyStarOnPress,
	"SettingTimeKeySharpOnPress", SettingTimeKeySharpOnPress,
	"SettingTimeKeyNumberOnPress", SettingTimeKeyNumberOnPress,
	"SettingTimeOnEnter", SettingTimeOnEnter,
    //SettingLocalPosition
	"SettingLocalPositionKeyStarOnPress", SettingLocalPositionKeyStarOnPress,
	"SettingLocalPositionKeySharpOnPress", SettingLocalPositionKeySharpOnPress,
	"SettingLocalPositionKeyManagerOnPress", SettingLocalPositionKeyManagerOnPress,
	"SettingLocalPositionKeyNumberOnPress", SettingLocalPositionKeyNumberOnPress,
	"SettingLocalPositionOnEnter", SettingLocalPositionOnEnter,
    //SettingScreenSaver
	"SettingScreenSaverKeyStarOnPress", SettingScreenSaverKeyStarOnPress,
	"SettingScreenSaverKeySharpOnPress", SettingScreenSaverKeySharpOnPress,
	"SettingScreenSaverKeyNumberOnPress", SettingScreenSaverKeyNumberOnPress,
	"SettingScreenSaverOnEnter", SettingScreenSaverOnEnter,
    //SettingAudioVolume
	"SettingAudioVolumeKeyStarOnPress", SettingAudioVolumeKeyStarOnPress,
	"SettingAudioVolumeKeySharpOnPress", SettingAudioVolumeKeySharpOnPress,
	"SettingAudioVolumeKeyNumberOnPress", SettingAudioVolumeKeyNumberOnPress,
	"SettingAudioVolumeOnEnter", SettingAudioVolumeOnEnter,
    //SettingCustom
	"SettingCustomKeyStarOnPress", SettingCustomKeyStarOnPress,
	"SettingCustomKeySharpOnPress", SettingCustomKeySharpOnPress,
	"SettingCustomKeyNumberOnPress", SettingCustomKeyNumberOnPress,
	"SettingCustomOnEnter", SettingCustomOnEnter,
    //SettingSystem
	"SettingSystemKeyStarOnPress", SettingSystemKeyStarOnPress,
	"SettingSystemKeySharpOnPress", SettingSystemKeySharpOnPress,
	"SettingSystemKeyNumberOnPress", SettingSystemKeyNumberOnPress,
	"SettingSystemOnEnter", SettingSystemOnEnter,

    NULL, NULL
};
