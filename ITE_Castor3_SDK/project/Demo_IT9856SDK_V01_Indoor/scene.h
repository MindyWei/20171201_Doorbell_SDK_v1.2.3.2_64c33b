/** @file
 * ITE DoorBell Indoor Scene Definition.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_indoor ITE DoorBell Indoor Modules
 *  @{
 */
#ifndef SCENE_H
#define SCENE_H

#include "ite/itu.h"
#include "doorbell.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_indoor_scene Scene
 *  @{
 */

#define MS_PER_FRAME                17              ///< Drawing delay per frame

/**
 * Custom event definition.
 */
typedef enum
{
	EVENT_CUSTOM_NETWORK		= ITU_EVENT_CUSTOM, ///< Changes network status; The parameter "0" for not ready, "1" for ready. Custom0 event on GUI Designer.
	EVENT_CUSTOM_SCREENSAVER,                       ///< Ready to enter screensaver mode. Custom1 event on GUI Designer.
	EVENT_CUSTOM_MESSAGE,                           ///< Changes new message count status; The parameter is the string of new message count. Custom2 event on GUI Designer.
	EVENT_CUSTOM_MESSAGE_VIDEO,                     ///< Changes new video message count status; The parameter is the string of new video message count. Custom3 event on GUI Designer.
	EVENT_CUSTOM_MESSAGE_TEXT,                      ///< Changes new text message count status; The parameter is the string of new text message count. Custom4 event on GUI Designer.
	EVENT_CUSTOM_MESSAGE_RECORD,                    ///< Not used.
	EVENT_CUSTOM_DISTURB_MODE,                      ///< Changes do not disturb mode; The parameter is the string of DoNotDisturbMode. Custom6 event on GUI Designer.
	EVENT_CUSTOM_VOICEMEMO,                         ///< A new video memo request is incomming. The parameter is the string of IP address + call ID + having video or not (1 or 0). Custom7 event on GUI Designer.
	EVENT_CUSTOM_MISS_CALLS,                        ///< Changes the count status of missed calls; The parameter is the string of missed calls count. Custom8 event on GUI Designer.
	EVENT_CUSTOM_CALL_INCOMMING,                    ///< A new calling is incomming. The parameter is the string of IP address + call ID + having video or not (1 or 0). Custom9 event on GUI Designer.
	EVENT_CUSTOM_CALL_CONNECTED,                    ///< A calling is connected. Custom10 event on GUI Designer.
	EVENT_CUSTOM_CALL_END,                          ///< A calling is ended. Custom11 event on GUI Designer.
	EVENT_CUSTOM_CALL_ERROR,                        ///< A calling is error. The parameter is the string of SIP error code. Custom12 event on GUI Designer.
	EVENT_CUSTOM_CAMERA_TIMEOUT,                    ///< Watching camera is timeout. Custom13 event on GUI Designer.
	EVENT_CUSTOM_GUARD,                             ///< Changes guard mode; The parameter is "0" for disabling guard mode, "1" for enabling guard mode. Custom14 event on GUI Designer.
	EVENT_CUSTOM_UPGRADE,                           ///< Ready to upgrade address book or card list. Custom15 event on GUI Designer.
    EVENT_CUSTOM_ANSWER,                            ///< Answers current calling phone. Custom16 event on GUI Designer.
    EVENT_CUSTOM_SD_INSERTED,                       ///< #17: SD card inserted.
    EVENT_CUSTOM_SD_REMOVED,                        ///< #18: SD card removed.
	EVENT_CUSTOM_USB_INSERTED,                      ///< #19: USB drive inserted.
    EVENT_CUSTOM_USB_REMOVED                        ///< #20: USB drive removed.

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
 * Sets the status of scene.
 *
 * @param ready true for ready, false for not ready yet.
 */
void SceneSetReady(bool ready);

/**
 * Try to continue play guard alarm.
 */
void SceneContinuePlayGuardAlarm(void);

/**
 * Quits the scene.
 *
 * @param value The reason to quit the scene.
 */
void SceneQuit(QuitValue value);

/**
 * scene erter to video state.
 */
void SceneEnterVideoState(int timePerFrm);

/**
 * scene leave from video state.
 */
void SceneLeaveVideoState(void);

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
 * Sets ext calling status .
 *
 * @param 1 for on calling, 0 for idle not calling.
 */
void SceneSetExtCalling(int ext_calling);

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

/**
 * Predraw scene.
 */
void ScenePredraw(int arg);

/**
 * Resets mouse long-press time.
 */
void SceneResetLongPressTime(void);

// phone
/**
 * Sets the infomation to show on the add contact dialog.
 *
 * @param info The DeviceInfo of contact to add. Can be NULL.
 * @param comment The comment. Can be NULL.
 * @param lastLayer The widget of last layer. Can be NULL.
 */
void PhoneContactsAddShow(DeviceInfo* info, char* comment, ITULayer* lastLayer);

/**
 * Sets the infomation to show on the modify contact dialog.
 *
 * @param info The DeviceInfo of contact to add. Can be NULL.
 * @param comment The comment. Can be NULL.
 * @param blacklist The contact is in the black list or not.
 * @param refkey The reference key. Can be NULL.
 * @param lastLayer The widget of last layer. Can be NULL.
 */
void PhoneContactsModifyShow(DeviceInfo* info, char* comment, bool blacklist, char* refkey, ITULayer* lastLayer);

/**
 * Sets the infomation to show on the contact address keypad dialog.
 *
 * @param info The DeviceInfo of contact.
 * @param lastLayer The widget of last layer.
 */
void PhoneContactsKeypadShow(DeviceInfo* info, ITULayer* lastLayer);

/**
 * Sets the infomation to show on the contact comment keyboard dialog.
 *
 * @param room The contact address.
 * @param lastLayer The widget of last layer.
 */
void PhoneContactsKeyboardShow(char* room, ITULayer* lastLayer);

/**
 * Sets the infomation to show on the phone log detail dialog.
 *
 * @param name The alias name of target on the address book.
 * @param room The contact address.
 * @param time The logging time.
 * @param duration The phone time length.
 * @param type The string of CallLogType.
 * @param lastLayer The widget of last layer.
 */
void PhoneLogViewShow(char* name, char* room, char* time, char* duration, char* type, ITULayer* lastLayer);

// message
/**
 * Notifies the un-play count of captured recording files to the scene.
 *
 * @param count The un-play count of captured recording files.
 */
void MessageNotifyUnreadRecordMessage(int count);

/**
 * Sets the infomation to the playing video message screen.
 *
 * @param topic The topic of video message.
 * @param time The time of video message.
 * @param id The ID of video message.
 */
void MessageVideoViewShow(char* topic, char* time, int id);

/**
 * Sets the infomation to the playing captured recording file screen.
 *
 * @param topic The topic of recording file.
 * @param time The time of recording file.
 * @param id The ID of recording file.
 * @param entry The captured entry.
 */
void MessageRecordViewShow(char* topic, char* time, int id, CaptureEntry* entry);

/**
 * Sets the infomation to the viewing text message screen.
 *
 * @param image The URL of text message image.
 */
void MessageServiceViewShow(char* image);

// camera
/**
 * Sets the infomation to the viewing camera fullscreen.
 *
 * @param camera The camera name.
 * @param ip The IP address of camera.
 */
void CameraViewShow(char* camera, char* ip);

// guard

// house

// setting
/**
 * The function definition of getting inputted IP address.
 *
 * @param ip The inputted IP address.
 */
typedef void (*SceneSettingEngineerNetworkIPCallback)(char* ip);

/**
 * The function definition of getting inputted MAC address.
 *
 * @param mac The inputted MAC address.
 */
typedef void (*SceneSettingEngineerNetworkMacCallback)(char* mac);

/**
 * The function definition of getting inputted WiFi password.
 *
 * @param password The inputted WiFi password.
 */
typedef void (*SceneSettingEngineerNetworkWiFiPasswordCallback)(char* password);

/**
 * Shows the IP address setting dialog.
 *
 * @param ip The original IP address.
 * @param func The callback function which is called when the input is finished.
 */
void SettingEngineerNetworkIPShow(char* ip, SceneSettingEngineerNetworkIPCallback func);

/**
 * Shows the MAC address setting dialog.
 *
 * @param mac The original MAC address.
 * @param func The callback function which is called when the input is finished.
 */
void SettingEngineerNetworkMacShow(char* mac, SceneSettingEngineerNetworkMacCallback func);

/**
 * Shows the WiFi password setting dialog.
 *
 * @param password The original WiFi password.
 * @param func The callback function which is called when the input is finished.
 */
void SettingEngineerNetworkWiFiPasswordShow(char* password, SceneSettingEngineerNetworkWiFiPasswordCallback func);

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
 * Makes a phone call to the specified IP address of administrator unit device.
 *
 * @param ip The IP address of administrator unit device.
 * @return 0 for success, otherwise for fail.
 */
int CallingCallManager(char* ip);

// called
/**
 * Sets the infomation to the receiving call screen.
 *
 * @param id The call ID.
 * @param addr The IP address of target.
 * @param video 1 for has video, 0 for audio only.
 */
void CalledIncomingShow(int id, char* addr, int video);

// video record
/**
 * Sets the infomation to the video record screen.
 *
 * @param id The call ID.
 * @param addr The IP address of target.
 * @param video 1 for has video, 0 for audio only.
 */
void VideoRecordIncomingShow(int id, char* addr, int video);

// setting
/**
 * Resets password verify.
 */
void SettingResetVerify(void);

// dialogs
/**
 * The function definition of deleting dialog.
 *
 * @param del true for confirm, false for cancel.
 */
typedef void (*SceneDeleteCallback)(bool del);

/**
 * The function definition of clearing dialog.
 *
 * @param del true for confirm, false for cancel.
 */
typedef void (*SceneClearCallback)(bool clear);

/**
 * Shows the deleting dialog.
 *
 * @param func The callback function which is called when the user closes the dialog.
 */
void SceneDeleteShow(SceneDeleteCallback func);

/**
 * Shows the clearing dialog.
 *
 * @param func The callback function which is called when the user closes the dialog.
 */
void SceneClearShow(SceneClearCallback func);

/**
 * Global instance variable of scene.
 */
extern ITUScene theScene;

/** @} */ // end of doorbell_indoor_scene

#ifdef __cplusplus
}
#endif

#endif /* SCENE_H */
/** @} */ // end of doorbell_indoor