/** @file
 * ITE DoorBell Outdoor Modules.
 *
 * @author Jim Tan
 * @version 1.0
 * @date 2013
 * @copyright ITE Tech. Inc. All Rights Reserved.
 */
/** @defgroup doorbell_outdoor ITE DoorBell Outdoor Modules
 *  @{
 */
#ifndef DOORBELL_H
#define DOORBELL_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup doorbell_outdoor_deviceinfo Device Infomation
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
    DEVICE_IPCAM,        ///< IP Camera
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
} DeviceInfo;

extern DeviceInfo theCenterInfo;    ///< Device information of control server.
extern DeviceInfo theDeviceInfo;    ///< Device information of this device.

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

/** @} */ // end of doorbell_outdoor_deviceinfo

/** @defgroup doorbell_outdoor_addressbook Address Book
 *  @{
 */
/**
 * Maximum Administrator Unit count.
 */
#define MAX_MANAGER_COUNT 10

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

/** @} */ // end of doorbell_outdoor_addressbook

/** @defgroup doorbell_outdoor_cardlist Card List
 *  @{
 */
/**
 * Initializes card list module.
 */
void CardListInit(void);

/**
 * Exits card list module.
 */
void CardListExit(void);

/**
 * Gets version of card list.
 *
 * @return the version integer.
 */
int CardListGetVersion(void);

/**
 * Reloads the card list.
 */
void CardListReload(void);

/** @} */ // end of doorbell_outdoor_cardlist

/** @defgroup doorbell_outdoor_config Configuration
 *  @{
 */
// configruation
/**
 * Configuration definition.
 */
