/** @file
 * ITE DoorBell Lobby Scene Definition.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_lobby ITE DoorBell Lobby Modules
 *  @{
 */
#ifndef SCENE_H
#define SCENE_H

#include "ite/itu.h"
#include "doorbell.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_lobby_scene Scene
 *  @{
 */
typedef enum
{
	EVENT_CUSTOM_KEY_STAR		= ITU_EVENT_CUSTOM, ///< Presses "*" key; The parameter is the string of key. Custom0 event on GUI Designer.
	EVENT_CUSTOM_KEY_SHARP,                         ///< Presses "#" key; The parameter is the string of key. Custom1 event on GUI Designer.
	EVENT_CUSTOM_KEY_NUMBER,                        ///< Presses the number key; The parameter is the string of key. Custom2 event on GUI Designer.
	EVENT_CUSTOM_KEY_MANAGER,                       ///< Presses the key of calling to administrator unit. Custom3 event on GUI Designer.
	EVENT_CUSTOM_NETWORK,                           ///< Changes network status; The parameter "0" for not ready, "1" for ready. Custom4 event on GUI Designer.
	EVENT_CUSTOM_SCREENSAVER,                       ///< Ready to enter screensaver mode. Custom5 event on GUI Designer.
	EVENT_CUSTOM_CALL_INCOMMING,                    ///< A new calling is incomming. The parameter is the string of IP address + call ID + having video or not (1 or 0). Custom6 event on GUI Designer.
	EVENT_CUSTOM_CALL_CONNECTED,                    ///< A calling is connected. Custom7 event on GUI Designer.
	EVENT_CUSTOM_CALL_END,                          ///< A calling is ended. Custom8 event on GUI Designer.
	EVENT_CUSTOM_CALL_ERROR,                        ///< A calling is error. The parameter is the string of SIP error code. Custom9 event on GUI Designer.
    EVENT_CUSTOM_OPEN_DOOR,                         ///< Notifies to open the door. Custom10 event on GUI Designer.
	EVENT_CUSTOM_UPGRADE,                           ///< Ready to upgrade address book or card list. Custom11 event on GUI Designer.

} CustomEvent;

// scene
/**
 * Initializes scene module.
 */
void SceneInit(void);

/**
 * Exits scene module.
 */
void SceneExit(void);

/**
 * Loads ITU file.
 */
void SceneLoad(void);

/**
 * Runs the main loop to receive events, update and draw scene.
 *
 * @return The QuitValue.
 */
int SceneRun(void);

/**
 * Quits the scene.
 *
 * @param value The reason to quit the scene.
 */
void SceneQuit(QuitValue value);

/**
 * Gets the current quit value.
 *
 * @return The current quit value.
 */
QuitValue SceneGetQuitValue(void);

/**
 * Sets the IP address of remote side.
 *
 * @param ip the IP address of remote side.
 */
void SceneSetRemoteIP(char* ip);

/**
 * Is the call connected or not.
 *
 * @return true for connected, false for not connected yet.
 */
bool SceneIsCallConnected(void);

/**
 * Opens the door.
 *
 * @param updateUI updates UI or not.
 */
void SceneOpenDoor(int updateUI);

/**
 * Hangs up the phone.
 */
void SceneHangUp(void);

/**
 * Try to continue play guard alarm.
 */
void SceneContinuePlayGuardAlarm(void);

/**
 * Global instance variable of scene.
 */
extern ITUScene theScene;

// user

// calling
/**
 * Makes a phone call to the specified address.
 *
 * @param info The device info to call to. NULL for Administrator Units.
 * @return 0 for success, otherwise for fail.
 */
int CallingCall(DeviceInfo* info);

// called

// setting
/**
 * Shows the setting information page.
 */
void SettingInfoShow(void);

/**
 * Shows the setting local position page.
 */
void SettingLocalPositionShow(void);

/**
 * Shows the setting guard page.
 */
void SettingGuardShow(void);

/**
 * Shows the setting time page.
 */
void SettingTimeShow(void);

/**
 * Shows the setting screensaver page.
 */
void SettingScreenSaverShow(void);

/**
 * Shows the setting audio volume page.
 */
void SettingAudioVolumeShow(void);

/**
 * Shows the setting custom data page.
 */
void SettingCustomShow(void);

/**
 * Shows the setting system page.
 */
void SettingSystemShow(void);

// voice memo
/**
 * Makes a video memo to the specified indoor phone device.
 *
 * @param info The DeviceInfo of indoor phone.
 */
void VoiceMemoCall(DeviceInfo* info);

/** @} */ // end of doorbell_lobby_scene

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */
/** @} */ // end of doorbell_lobby