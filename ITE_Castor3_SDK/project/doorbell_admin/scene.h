/** @file
 * ITE DoorBell Administrator Scene Definition.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_admin ITE DoorBell Administrator Modules
 *  @{
 */
#ifndef SCENE_H
#define SCENE_H

#include "ite/itu.h"
#include "doorbell.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_admin_scene Scene
 *  @{
 */
typedef enum
{
	EVENT_CUSTOM_KEY_STAR		= ITU_EVENT_CUSTOM, ///< Presses "*" key; The parameter is the string of key. Custom0 event on GUI Designer.
	EVENT_CUSTOM_KEY_SHARP,                         ///< Presses "#" key; The parameter is the string of key. Custom1 event on GUI Designer.
	EVENT_CUSTOM_KEY_NUMBER,                        ///< Presses the number key; The parameter is the string of key. Custom2 event on GUI Designer.
	EVENT_CUSTOM_KEY_MANAGER,                       ///< Presses the key of calling to administrator unit. Custom3 event on GUI Designer.
	EVENT_CUSTOM_MESSAGE_GUARD,                     ///< Changes new guard message count status; The parameter is the string of new guard message count. Custom4 event on GUI Designer.
    EVENT_CUSTOM_NETWORK,                           ///< Changes network status; The parameter "0" for not ready, "1" for ready. Custom5 event on GUI Designer.
	EVENT_CUSTOM_DISTURB_MODE,                      ///< Changes do not disturb mode; The parameter is the string of DoNotDisturbMode. Custom6 event on GUI Designer.
	EVENT_CUSTOM_SCREENSAVER,                       ///< Ready to enter screensaver mode. Custom7 event on GUI Designer.
	EVENT_CUSTOM_MISS_CALLS,                        ///< Changes the count status of missed calls; The parameter is the string of missed calls count. Custom8 event on GUI Designer.
	EVENT_CUSTOM_CALL_INCOMMING,                    ///< A new calling is incomming. The parameter is the string of IP address + call ID + having video or not (1 or 0). Custom9 event on GUI Designer.
	EVENT_CUSTOM_CALL_CONNECTED,                    ///< A calling is connected. Custom10 event on GUI Designer.
	EVENT_CUSTOM_CALL_END,                          ///< A calling is ended. Custom11 event on GUI Designer.
	EVENT_CUSTOM_CALL_ERROR,                        ///< A calling is error. The parameter is the string of SIP error code. Custom12 event on GUI Designer.
	EVENT_CUSTOM_CAMERA_TIMEOUT,                    ///< Watching camera is timeout. Custom13 event on GUI Designer.
	EVENT_CUSTOM_CHANGE_DISTURB_MODE,               ///< Changes disturb mode. Custom14 event on GUI Designer.
	EVENT_CUSTOM_UPGRADE,                           ///< Ready to upgrade address book or card list. Custom15 event on GUI Designer.
    EVENT_CUSTOM_ANSWER                             ///< Answers current calling phone. Custom16 event on GUI Designer.

} CustomEvent;


typedef enum
{
    SPOTTED_NONE,
    SPOTTED_INIT,    
    SPOTTED_REJECT,
    SPOTTED_ALLOW,
} SPOTTED_STATUS; 


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
 * Opens the door on the remote device.
 *
 * @param ip The IP address of remote device. Can be NULL for current calling IP address of target.
 */
void SceneOpenDoor(char* ip);

/**
 * Hangs up the phone.
 */
void SceneHangUp(void);

/**
 * Clears all the missed calls.
 */
void SceneClearMissedCalls(void);

/**
 * Resets all the status of indicator UI.
 */
void SceneUpdateIndicators(void);

/**
 * Gotos main menu layer.
 */
void SceneGotoMainMenu(void);

/**
 * Is watching camera or video message right now.
 *
 * @return 1 for watching, 0 for not watching.
 */
int SceneCameraIsViewing(void);

/**
 * Sets the status of watching camera or video message.
 *
 * @param viewing true for watching, false for not watching.
 */
void SceneCameraSetViewing(bool viewing);

/**
 * Enables keypad or not.
 *
 * @param success true for enabled, false otherwise.
 */
void SceneEnableKeypad(bool en);

/**
 * Sets the status of scene.
 *
 * @param ready true for ready, false for not ready yet.
 */
void SceneSetReady(bool ready);

/**
 * Quits the scene.
 *
 * @param value The reason to quit the scene.
 */
void SceneQuit(QuitValue value);

#if (CFG_CHIP_FAMILY == 9850)
/**
 * scene erter to video state.
 */
void SceneEnterVideoState(int timePerFrm);

/**
 * scene leave from video state.
 */
void SceneLeaveVideoState(void);
#else
/**
 * scene erter to video state.
 */
void SceneEnterVideoState(void);

/**
 * scene leave from video state.
 */
void SceneLeaveVideoState(void);
#endif

/**
 * Gets the current quit value.
 *
 * @return The current quit value.
 */
QuitValue SceneGetQuitValue(void);

/**
 * Sets the IP address of remote side.
 *
 * @param the IP address of remote side.
 */
void SceneSetRemoteIP(char* ip);

/**
 * Is the call connected or not.
 *
 * @return true for connected, false for not connected yet.
 */
bool SceneIsCallConnected(void);

/**
 * Sets the last do not disturb mode.
 *
 * @param the the last do not disturb mode.
 */
void SceneSetLastDoNotDisturbMode(DoNotDisturbMode mode);

/**
 * Changes language file.
 */
void SceneChangeLanguage(void);

// camera
/**
 * Sets the infomation to the viewing camera fullscreen.
 *
 * @param camera The camera name.
 * @param ip The IP address of camera.
 */
void CameraViewShow(char* camera, char* ip);

// guard
/**
 * Sets the infomation to the viewing guard message screen.
 *
 * @param index The index of guard log.
 */
void GuardLogViewShow(int index);

// calling
/**
 * Makes a phone call to the specified address.
 *
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @param ext The ext code of address. Can be NULL if not exist.
 * @return 0 for success, otherwise for fail.
 */
int CallingCall(char* area, char* building, char* unit, char* floor, char* room, char* ext);

/**
 * Makes a phone call to the specified IP address.
 *
 * @param ip The IP address.
 * @return 0 for success, otherwise for fail.
 */
int CallingCallByIP(char* ip);

/**
 * Makes phone call to the administrator unit devices one by one.
 *
 * @return 0 for success, otherwise for fail.
 */
int CallingCallManager(void);

// called
/**
 * Sets the infomation to the receiving call screen.
 *
 * @param id The call ID.
 * @param addr The IP address of target.
 * @param video 1 for has video, 0 for audio only.
 */
void CalledIncomingShow(int id, char* addr, int video);

// error
/**
 * Sets the infomation to the error screen.
 *
 * @param err The error number.
 */
void ErrorShow(ErrorNumber err);

// redirect
/**
 * Hang up the current call.
 */
void RedirectHangUp(int arg);

// record view
/**
 * Sets the infomation to the playing captured recording file screen.
 *
 * @param topic The topic of recording file.
 * @param id The ID of recording file.
 * @param entry The captured entry.
 */
void RecordViewShow(char* topic, int id, CaptureEntry* entry);

// video memo
/**
 * Makes a video memo to the specified indoor phone device.
 *
 * @param info The DeviceInfo of indoor phone.
 */
void VideoMemoCall(DeviceInfo* info, bool videoFromIPCam);

/**
 * Global instance variable of scene.
 */
extern ITUScene theScene;

/** @} */ // end of doorbell_admin_scene

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */
/** @} */ // end of doorbell_admin