typedef struct
{
    // network
    int dhcp;                                                           ///< Enable DHCP or not
    char ipaddr[16];                                                    ///< IP address
    char netmask[16];                                                   ///< Netmask
    char gw[16];                                                        ///< Gateway address
    char dns[16];                                                       ///< DNS address

    // settings
    char engineer_password[8];                                          ///< Password of entering engineer mode
    char door_password[8];                                              ///< Password of opening door
    int voicememo_time;                                                 ///< Maximum voice memo time (sec)
    int door_delay;                                                     ///< Open door delay (sec)
    int local_type;                                                     ///< Device type of this device @see DeviceType
    char area[4];                                                       ///< Area code of address of this device
    char building[4];                                                   ///< Building code of address of this device
    char unit[4];                                                       ///< Unit code of address of this device
    char floor[4];                                                      ///< Floor code of address of this device
    char room[4];                                                       ///< Room code of address of this device
    char ext[4];                                                        ///< Extension code of address of this device
    int brightness;                                                     ///< Brightness, the range is 0~9
    int screensaver_time;                                               ///< Time to enter screen saver mode, unit is minute
    int screensaver_type;                                               ///< Screen saver type @see ScreensaverType
    int keylevel;                                                       ///< Key volume percentage, range is 0~100
    char ringsound[PATH_MAX];                                           ///< Ring sound file path
    char keysound[PATH_MAX];                                            ///< Key sound file path
    char help_tel[16];                                                  ///< Help telephone number
    int dial_time;                                                      ///< Maximum dialing time (sec)
    int calling_time;                                                   ///< Maximum calling time (sec)
    int mgsalarm_time;                                                  ///< Maximum mgsalarm time (sec)       
    
    // guard
    char guard_sensor_initvalues[16];                                   ///< Intiial values of guard sensors

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

/** @} */ // end of doorbell_outdoor_config

/** @defgroup doorbell_outdoor_guard Guard
 *  @{
 */
// guard
/**
 * Guard sensors definition.
 */
typedef enum
{
    GUARD_EMERGENCY,    ///< Emergency sensor

    GUARD_SENSOR_COUNT  ///< Total sensor count
} GuardSensor;

/**
 * Guard status definition.
 */
typedef enum
{
    GUARD_ENABLE,   ///< Guard is enabled
    GUARD_DISABLE,  ///< Guard is disabled
    GUARD_ALARM,    ///< Guard is alarming
    GUARD_UNALARM   ///< Guard is un-alarmed
} GuardStatus;

/**
 * Initializes guard module.
 */
void GuardInit(void);
/**
 * Checks guard events.
 *
 * @return 0 for no event; -1 for events need to be alarming.
 */
int GuardCheck(void);

/** @} */ // end of doorbell_outdoor_guard

/** @defgroup doorbell_outdoor_event Event
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
 * Opening door mode of uploading event definitions.
 */
typedef enum
{
    EVENT_OPENDOOR_REMOTE,      ///< Opening door by remote device
    EVENT_OPENDOOR_PASSWORD,    ///< Opening door by password
    EVENT_OPENDOOR_CARD         ///< Opening door by card
} EventOpenDoorMode;

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
 * Writes guard log to upload.
 *
 * @param sensor The trigged sensor.
 * @param status The guard status.
 * @param timestamp The trigged time-stamp.
 */
void EventWriteGuardLog(GuardSensor sensor, GuardStatus status, time_t timestamp);

/**
 * Writes registering device event to upload.
 *
 * @param hardwareAddress The MAC address of this device to upload.
 */
void EventWriteRegisterDevice(uint8_t hardwareAddress[]);

/**
 * Writes opening door event to upload.
 *
 * @param mode The mode of open door.
 * @param info The device information. Can be NULL.
 * @param verified Success to open door or not.
 * @param card Card number if opening door by card. Can be NULL.
 * @param timestamp The pointer of trigged time-stamp. Can be NULL.
 */
void EventWriteOpenDoorLog(EventOpenDoorMode mode, DeviceInfo* info, bool verified, char* card, time_t* timestamp);

/**
 * Writes snapshoting event to upload.
 *
 * @param time The time of taking the snapshot.
 */
void EventWriteSnapshotLog(char* time);

/** @} */ // end of doorbell_outdoor_event

/** @defgroup doorbell_outdoor_linphone Linphone
 *  @{
 */
/**
 * Hint sound definition.
 */
typedef enum
{
    HINT_SOUND_OPENDOOR,        ///< Hint sound on opening door
    HINT_SOUND_VOICEMEMO,       ///< Hint sound on recording voice memo
    HINT_SOUND_ROOM_NOT_EXIST,  ///< Hint sound for calling wrong address
    HINT_SOUND_REDIAL           ///< Hint sound for redialing
} HintSound;

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
void LinphonePlayRingSound(void);

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
 */
void LinphoneSetRingLevel(int level);

/**
 * Plays key sound.
 */
void LinphonePlayKeySound(void);

/**
 * Enables key sound.
 */
void LinphoneEnableKeySound(void);

/**
 * Disables key sound.
 */
void LinphoneDisableKeySound(void);

/**
 * Gets the percentage of key volume.
 *
 * @return The percentage of key volume, range is 0~100.
 */
int LinphoneGetKeyLevel(void);

/**
 * Sets the percentage of key volume.
 *
 * @param level The percentage of key volume, range is 0~100.
 */
void LinphoneSetKeyLevel(int level);

/**
 * Plays hint sound.
 *
 * @param snd The hint sound to play.
 */
void LinphonePlayHintSound(HintSound snd);

/**
 * Plays key number sound.
 *
 * @param num The key number sound to play.
 */
void LinphonePlayNumberKeySound(char num);

/**
 * Makes a call to the specified IP address.
 *
 * @param ip The specified IP address to call.
 * @param video Enables video feature.
 */
void LinphoneCall(char* ip, bool video);

/**
 * Answers current call.
 */
void LinphoneAnswer(void);

/**
 * Terminates current call.
 */
void LinphoneTerminate(void);

/**
 * Makes calls to the devices in the same time.
 *
 * @param infoArray the array of DeviceInfo to make calls.
 * @param count The count of DeviceInfo in the infoArray.
 * @param video Enables video feature.
 * @param earlyMedia Enables early media feature.
 */
void LinphoneCallDevices(DeviceInfo infoArray[], int count, bool video, bool earlyMedia);

/**
 * Terminates all calls except the specified call ID.
 *
 * @param id Only the specified ID don't to terminate.
 */
void LinphoneTerminateOthers(long id);

/**
 * Makes a video message to the specified device.
 *
 * @param info The DeviceInfo to make a video message.
 */
void LinphoneRecordVideoMessage(DeviceInfo* info);

/**
 * Registers to SIP server. The SIP server address is the center address. The SIP ID is the device address like 01-01-01-01-01-01.
 */
void LinphoneRegister(void);

/** @} */ // end of doorbell_outdoor_linphone

/** @defgroup doorbell_outdoor_network Network
 *  @{
 */
/**
 * Initializes network module.
 */
void NetworkInit(void);

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

/** @} */ // end of doorbell_outdoor_network

/** @defgroup doorbell_outdoor_peripheral Peripheral
 *  @{
 */
/**
 * Initializes peripheral module.
 */
void PeripheralInit(void);

/**
 * Opens door.
 */
void PeripheralOpenDoor(void);

/**
 * Determines whether the door is opened or not.
 *
 * @return true for opened; false for closed.
 */
bool PeripheralDoorIsOpened(void);

/**
 * Reads the card number.
 *
 * @return the card number if exist; NULL for no card exists.
 */
char* PeripheralReadCard(void);

/**
 * Invokes the elevator.
 *
 * @param floor The floor of indoor phone.
 * @param up The direction is up or not.
 */
void PeripheralCallElevator(char* floor, bool up);

/** @} */ // end of doorbell_outdoor_peripheral

/** @defgroup doorbell_outdoor_remote Remote
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
} ServerInfo;

/**
 * Initializes remote module.
 */
void RemoteInit(void);

/**
 * Verifies the card number from the control server.
 *
 * @param cardnum The card number to verify.
 * @return true for valid; false for invalid.
 */
bool RemoteCheckCard(char* cardnum);

/**
 * Verifies the password by the indoor phone.
 *
 * @param num The floor code and room code.
 * @param password The password to verify.
 * @param devinfo The DeviceInfo of this device.
 * @return true for valid; false for invalid.
 */
bool RemoteIndoorCheckPassword(char* num, char* password, DeviceInfo* devinfo);

/**
 * Verifies the password by the control server.
 *
 * @param info The DeviceInfo which the password belong to.
 * @param password The password to verify.
 * @return true for valid; false for invalid.
 */
bool RemoteCenterCheckPassword(DeviceInfo* info, char* password);

/**
 * Gets the information of control server.
 *
 * @return The ServerInfo of control server.
 */
ServerInfo* RemoteGetServerInfo(void);

/** @} */ // end of doorbell_outdoor_remote

/** @defgroup doorbell_outdoor_screen Screen
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
 * Takes a screenshot to USB drive.
 *
 * @param lcdSurf The LCD surface widget.
 */
void Screenshot(void* lcdSurf);

/** @} */ // end of doorbell_outdoor_screen

/** @defgroup doorbell_outdoor_snapshot Snapshot
 *  @{
 */
/**
 * Snapshot definition.
 */
typedef struct
{
    char time[32];      ///< Snapshot time
    int size;           ///< Snapshot size
    uint8_t* buffer;    ///< Snapshot buffer pointer
} Snapshot;

/**
 * Initializes snapshot module.
 */
void SnapshotInit(void);

/**
 * Exits snapshot module.
 */
void SnapshotExit(void);

/**
 * Takes a snapshot.
 *
 * @return 0 for success; otherwise for fail.
 */
time_t SnapshotTake(void);

/**
 * Gets the snapshot data.
 *
 * @param time The snapshot time to get.
 * @param size The pointer to retrieve the size of JPEG data.
 * @return The snapshot JPEG data.
 */
uint8_t* SnapshotGet(char* time, int* size);

/** @} */ // end of doorbell_outdoor_snapshot

/** @defgroup doorbell_outdoor_upgrade Upgrade
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
    QUIT_UPGRADE_CARDLIST,      ///< Quit to upgrade card list
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

/** @} */ // end of doorbell_outdoor_upgrade

/** @defgroup doorbell_outdoor_webserver Web Server
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

/** @} */ // end of doorbell_outdoor_webserver

#ifdef __cplusplus
}
#endif

#endif /* DOORBELL_H */
/** @} */ // end of doorbell_outdoor