/** @file
 * ITE DoorBell Administrator Modules.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_admin ITE DoorBell Administrator Modules
 *  @{
 */
#ifndef DOORBELL_H
#define DOORBELL_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_admin_deviceinfo Device Infomation
 *  @{
 */
/**
 * Device type definition.
 */
typedef enum
{
    DEVICE_CENTER,      ///< Control Server
    DEVICE_OUTDOOR,     ///< Door Camera
    DEVICE_ENTRANCE,    ///< Lobby Phone
    DEVICE_BUILDING,    ///< Building Phone
    DEVICE_WALL,        ///< Wall Phone
    DEVICE_INDOOR,      ///< Indoor Phone
    DEVICE_MANAGER,     ///< Administrator Unit
    DEVICE_INDOOR2,     ///< Indoor Phone with internal SD storage
    DEVICE_MOBILE,      ///< Mobile phone
    DEVICE_INTERCOM,    ///< Intercom
    DEVICE_IPCAM,       ///< IP Camera
    DEVICE_UNKNOWN      ///< Unknown Device   
} DeviceType;

/**
 * Device information definition.
 */
typedef struct
{
    DeviceType type;    ///< Device type.
    char area[4];       ///< Area code of address, 2 characters. 
    char building[4];   ///< Building code of address, 2 characters. 
    char unit[4];       ///< Unit code of address, 2 characters. 
    char floor[4];      ///< Floor code of address, 2 characters. 
    char room[4];       ///< Room code of address, 2 characters. 
    char ext[4];        ///< Extension code of address, 2 characters. 
    char alias[128];    ///< Alias name written in XML of address book.
    char ip[16];        ///< IP address.
    char id[16];        ///< Ipcam id
    char pw[16];        /// <Ipcam password 
} DeviceInfo;

extern DeviceInfo theCenterInfo;    ///< Device information of control server.
extern DeviceInfo theDeviceInfo;    ///< Device information of this device.
extern DeviceInfo theLobbyInfo;     ///< Device information of the first lobby device on the address book.

/**
 * Initializes device information module.
 */
void DeviceInfoInit(void);

/**
 * Initializes device information module by DHCP setting.
 *
 * @param ip The IP address gotten from DHCP server.
 */
void DeviceInfoInitByDhcp(char* ip);

/**
 * Determines whether this device is in engineer mode or not.
 *
 * @return true in engineer mode, false in normal mode.
 */
bool DeviceInfoIsInEngineerMode(void);

/** @} */ // end of doorbell_admin_deviceinfo

/** @defgroup doorbell_admin_addressbook Address Book
 *  @{
 */
/**
 * Maximum Administrator Unit count.
 */
#define MAX_MANAGER_COUNT 10

/**
 * Maximum camera device count.
 */
#define MAX_CAMERA_COUNT 99

/**
 * Maximum misc device count.
 */
#define MAX_MISC_COUNT 99

/**
 * Maximum ip camera count.
 */
#define MAX_IPCAMERA_COUNT 9

/**
 * Remove old AddressBook.
 */
void RemoveAddressbook(void);

/**
 * Initializes address book module.
 */
void AddressBookInit(void);

/**
 * Exits address book module.
 */
void AddressBookExit(void);

/**
 * Gets version of addressbook.
 *
 * @return the version string. Remember to free it after used to avoid memory leak.
 */
char* AddressBookGetVersion(void);

/**
 * Gets device information of control server from addressbook.
 *
 * @param info The pointer to retrieve the DeviceInfo of control server.
 */
void AddressBookGetCenterInfo(DeviceInfo* info);

/**
 * Gets device information from addressbook by IP address.
 *
 * @param info The pointer to retrieve the DeviceInfo.
 * @param ip The IP address of device.
 */
void AddressBookGetDeviceInfo(DeviceInfo* info, char* ip);
/**
 * Gets IP address by address information from addressbook.
 *
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @param ext The ext code of address. Can be NULL if not exist.
 * @return the IP address string. Remember to free it after used to avoid memory leak.
 */
char* AddressBookGetDeviceIP(char* area, char* building, char* unit, char* floor, char* room, char* ext);

/**
 * Gets array of indoor phones by address information from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetDeviceInfoArray(DeviceInfo infoArray[], int count, char* area, char* building, char* unit, char* floor, char* room);

/**
 * Gets array of devices with camera from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetCameraInfoArray(DeviceInfo infoArray[], int count);

/**
 * Gets array of administrator units from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetManagerInfoArray(DeviceInfo infoArray[], int count);

/**
 * Gets array of misc devices from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetMiscInfoArray(DeviceInfo infoArray[], int count);

/**
 * Gets array of lobby phone devices from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetLobbyInfoArray(DeviceInfo infoArray[], int count);

/**
 * Gets IP address of gateway from addressbook.
 *
 * @param info The pointer to the DeviceInfo of this device.
 * @return the IP address string of gateway. Remember to free it after used to avoid memory leak.
 */
char* AddressBookGetGateway(DeviceInfo* info);

/**
 * Gets netmask from addressbook.
 *
 * @param info The pointer to the DeviceInfo of this device.
 * @return the netmask string. Remember to free it after used to avoid memory leak.
 */
char* AddressBookGetNetMask(DeviceInfo* info);

/**
 * Gets array of intercoms by address information from addressbook.
 *
 * @param infoArray The array of DeviceInfo to fill in.
 * @param count Maximum count of DeviceInfo in array.
 * @return the actual count of filled DeviceInfo in array.
 */
int AddressBookGetIntercomInfoArray(DeviceInfo infoArray[], int count);

/**
 * Is the intercom address exist in the addressbook.
 *
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @param ext The ext code of address.
 * @return true if exist, false otherwise.
 */
bool AddressBookIsIntercomExist(char* area, char* building, char* unit, char* floor, char* room, char* ext);

/**
 * Exports address book to USB disk or SD card.
 */
void AddressBookExport(void);

/** @} */ // end of doorbell_admin_addressbook

/** @defgroup doorbell_admin_audio Audio Player
 *  @{
 */
/**
 * Initializes audio module.
 */
void AudioInit(void);

/**
 * Exits audio module.
 */
void AudioExit(void);

/**
 * Plays the specified wav file.
 *
 * @param filename The specified wav file to play.
 * @return 0 for success, otherwise failed.
 */
int AudioPlay(char* filename);

/**
 * Stops playing sound.
 */
void AudioStop(void);

/**
 * Plays keypad sound.
 */
void AudioPlayKeySound(void);

/**
 * Sets the volume of keypad sound.
 *
 * @param level the percentage of volume.
 */
void AudioSetKeyLevel(int level);

/**
 * Mutes all audio.
 */
void AudioMute(void);

/**
 * Un-mutes all audio.
 */
void AudioUnMute(void);

/**
 * Determines whether this audio is muted or not.
 *
 * @return true muted, false otherwise.
 */
bool AudioIsMuted(void);

/** @} */ // end of doorbell_admin_audio
/** @defgroup doorbell_admin_calllog Call Logs
 *  @{
 */

/**
 * Call log type definition.
 */
typedef enum
{
    CALLLOG_UNKNOWN,   ///< Unknown call
    CALLLOG_MISSED,    ///< Missed call
    CALLLOG_RECEIVED,  ///< Received call
    CALLLOG_SENT       ///< Sent call
} CallLogType;

/**
 * Calling log definition
 */
typedef struct
{
    CallLogType type;   ///< Call log type
    char ip[16];        ///< IP address of target
    char time[32];      ///< Calling time
    int duration;       ///< Calling duration, in seconds.
    int refkey;         ///< Index number as reference key
} CallLogEntry;

/**
 * Initializes call log module.
 */
void CallLogInit(void);

/**
 * Exits call log module.
 */
void CallLogExit(void);

/**
 * Gets total count of call logs.
 *
 * @return the total count of call logs.
 */
int CallLogGetCount(void);

/**
 * Gets the call log entry by index.
 *
 * @param index The index of call log entry.
 * @return The call log entry, NULL if not found.
 */
CallLogEntry* CallLogGetEntry(int index);

/**
 * Deletes the call log entry by index.
 *
 * @param index The index of call log entry.
 */
void CallLogDeleteEntry(int index);

/**
 * Clears the call log entries.
 */
void CallLogClearEntries(void);
/**
 * Gets the count of current missed calls.
 *
 * @return The call log entry, NULL if not found.
 */
int CallLogGetCurrMissedCount(void);

/**
 * Clears the count of current missed calls.
 */
void CallLogClearCurrMissedCount(void);

/** @} */ // end of doorbell_admin_calllog

/** @defgroup doorbell_admin_capture Capture
 *  @{
 */

#define CAPUTRE_MAX_FILENAME_SIZE  40  ///< Maximum size of captured file name.
#define CAPUTRE_MAX_NAME_SIZE      40  ///< Maximum count of capture item name.

/**
 * Capture media definitions.
 */
typedef enum
{
    MEDIA_TEXT,     ///< Text media type.
    MEDIA_IMAGE,    ///< Image media type.
    MEDIA_VIDEO,    ///< Video media type.
    MEDIA_AUDIO     ///< Audio media type.
} MediaType;

/**
 * Captured file entry.
 */
typedef struct
{
    MediaType type;                             ///< The media type.
    char filename[CAPUTRE_MAX_FILENAME_SIZE];   ///< The captured file name.
    char name[CAPUTRE_MAX_NAME_SIZE];           ///< The name show on the UI.
    time_t time;                                ///< The captured time.
    int sec;                                    ///< The time length, in seconds.
    bool played;                                ///< The file is played or not.
} CaptureEntry;

/**
 * Initializes capture module.
 */
void CaptureInit(void);

/**
 * Exits capture module.
 */
void CaptureExit(void);

/**
 * Gets total count of captured file.
 *
 * @return the total count of captured file.
 */
int CaptureGetCount(void);

/**
 * Gets unplay count of captured file.
 *
 * @return unplay count of captured file.
 */
int CaptureGetUnplayCount(void);

/**
 * Gets the captured entry by index.
 *
 * @param index The index of captured entry.
 * @return The captured entry, NULL if not found.
 */
CaptureEntry* CaptureGetEntry(int index);

/**
 * Deletes the captured entry by index.
 *
 * @param index The index of captured entry.
 */
void CaptureDeleteEntry(int index);

/**
 * Deletes the Snapshotcaptured entry by index.
 *
 * @param index The index of captured entry.
 */
void CaptureSnapshotDeleteEntry(int index);

/**
 * Playing the captured file is finished or not.
 *
 * @return true for finished, false for otherwise.
 */
bool CaptureIsPlayFinished(void);

/**
 * Plays the captured file. Only for MEDIA_VIDEO and MEDIA_AUDIO types.
 *
 * @param index The index of captured file to play.
 * @return 0 for success, otherwise failed.
 */
int CaptureStartPlay(int index);

/**
 * Stops playing captured file.
 */
void CaptureStopPlay(void);

/**
 * Is recording or not.
 *
 * @return true if recording, false otherwise.
 */
bool CaptureIsRecording(void);

bool IPCamIsRecording(void);
int IPCamStartStreaming(void);
int IPCamStopStreaming(void);
int IPCamStartRecord(MediaType type, char* addr);
int IPCamStopRecord(void);
int IPCamSnapshot(char* addr, time_t t);

/**
 * Starts capturing.
 *
 * @param type The media type. Only for MEDIA_VIDEO and MEDIA_AUDIO types.
 * @param addr The IP address of target to capture.
 * @return 0 for success, otherwise failed.
 */
int CaptureStartRecord(MediaType type, char* addr);

/**
 * Stops capturing.
 *
 * @return 0 for success, otherwise failed.
 */
int CaptureStopRecord(void);

/**
 * Snapshots current image.
 *
 * @param addr The IP address of target to snapshot.
 * @param t The snapshot time. 0 for current time.
 * @return 0 for success, otherwise failed.
 */
int CaptureSnapshot(char* addr, time_t t);

/**
 * Gets the file name of snapshot by time.
 *
 * @param time The snapshot time. The format is like 01/01/2014 12:00:00
 * @return The file name.
 */
char* CaptureGetSnapshotFileNameByTime(char* time);

/**
 * Gets the path of capture directory.
 *
 * @return The path.
 */
char* CaptureGetPath(void);

/**
 * Checks the disk space is availabe to Snapshot current image.
 *
 * @return true for success, otherwise failed.
 */
bool CaptureCheckDiskSpace(int max_file_size, MediaType media_type);

/** @} */ // end of doorbell_admin_capture

/** @defgroup doorbell_admin_config Configuration
 *  @{
 */
/**
 * Do not disturb mode definition.
 */
typedef enum
{
    DONOT_DISTURB_NONE, ///< Do nothing
    DONOT_DISTURB_MUTE, ///< Mute
    DONOT_DISTURB_ALL   ///< All time
} DoNotDisturbMode;

/**
 * Language type definition.
 */
typedef enum
{
    LANG_ENG,   ///< English
    LANG_CHT,   ///< Traditional Chinese
    LANG_CHS    ///< Simplified Chinese
} LangType;

typedef struct
{
    // network
    int dhcp;                                                           ///< Enable DHCP or not
    char ipaddr[16];                                                    ///< IP address
    char netmask[16];                                                   ///< Netmask
    char gw[16];                                                        ///< Gateway address
    char dns[16];                                                       ///< DNS address

    // display
    int brightness;                                                     ///< Brightness, the range is 0~9
    int screensaver_time;                                               ///< Time to enter screen saver mode, unit is minute
    int screensaver_type;                                               ///< Screen saver type @see ScreensaverType
    int lang;                                                           ///< Language type @see LangType

    // phone
    char ringsound[PATH_MAX];                                           ///< Ring sound file path
    char keysound[PATH_MAX];                                            ///< Key sound file path
    int keylevel;                                                       ///< Key volume percentage, range is 0~100
    char ipcam_ipaddr[64];                                              ///< Binded IPCAM IP address

    // setting
    char engineer_password[8];                                          ///< Password of entering engineer mode
    char area[4];                                                       ///< Area code of address of this device
    char building[4];                                                   ///< Building code of address of this device
    char unit[4];                                                       ///< Unit code of address of this device
    char floor[4];                                                      ///< Floor code of address of this device
    char room[4];                                                       ///< Room code of address of this device
    char ext[4];                                                        ///< Extension code of address of this device
    int dial_time;                                                      ///< Maximum dialing time (sec)
    int calling_time;                                                   ///< Maximum calling time (sec)
    int missed_call_count;                                              ///< Missed call count
    int muted;                                                          ///< Audio muted or not
    int touch_calibration;                                              ///< Need to do the touch calibration or not
    int do_not_disturb_mode;                                            ///< House mode @see DoNotDisturbMode   
    char help_tel[16];                                                  ///< Help telephone number
    int videomemo_time;                                                 ///< Maximum video memo time (sec)
    char intercom_ipaddr[16];                                            ///< Default IP address of intercom device
} Config;

/**
 * Global instance variable of configuration.
 */
extern Config theConfig;

/**
 * Loads configuration file.
 */
void ConfigLoad(void);

/**
 * Updates CRC files.
 *
 * @param filepath The file path to update the CRC value. NULL for ini file on public drive.
 */
void ConfigUpdateCrc(char* filepath);

/**
 * Saves the public part of configuration to file.
 */
void ConfigSave(void);

/**
 * Saves the public and private parts of configuration to file.
 */
void ConfigSaveAll(void);

/** @} */ // end of doorbell_admin_config

/** @defgroup doorbell_admin_error Error definition
 *  @{
 */
/**
 * Error number definition.
 */
typedef enum
{
    ERROR_NO_ADMIN      ///< No more Administrator Unit
} ErrorNumber;

/** @} */ // end of doorbell_admin_error

/** @defgroup doorbell_admin_event Event
 *  @{
 */
/**
 * Calling out actions of uploading event definitions.
 */
typedef enum
{
    EVENT_CALLOUT_START,    ///< Calling out starting
    EVENT_CALLOUT_END,      ///< Calling out ended
    EVENT_CALLOUT_NOACK     ///< No response of calling out
} EventCalloutAction;

/**
 * Initializes event module.
 */
void EventInit(void);

/**
 * Exits event module.
 */
void EventExit(void);

/**
 * Writes calling out log to upload.
 *
 * @param ip The IP address of target to call out.
 * @param action The action occured.
 */
void EventWriteCalloutLog(char* ip, EventCalloutAction action);

/**
 * Writes registering device event to upload.
 *
 * @param hardwareAddress The MAC address of this device to upload.
 */
void EventWriteRegisterDevice(uint8_t hardwareAddress[]);

/** @} */ // end of doorbell_admin_event

/** @defgroup doorbell_admin_guard Guard
 *  @{
 */
/**
 * Guard events definition.
 */
typedef enum
{
    GUARD_EVENT_WARN,     ///< Warning event
    GUARD_EVENT_OPENDOOR  ///< Open door event

} GuardEvent;

/**
 * Guard type definition.
 */
typedef enum
{
    GUARD_TYPE_EMERGENCY,    ///< Emergency alarm
    GUARD_TYPE_INFRARED,     ///< Infrared alarm
    GUARD_TYPE_DOOR,         ///< Door alarm
    GUARD_TYPE_WINDOW,       ///< Window alarm
    GUARD_TYPE_SMOKE,        ///< Smoke alarm
    GUARD_TYPE_GAS,          ///< Gas alarm
    GUARD_TYPE_AREA,         ///< Area alarm
    GUARD_TYPE_ROB,          ///< Rob alarm
    GUARD_TYPE_OPENDOOR      ///< Open door alarm

} GuardType;

/**
 * Guard state definition.
 */
typedef enum
{
    GUARD_UNHANDLED,    ///< Guard event is unhandled
    GUARD_HANDLED       ///< Guard event is handled
} GuardState;

/**
 * Guard logging definition.
 */
typedef struct
{
    unsigned long timestamp;    ///< Time stamp of logging
    GuardEvent ev;              ///< Guard event of logging
    GuardType type;             ///< Guard type of logging
    char area[4];               ///< Area code of address, 2 characters. 
    char building[4];           ///< Building code of address, 2 characters. 
    char unit[4];               ///< Unit code of address, 2 characters. 
    char floor[4];              ///< Floor code of address, 2 characters. 
    char room[4];               ///< Room code of address, 2 characters. 
    char ext[4];                ///< Extension code of address, 2 characters. 
    GuardState state;           ///< Guard event state
    char handler[64];           ///< Handler.
    int read;                   ///< Read or not. 1: read 0: not read yet.
} GuardLog;

/**
 * Initializes guard module.
 */
void GuardInit(void);

/**
 * Exits guard module.
 */
void GuardExit(void);

/**
 * Gets total count of guard logging.
 *
 * @return The total count.
 */
int GuardGetLogCount(void);

/**
 * Gets the guard logging entry by the specified index.
 *
 * @param index The index to get logging entry.
 * @return The guard logging entry.
 */
GuardLog* GuardGetLog(int index);

/**
 * Deletes the guard logging entry by the specified index.
 *
 * @param index The index to delete logging entry.
 */
void GuardDeleteLog(int index);

/**
 * Loads all the guard logging entries from file.
 */
void GuardLoadLog(void);

/**
 * Saves all the guard logging entries to file.
 */
void GuardSaveLog(void);

/**
 * Parses the XML of guard log.
 *
 * @param xml The guard log XML to parse.
 * @param size The size of XML string.
 */
void GuardParseLog(char* xml, int size);

/**
 * Sends the guard sync log.
 *
 * @param log The guard log to sync.
 */
void GuardSendSyncLog(GuardLog* log);

/**
 * Receives the XML of guard sync log.
 *
 * @param xml The guard log XML to parse.
 * @param size The size of XML string.
 */
void GuardReceiveSyncLog(char* xml, int size);

/**
 * Gets count of unhandled guard logging.
 *
 * @return The count.
 */
int GuardGetUnhandledLogCount(void);

/**
 * Handles guard log.
 *
 * @param index The index of guard log to handle.
 * @param user The user ID.
 */
void GuardHandleLog(int index, char* user);

/** @} */ // end of doorbell_admin_guard

/** @defgroup doorbell_admin_linphone Linphone
 *  @{
 */
/**
 * Initializes linphone module.
 */
void LinphoneInit(void);

/**
 * Exits linphone module.
 */
void LinphoneExit(void);

/**
 * Gets the percentage of voice volume.
 *
 * @return The percentage of voice volume, range is 0~100.
 */
int LinphoneGetVoiceLevel(void);

/**
 * Sets the percentage of voice volume.
 *
 * @param level The percentage of voice volume, range is 0~100.
 */
void LinphoneSetVoiceLevel(int level);

/**
 * Gets the percentage of MIC volume.
 *
 * @return The percentage of MIC volume, range is 0~100.
 */
int LinphoneGetMicLevel(void);

/**
 * Sets the percentage of MIC volume.
 *
 * @param level The percentage of MIC volume, range is 0~100.
 */
void LinphoneSetMicLevel(int level);

/**
 * Plays ring sound.
 */
void LinphonePlayRingSound(int arg);

/**
 * Sets the file path of ring sound. Must be WAV file.
 *
 * @param file the file path of ring sound.
 */
void LinphoneSetRingSound(char* file);

/**
 * Gets the percentage of ring volume.
 *
 * @return The percentage of ring volume, range is 0~100.
 */
int LinphoneGetRingLevel(void);

/**
 * Sets the percentage of ring volume.
 *
 * @param level The percentage of ring volume, range is 0~100.
 * @param active Active or not.
 */
void LinphoneSetRingLevel(int level, bool active);

/**
 * Plays key sound.
 */
void LinphonePlayKeySound(void);

/**
 * Sets the percentage of key volume.
 *
 * @param level The percentage of key volume, range is 0~100.
 */
void LinphoneSetKeyLevel(int level);

/**
 * Plays warning sound.
 */
void LinphonePlayWarnSound(void);

/**
 * Makes a call to the specified IP address.
 *
 * @param ip The specified IP address to call.
 * @param video Enables video feature.
 * @param earlyMedia Enables early media feature.
 */
void LinphoneCall(char* ip, bool video, bool earlyMedia);

/**
 * Watches camera of the specified IP address.
 *
 * @param ip The specified IP address to watch.
 */
void LinphoneWatchCamera(char* ip);

/**
 * Stop watching camera.
 */
void LinphoneStopCamera(void);

void LinphoneBindIPCamEnable(void);
void LinphoneBindIPCamDisable(void);

/**
 * Answers a call for the specified call ID.
 *
 * @param id The call ID to answer.
 */
void LinphoneAnswer(int id);

/**
 * Mutes microphone.
 */
void LinphoneMuteMic(void);

/**
 * Un-mutes microphone.
 */
void LinphoneUnMuteMic(void);

/**
 * Terminates the specified call ID.
 *
 * @param id The call ID to terminate.
 */
void LinphoneTerminate(int id);

/**
 * Adds a contact.
 *
 * @param info The device information of the contact.
 * @param blacklist Adds the contact to black list or not.
 * @param comment The comment of the contact.
 */
void LinphoneAddFriend(DeviceInfo* info, bool blacklist, char* comment);

/**
 * Deletes a contact.
 *
 * @param addr The address of the contact to delete.
 */
void LinphoneDeleteFriend(char* addr);

/**
 * Clears all contacts.
 */
void LinphoneClearFriends(void);

/**
 * Deletes a calling log.
 *
 * @param refkey The reference key of the calling log to delete.
 */
void LinphoneDeleteCallLog(int refkey);

/**
 * Sets the window ID to display video.
 *
 * @param widget The widget pointer to display video.
 */
void LinphoneSetWindowID(void* widget);

/**
 * Plays the video message on the control server.
 *
 * @param ip The IP address of control server.
 * @param id The ID of video message to play.
 */
void LinphonePlayVideoMessage(char* ip, int id);

/**
 * Makes calls to the devices in the same time.
 *
 * @param infoArray the array of DeviceInfo to make calls.
 * @param count The count of DeviceInfo in the infoArray.
 * @param video Enables video feature.
 * @param earlyMedia Enables early media feature.
 */
void LinphoneCallDevices(DeviceInfo infoArray[], int count, bool video, bool earlyMedia, bool videoFromIPCam);

/**
 * Terminates all calls except the specified call ID.
 *
 * @param id Only the specified ID don't to terminate.
 */
void LinphoneTerminateOthers(long id);

/**
 * Stops playing sound.
 */
void LinphoneStopSound(void);

/**
 * Enables do not disturb mode.
 */
void LinphoneEnableDoNotDisturb(void);

/**
 * Disables do not disturb mode.
 */
void LinphoneDisableDoNotDisturb(void);
/**
 * Determines whether it is in do not disturb mode or not.
 *
 * @return true for it is in do not disturb mode, false otherwise.
 */
bool LinphoneIsInDoNotDisturb(void);

/**
 * Redirects current call to the specified IP address.
 *
 * @param ip The IP address to redirect to.
 */
void LinphoneRedirect(char* ip);

/**
 * Transfers current call to the specified IP address.
 *
 * @param ip The IP address to redirect to.
 */
void LinphoneTransfer(char* ip);

/**
 * Snapshots current video to the specified file path.
 *
 * @param filepath The file path to save the snapshot.
 */
void LinphoneSnapshot(char* filepath);

/**
 * Registers to SIP server. The SIP server address is the center address. The SIP ID is the device address like 01-01-01-01-01-01.
 */
void LinphoneRegister(void);

void LinphoneStartIPCamStreaming(void);
void LinphoneStopIPCamStreaming(void);
void LinphoneStartIPCamRecord(char* filepath);
void LinphoneStopIPCamRecord(void);
void LinphoneIPCamSnapshot(char* filepath);

/**
 * Starts a video memo recording to the specified file path.
 *
 * @param filepath The file path to save the video memo.
 */
void LinphoneStartVideoMemoRecord(char* filepath);

/**
 * Stops the video memo recording.
 */
void LinphoneStopVideoMemoRecord(void);

/**
 * Starts playing a video memo recording.
 *
 * @param filepath The file path of the video memo to play.
 */
void LinphoneStartVideoMemoPlay(char* filepath);

/**
 * Stops playing the video memo.
 */
void LinphoneStopVideoMemoPlay(void);

/**
 * Makes a video message to the specified device.
 *
 * @param info The DeviceInfo to make a video message.
 */
void LinphoneRecordVideoMessage(DeviceInfo* info, bool videoFromIPCam);

/** @} */ // end of doorbell_admin_linphone

/** @defgroup doorbell_admin_network Network
 *  @{
 */
/**
 * Initializes network module.
 */
void NetworkInit(void);

/**
 * Exits network module.
 */
void NetworkExit(void);

/**
 * Resets network module.
 */
void NetworkReset(void);

/**
 * Determines whether the network is ready or not.
 *
 * @return true for ready; false for net ready yet.
 */
bool NetworkIsReady(void);

/**
 * Determines whether the control server is online or not.
 *
 * @return true for online; false for offline.
 */
bool NetworkServerIsReady(void);

/**
 * Updates current time from SNTP server.
 */
void NetworkSntpUpdate(void);

/** @} */ // end of doorbell_admin_network

/** @defgroup doorbell_admin_remote Remote
 *  @{
 */
/**
 * Control Server information definition.
 */
typedef struct
{
    char firmwareVersion[64];   ///< Firmware version
    char firmwareUrl[256];      ///< Firmware URL
    int addressBookVersion;     ///< Address book version
    char addressBookUrl[256];   ///< Address book URL
    int screensaverVersion;     ///< Screensaver image version
    char screensaverUrl[256];   ///< Screensaver image URL
    int cardListVersion;        ///< Card list version
    char cardListUrl[256];      ///< Card list URL
    int settingVersion;         ///< Settings version
    char settingUrl[256];       ///< Settings URL
    int advertisementVersion;   ///< Advertisement version
    char advertisementUrl[256]; ///< Advertisement URL
    int ringLevel;              ///< Ring level
    int playLevel;              ///< Play level
    int recLevel;               ///< Record level
    int dialTime;               ///< Dial time
    int callingTime;            ///< Calling time
    int guardTime;              ///< Guard time
} ServerInfo;

/**
 * The function definition of getting the intercom information.
 *
 * @param info The pointer to retrieve ServerInfo.
 */
typedef void (*RemoteGetIntercomInfoCallback)(ServerInfo* info);

/**
 * Initializes remote module.
 */
void RemoteInit(void);

/**
 * Exits remote module.
 */
void RemoteExit(void);

/**
 * Opens door of the specified IP address.
 *
 * @param ip The IP address to open door.
 */
void RemoteOpenDoor(char* ip);

/**
 * Gets the information of control server.
 *
 * @return The ServerInfo of control server.
 */
ServerInfo* RemoteGetServerInfo(void);

/**
 * Gets the intercom information.
 *
 * @param ip The intercom ip.
 * @param func The callback function to call when the downloading finished.
 */
void RemoteGetIntercomInfo(char* ip, RemoteGetIntercomInfoCallback func);

/**
 * Sets the configuration to intercom.
 *
 * @param ip the IP address string.
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @param ext The ext code of address.
 * @return 0 for success; otherwise for fail.
 */
int RemoteSetIntercomSetting(char* ip, char* area, char* building, char* unit, char* floor, char* room, char* ext);

/**
 * Sets the intercom configuration.
 *
 * @param ip The intercom ip.
 * @param ringLevel The ring level.
 * @param playLevel The play level.
 * @param recLevel The record level.
 * @param dialTime The dial time.
 * @param callingTime The calling time.
 * @param guardTime The guard time.
 */
void RemoteSetIntercomConfig(char* ip, int ringLevel, int playLevel, int recLevel, int dialTime, int callingTime, int guardTime);

/** @} */ // end of doorbell_admin_remote

/** @defgroup doorbell_admin_screen Screen
 *  @{
 */
/**
 * Screensaver type definition.
 */
typedef enum
{
    SCREENSAVER_NONE,   ///< No screensaver
    SCREENSAVER_CLOCK,  ///< Clock sreensaver
    SCREENSAVER_BLANK,  ///< Black screen screensaver
    SCREENSAVER_PHOTO   ///< Photo screensaver
} ScreensaverType;

/**
 * Initializes screen module.
 */
void ScreenInit(void);

/**
 * Turns off screen.
 */
void ScreenOff(void);

/**
 * Turns on screen.
 */
void ScreenOn(void);

/**
 * Is the screen off or on.
 *
 * @return true for off; false for on.
 */
bool ScreenIsOff(void);

/**
 * Sets the brightness.
 *
 * @param value The brightness value.
 */
void ScreenSetBrightness(int value);

/**
 * Gets the maximum brightness level.
 *
 * @return the maximum brightness level.
 */
int ScreenGetMaxBrightness(void);

/**
 * Re-counts the time to enter screensaver.
 */
void ScreenSaverRefresh(void);

/**
 * Checks whether it is about time to enter screensaver mode.
 *
 * @return 0 for net yet, otherwise for entering screensaver mode currently.
 */
int ScreenSaverCheck(void);

/**
 * Is on screensaver mode or not.
 */
bool ScreenSaverIsScreenSaving(void);

/**
 * Takes a screenshot to USB drive.
 *
 * @param lcdSurf The LCD surface widget.
 */
void Screenshot(void* lcdSurf);

/** @} */ // end of doorbell_admin_screen

/** @defgroup doorbell_admin_string String
 *  @{
 */
/**
 * Gets address description by address information.
 *
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @param floor The floor code of address.
 * @param room The room code of address.
 * @param ext The ext code of address. Can be NULL if not exist.
 * @return the address string. Remember to free it after used to avoid memory leak.
 */
char* StringGetRoomAddress(char* area, char* building, char* unit, char* floor, char* room, char* ext);

/**
 * Gets camera description by address information.
 *
 * @param area The area code of address.
 * @param building The building code of address.
 * @param unit The unit code of address.
 * @return the camera string. Remember to free it after used to avoid memory leak.
 */
char* StringGetCameraPosition(char* area, char* building, char* unit);

/**
 * Gets system information description.
 *
 * @param count The pointer to retrieve the count of entry.
 * @return the array of system information strings.
 */
const char* StringGetSystemInfoArray(int* count);

/**
 * Gets device type description.
 *
 * @param type The device type.
 * @return the device type string.
 */
const char* StringGetDeviceType(DeviceType type);

/**
 * Gets the description of calling log type.
 *
 * @param type The calling log type.
 * @return the string of calling log type.
 */
const char* StringGetCallLogType(CallLogType type);

/**
 * Gets the description of guard event.
 *
 * @param ev The guard event.
 * @return the string of guard event.
 */
const char* StringGetGuardEvent(GuardEvent ev);

/**
 * Gets the description of guard type.
 *
 * @return the string of guard type.
 */
const char* StringGetGuardType(GuardType type);

/**
 * Gets the description of guard state.
 *
 * @return the string of guard state.
 */
const char* StringGetGuardState(GuardState state);

/**
 * Gets the description of working mode.
 *
 * @param engineer Is in engineer mode or not.
 * @return the string of working mode.
 */
const char* StringGetWorkMode(bool engineer);

/** @} */ // end of doorbell_admin_string

/** @defgroup doorbell_admin_upgrade Upgrade
 *  @{
 */
/**
 * Quit value definition.
 */
typedef enum
{
    QUIT_NONE,                  ///< Do not quit
    QUIT_DEFAULT,               ///< Quit for nothing
    QUIT_RESET_FACTORY,         ///< Quit to reset to factory setting
    QUIT_RESET_TEMP_DRIVE,      ///< Quit to reset temp drive
    QUIT_UPGRADE_FIRMWARE,      ///< Quit to upgrade firmware
    QUIT_UPGRADE_RESOURCE,      ///< Quit to upgrade resource
    QUIT_UPGRADE_ADDRESSBOOK,   ///< Quit to upgrade address book
    QUIT_UPGRADE_USERLIST,      ///< Quit to upgrade user list
    QUIT_UPGRADE_WEB,           ///< Quit to wait web upgrading
    QUIT_RESET_NETWORK          ///< Quit to reset network
} QuitValue;

/**
 * Initializes upgrade module.
 *
 * @return 0 for initializing success, non-zero for initializing failed and the value will be the QuitValue.
 */
int UpgradeInit(void);

/**
 * Sets the CRC value of the specified file path.
 *
 * @param filepath The file path to update the CRC value.
 */
void UpgradeSetFileCrc(char* filepath);

/**
 * Gets the XML string of the device information.
 *
 * @param size The pointer to retrieve the string size.
 * @return The XML string. Remember to free it after used to avoid memory leak.
 */
char* UpgradeGetDeviceInfo(int* size);

/**
 * Sets the URL to upgrade.
 *
 * @param url The url to download and upgrade.
 */
void UpgradeSetUrl(char* url);

/**
 * Sets the stream to upgrade.
 *
 * @param stream The stream to upgrade.
 */
void UpgradeSetStream(void* stream);

/**
 * Processes the upgrade procedure by QuitValue.
 *
 * @param code The QuitValue.
 * @return 0 for process success; otherwise failed.
 */
int UpgradeProcess(int code);

/**
 * Is upgrading finished or not.
 *
 * @return true for finished; otherwise not finish yet.
 */
bool UpgradeIsFinished(void);

/**
 * Gets the upgrading result.
 *
 * @return 0 for success, failed otherwise.
 */
int UpgradeGetResult(void);

/**
 * Gets the SD drive path.
 *
 * @return the SD drive path. NULL for SD not exist.
 */
char* UpgradeGetSDPath(void);

/** @} */ // end of doorbell_admin_upgrade

/** @defgroup doorbell_indoor_userlist User List
 *  @{
 */
/**
 * Initializes user list module.
 */
void UserListInit(void);

/**
 * Exits user list module.
 */
void UserListExit(void);

/**
 * Gets version of user list.
 *
 * @return the version integer.
 */
int UserListGetVersion(void);

/**
 * Reloads the user list.
 */
void UserListReload(void);

/**
 * Checks user login.
 *
 * @param id User ID.
 * @param pw User password.
 * @return true for pass, false for failed.
 */
bool UserListCheckUser(char* id, char* pw);

/**
 * Gets current user ID.
 *
 * @return current user ID.
 */
char* UserListCurrentUserID(void);

/**
 * Gets current user password.
 *
 * @return current user password.
 */
char* UserListCurrentUserPassword(void);

/**
 * Gets array of user id.
 *
 * @param idArray The array of user id to fill in.
 * @param count Maximum count of user id in array.
 * @return the actual count of filled user id in array.
 */
int UserListGetUserIDArray(char idArray[][64], int count);

/**
 * Deletes user.
 *
 * @param id The user id to delete.
 */
void UserListDeleteUser(char* id);

/**
 * Adds user.
 *
 * @param id The user id.
 * @param pw The user password.
 * @return true if success, false otherwise.
 */
bool UserListAddUser(char* id, char* pw);

/** @} */ // end of doorbell_indoor_userlist

/** @defgroup doorbell_admin_videorecord Video Recorder
 *  @{
 */
/**
 * Initializes video recorder module.
 */
void VideoRecordInit(void);

/**
 * Exits video recorder module.
 */
void VideoRecordExit(void);

int IPCamStreamStartStreaming(void);
int IPCamStreamStopStreaming(void);
int IPCamRecordStartRecord(char* filepath);
int IPCamRecordStopRecord(void);

/**
 * Start recording to the specified file.
 *
 * @param filepath The specified file to record.
 * @return 0 for success, otherwise failed.
 */
int VideoRecordStartRecord(char* filepath);

/**
 * Stop recording.
 *
 * @return 0 for success, otherwise failed.
 */
int VideoRecordStopRecord(void);

/**
 * Gets the time length of the specified recorded file, in seconds.
 *
 * @param filepath The specified file path.
 * @return The time length, in seconds.
 */
int VideoRecordGetTimeLength(char* filepath);

/**
 * The function definition of video record play finished.
 */
typedef void (*VideoRecordPlayFinishedCallback)(void);

/**
 * Plays the specified recorded file.
 *
 * @param filepath The specified recorded file to play.
 * @param func The callback function to call when the playing finished.
 * @return 0 for success, otherwise failed.
 */
int VideoRecordStartPlay(char* filepath, VideoRecordPlayFinishedCallback func);

/**
 * Stops playing recorded file.
 */
void VideoRecordStopPlay(void);

/** @} */ // end of doorbell_admin_videorecord

/** @defgroup doorbell_admin_webserver Web Server
 *  @{
 */
/**
 * Initializes web server module.
 */
void WebServerInit(void);

/**
 * Exits web server module.
 */
void WebServerExit(void);

/** @} */ // end of doorbell_admin_webserver

#ifdef __cplusplus
}
#endif

#endif /* DOORBELL_H */
/** @} */ // end of doorbell_admin