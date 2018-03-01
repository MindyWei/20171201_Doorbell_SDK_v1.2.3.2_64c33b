#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "scene.h"
#include "thread_helper.h"

#define INI_FILENAME "doorbell_indoor.ini"

#define GUARD_RECORD "guard_record.txt"

#ifdef BACKUP_FLAG
    #define BACKUP_DRIVE "D"
#else
    #define BACKUP_DRIVE CFG_TEMP_DRIVE
#endif

typedef enum
{
    CONFIG_CRC,
    CONFIG_SAVE,
    CONFIG_SAVE_PRIVATE,
#ifdef BACKUP_FLAG
    CONFIG_SAVE_TEMP,
    CONFIG_SAVE_TEMP_BACKUP
#else
    CONFIG_SAVE_TEMP
#endif
} ConfigAction;

Config theConfig;
static dictionary* cfgIni;
static dictionary* cfgPrivateIni;
static dictionary* cfgTempIni;
pthread_mutex_t cfgMutex  = PTHREAD_MUTEX_INITIALIZER;

extern GuardLog guardLogArray[];
extern int guardLogCount;

void ConfigLoad(void)
{
    cfgIni = iniparser_load(CFG_PUBLIC_DRIVE ":/" INI_FILENAME);
    if (!cfgIni)
    {
        cfgIni = dictionary_new(0);
        assert(cfgIni);

        dictionary_set(cfgIni, "tcpip", NULL);
        dictionary_set(cfgIni, "doorbell", NULL);
    }

    cfgPrivateIni = iniparser_load(CFG_PRIVATE_DRIVE ":/" INI_FILENAME);
	if (!cfgPrivateIni)
	{
	    cfgPrivateIni = dictionary_new(0);
        assert(cfgPrivateIni);

        dictionary_set(cfgPrivateIni, "doorbell", NULL);
	}

    cfgTempIni = iniparser_load(BACKUP_DRIVE ":/" INI_FILENAME);
	if (!cfgTempIni)
	{
	    cfgTempIni = dictionary_new(0);
        assert(cfgTempIni);

        dictionary_set(cfgTempIni, "doorbell", NULL);
	}

    // private
    strncpy(theConfig.area, iniparser_getstring(cfgPrivateIni, "doorbell:area", ""), sizeof (theConfig.area) - 1);
    strncpy(theConfig.building, iniparser_getstring(cfgPrivateIni, "doorbell:building", ""), sizeof (theConfig.building) - 1);
    strncpy(theConfig.unit, iniparser_getstring(cfgPrivateIni, "doorbell:unit", ""), sizeof (theConfig.unit) - 1);
    strncpy(theConfig.floor, iniparser_getstring(cfgPrivateIni, "doorbell:floor", ""), sizeof (theConfig.floor) - 1);
    strncpy(theConfig.room, iniparser_getstring(cfgPrivateIni, "doorbell:room", ""), sizeof (theConfig.room) - 1);
    strncpy(theConfig.ext, iniparser_getstring(cfgPrivateIni, "doorbell:ext", ""), sizeof (theConfig.ext) - 1);

    // network
    theConfig.dhcp = iniparser_getboolean(cfgIni, "tcpip:dhcp", 1);
    strncpy(theConfig.ipaddr, iniparser_getstring(cfgIni, "tcpip:ipaddr", "192.168.1.1"), sizeof (theConfig.ipaddr) - 1);
    strncpy(theConfig.netmask, iniparser_getstring(cfgIni, "tcpip:netmask", "255.255.255.0"), sizeof (theConfig.netmask) - 1);
    strncpy(theConfig.gw, iniparser_getstring(cfgIni, "tcpip:gw", "192.168.1.254"), sizeof (theConfig.gw) - 1);
    strncpy(theConfig.dns, iniparser_getstring(cfgIni, "tcpip:dns", "192.168.1.254"), sizeof (theConfig.dns) - 1);

    //multi network
    strncpy(theConfig.ipaddr_0, iniparser_getstring(cfgIni, "multi:ipaddr_0", "192.168.191.1"), sizeof (theConfig.ipaddr_0) - 1);
    strncpy(theConfig.netmask_0, iniparser_getstring(cfgIni, "multi:netmask_0", "255.255.255.0"), sizeof (theConfig.netmask_0) - 1);
    strncpy(theConfig.gw_0, iniparser_getstring(cfgIni, "multi:gw_0", "192.168.191.254"), sizeof (theConfig.gw_0) - 1);

	//dhcp_setting
	strncpy(theConfig.default_ipaddr, iniparser_getstring(cfgIni, "dhcp:default_ipaddr", "192.168.1.1"), sizeof (theConfig.default_ipaddr) - 1);
	strncpy(theConfig.dhcp_ipaddr, iniparser_getstring(cfgIni, "dhcp:dhcp_ipaddr", "192.168.1.1"), sizeof (theConfig.dhcp_ipaddr) - 1);

    // display
    theConfig.lang = iniparser_getint(cfgIni, "doorbell:lang", LANG_ENG);
    theConfig.brightness = iniparser_getint(cfgIni, "doorbell:brightness", 8);
    theConfig.screensaver_time = iniparser_getint(cfgIni, "doorbell:screensaver_time", 30);
    theConfig.screensaver_type = iniparser_getint(cfgIni, "doorbell:screensaver_type", SCREENSAVER_BLANK);

    // phone
    strcpy(theConfig.ringsound, iniparser_getstring(cfgIni, "doorbell:ringsound", "ring1.wav"));
    strcpy(theConfig.keysound, iniparser_getstring(cfgIni, "doorbell:keysound", "key1.wav"));
    theConfig.keylevel = iniparser_getint(cfgIni, "doorbell:keylevel", 80);

    // guard
    theConfig.guard_mode = iniparser_getint(cfgIni, "doorbell:guard_mode", GUARD_NONE);
    strcpy(theConfig.guard_password, iniparser_getstring(cfgIni, "doorbell:guard_password", "666666"));
    strcpy(theConfig.guard_sensor_initvalues, iniparser_getstring(cfgIni, "doorbell:guard_sensor_initvalues", "00000000"));
    strcpy(theConfig.guard_indoor_sensors, iniparser_getstring(cfgIni, "doorbell:guard_indoor_sensors", "00000000"));
    strcpy(theConfig.guard_outdoor_sensors, iniparser_getstring(cfgIni, "doorbell:guard_outdoor_sensors", "00000000"));
    strcpy(theConfig.guard_sleep_sensors, iniparser_getstring(cfgIni, "doorbell:guard_sleep_sensors", "00000000"));
    theConfig.warnlevel = iniparser_getint(cfgIni, "doorbell:warnlevel", 100);

    // house
    theConfig.house_mode = iniparser_getint(cfgIni, "doorbell:house_mode", HOUSE_NONE);
    strcpy(theConfig.house_indoor_airconditioners, iniparser_getstring(cfgIni, "doorbell:house_indoor_airconditioners", "00000000"));
    strcpy(theConfig.house_indoor_curtains, iniparser_getstring(cfgIni, "doorbell:house_indoor_curtains", "00000000"));
    strcpy(theConfig.house_indoor_lights, iniparser_getstring(cfgIni, "doorbell:house_indoor_lights", "00000000"));
    strcpy(theConfig.house_indoor_plugs, iniparser_getstring(cfgIni, "doorbell:house_indoor_plugs", "00000000"));
    strcpy(theConfig.house_indoor_tvs, iniparser_getstring(cfgIni, "doorbell:house_indoor_tvs", "00000000"));
    strcpy(theConfig.house_outdoor_airconditioners, iniparser_getstring(cfgIni, "doorbell:house_outdoor_airconditioners", "00000000"));
    strcpy(theConfig.house_outdoor_curtains, iniparser_getstring(cfgIni, "doorbell:house_outdoor_curtains", "00000000"));
    strcpy(theConfig.house_outdoor_lights, iniparser_getstring(cfgIni, "doorbell:house_outdoor_lights", "00000000"));
    strcpy(theConfig.house_outdoor_plugs, iniparser_getstring(cfgIni, "doorbell:house_outdoor_plugs", "00000000"));
    strcpy(theConfig.house_outdoor_tvs, iniparser_getstring(cfgIni, "doorbell:house_outdoor_tvs", "00000000"));
    strcpy(theConfig.house_sleep_airconditioners, iniparser_getstring(cfgIni, "doorbell:house_sleep_airconditioners", "00000000"));
    strcpy(theConfig.house_sleep_curtains, iniparser_getstring(cfgIni, "doorbell:house_sleep_curtains", "00000000"));
    strcpy(theConfig.house_sleep_lights, iniparser_getstring(cfgIni, "doorbell:house_sleep_lights", "00000000"));
    strcpy(theConfig.house_sleep_plugs, iniparser_getstring(cfgIni, "doorbell:house_sleep_plugs", "00000000"));
    strcpy(theConfig.house_sleep_tvs, iniparser_getstring(cfgIni, "doorbell:house_sleep_tvs", "00000000"));

    theConfig.airconditioner_celsius = iniparser_getboolean(cfgIni, "doorbell:airconditioner_celsius", 1);

    // setting
    strcpy(theConfig.engineer_password, iniparser_getstring(cfgIni, "doorbell:engineer_password", "666666"));
    strcpy(theConfig.outdoor_ipaddr, iniparser_getstring(cfgIni, "doorbell:outdoor_ipaddr", "192.168.1.2"));

    // wifi mode
    theConfig.wifi_mode = iniparser_getint(cfgIni, "wifi:wifi_mode", 0);
    // wifi switch status
    theConfig.wifi_status= iniparser_getint(cfgIni, "wifi:wifi_status", 0);

    // login
    strncpy(theConfig.user_id, iniparser_getstring(cfgIni, "elevator:user_id", "admin"), sizeof (theConfig.user_id) - 1);
    strncpy(theConfig.user_password, iniparser_getstring(cfgIni, "elevator:user_password", "admin"), sizeof (theConfig.user_password) - 1);

    // SSID
    snprintf(theConfig.ssid, 64, iniparser_getstring(cfgIni, "wifi:ssid", ""));
    // Password
    snprintf(theConfig.password, 256, iniparser_getstring(cfgIni,  "wifi:password", ""));
    // Security mode
    snprintf(theConfig.secumode, 3, iniparser_getstring(cfgIni, "wifi:secumode", "NA"));

    // misc
    theConfig.dial_time = iniparser_getint(cfgIni, "doorbell:dial_time", 60);
    theConfig.calling_time = iniparser_getint(cfgIni, "doorbell:calling_time", 60);
    theConfig.touch_calibration = iniparser_getint(cfgIni, "doorbell:touch_calibration", 1);
    theConfig.do_not_disturb_mode = iniparser_getint(cfgIni, "doorbell:do_not_disturb_mode", DONOT_DISTURB_NONE);
    theConfig.do_not_disturb_option = iniparser_getint(cfgIni, "doorbell:do_not_disturb_option", DONOT_DISTURB_HANGUP);
    theConfig.do_not_disturb_start_hour = iniparser_getint(cfgIni, "doorbell:do_not_disturb_start_hour", 0);
    theConfig.do_not_disturb_start_min = iniparser_getint(cfgIni, "doorbell:do_not_disturb_start_min", 0);
    theConfig.do_not_disturb_end_hour = iniparser_getint(cfgIni, "doorbell:do_not_disturb_end_hour", 0);
    theConfig.do_not_disturb_end_min = iniparser_getint(cfgIni, "doorbell:do_not_disturb_end_min", 0);
    theConfig.bgcolor_red = iniparser_getint(cfgIni, "doorbell:bgcolor_red", 128);
    theConfig.bgcolor_green = iniparser_getint(cfgIni, "doorbell:bgcolor_green", 128);
    theConfig.bgcolor_blue = iniparser_getint(cfgIni, "doorbell:bgcolor_blue", 128);
    strcpy(theConfig.wallpaper, iniparser_getstring(cfgIni, "doorbell:wallpaper", ""));
    theConfig.audiolevel = iniparser_getint(cfgIni, "doorbell:audiolevel", 80);

    // temp
#ifdef BACKUP_FLAG
    theConfig.missed_call_count = iniparser_getint(cfgIni, "doorbell:missed_call_count", 0);
    theConfig.muted = iniparser_getboolean(cfgIni, "doorbell:muted", 0);
#else
    theConfig.missed_call_count = iniparser_getint(cfgTempIni, "doorbell:missed_call_count", 0);
    theConfig.muted = iniparser_getboolean(cfgTempIni, "doorbell:muted", 0);
#endif

    // mobile
    snprintf(theConfig.mobile_user0, 64, iniparser_getstring(cfgIni,  "doorbell:mobile_user0", ""));
    snprintf(theConfig.mobile_user1, 64, iniparser_getstring(cfgIni,  "doorbell:mobile_user1", ""));
    snprintf(theConfig.mobile_user2, 64, iniparser_getstring(cfgIni,  "doorbell:mobile_user2", ""));
    snprintf(theConfig.mobile_user3, 64, iniparser_getstring(cfgIni,  "doorbell:mobile_user3", ""));
    snprintf(theConfig.mobile_user4, 64, iniparser_getstring(cfgIni,  "doorbell:mobile_user4", ""));

#ifndef WIN32
    FILE *f = NULL;
    char *buf[40];
    int i = 0, tmp_sensor = 0, tmp_status = 0;
    unsigned long tmp_time = 0;
    GuardLog* entry = NULL;

    f = fopen(CFG_PUBLIC_DRIVE ":/" GUARD_RECORD, "r");
    if(f != NULL) {
        while(fgets(buf, 40, f)!=NULL) {
            sscanf(buf, "%lu %d %d", &tmp_time, &tmp_sensor, &tmp_status);
            entry = &guardLogArray[i];
            entry->timestamp = tmp_time;
            entry->sensor = tmp_sensor;
            entry->status = tmp_status;
            i++;
        }
        if(i!=0) {
            guardLogCount = i;
        }
    }
    fclose(f);
#endif    
}

static void SaveGuardRecord(void)
{
    FILE *f;
    int count=0, i=0;
    GuardLog* entry = NULL;

    f = fopen(CFG_PUBLIC_DRIVE ":/" GUARD_RECORD, "wb");
	if (!f)
    {
	    printf("cannot open guard record file: %s\n", CFG_PUBLIC_DRIVE ":/" GUARD_RECORD);
        return;
    }

    count=GuardGetLogCount();
    for(i = count-1; i>= 0; i--)
    {
        entry = GuardGetLog(i);
 
        fprintf(f, "%lu %d %d\n", entry->timestamp, entry->sensor, entry->status);
    }


    fclose(f);
}

static void SavePublic(void)
{
    FILE* f;
    char buf[8];

    // network
    iniparser_set(cfgIni, "tcpip:dhcp", theConfig.dhcp ? "y" : "n");
    iniparser_set(cfgIni, "tcpip:ipaddr", theConfig.ipaddr);
    iniparser_set(cfgIni, "tcpip:netmask", theConfig.netmask);
    iniparser_set(cfgIni, "tcpip:gw", theConfig.gw);
    iniparser_set(cfgIni, "tcpip:dns", theConfig.dns);

	//multi network
	iniparser_set(cfgIni, "multi:ipaddr_0", theConfig.ipaddr_0);
    iniparser_set(cfgIni, "multi:netmask_0", theConfig.netmask_0);
    iniparser_set(cfgIni, "multi:gw_0", theConfig.gw_0);

	//dhcp info
    iniparser_set(cfgIni, "dhcp:default_ipaddr", theConfig.default_ipaddr);
	iniparser_set(cfgIni, "dhcp:dhcp_ipaddr", theConfig.dhcp_ipaddr);

    // display
    sprintf(buf, "%d", theConfig.lang);
    iniparser_set(cfgIni, "doorbell:lang", buf);

    sprintf(buf, "%d", theConfig.brightness);
    iniparser_set(cfgIni, "doorbell:brightness", buf);
    sprintf(buf, "%d", theConfig.screensaver_time);
    iniparser_set(cfgIni, "doorbell:screensaver_time", buf);
    sprintf(buf, "%d", theConfig.screensaver_type);
    iniparser_set(cfgIni, "doorbell:screensaver_type", buf);

    // phone
    iniparser_set(cfgIni, "doorbell:ringsound", theConfig.ringsound);
    iniparser_set(cfgIni, "doorbell:keysound", theConfig.keysound);

    sprintf(buf, "%d", theConfig.keylevel);
    iniparser_set(cfgIni, "doorbell:keylevel", buf);

    // guard
    sprintf(buf, "%d", theConfig.guard_mode);
    iniparser_set(cfgIni, "doorbell:guard_mode", buf);
    iniparser_set(cfgIni, "doorbell:guard_password", theConfig.guard_password);
    iniparser_set(cfgIni, "doorbell:guard_sensor_initvalues", theConfig.guard_sensor_initvalues);
    iniparser_set(cfgIni, "doorbell:guard_indoor_sensors", theConfig.guard_indoor_sensors);
    iniparser_set(cfgIni, "doorbell:guard_outdoor_sensors", theConfig.guard_outdoor_sensors);
    iniparser_set(cfgIni, "doorbell:guard_sleep_sensors", theConfig.guard_sleep_sensors);

    sprintf(buf, "%d", theConfig.warnlevel);
    iniparser_set(cfgIni, "doorbell:warnlevel", buf);

    // house
    sprintf(buf, "%d", theConfig.house_mode);
    iniparser_set(cfgIni, "doorbell:house_mode", buf);
    iniparser_set(cfgIni, "doorbell:house_indoor_airconditioners", theConfig.house_indoor_airconditioners);
    iniparser_set(cfgIni, "doorbell:house_indoor_curtains", theConfig.house_indoor_curtains);
    iniparser_set(cfgIni, "doorbell:house_indoor_lights", theConfig.house_indoor_lights);
    iniparser_set(cfgIni, "doorbell:house_indoor_plugs", theConfig.house_indoor_plugs);
    iniparser_set(cfgIni, "doorbell:house_indoor_tvs", theConfig.house_indoor_tvs);
    iniparser_set(cfgIni, "doorbell:house_outdoor_airconditioners", theConfig.house_outdoor_airconditioners);
    iniparser_set(cfgIni, "doorbell:house_outdoor_curtains", theConfig.house_outdoor_curtains);
    iniparser_set(cfgIni, "doorbell:house_outdoor_lights", theConfig.house_outdoor_lights);
    iniparser_set(cfgIni, "doorbell:house_outdoor_plugs", theConfig.house_outdoor_plugs);
    iniparser_set(cfgIni, "doorbell:house_outdoor_tvs", theConfig.house_outdoor_tvs);
    iniparser_set(cfgIni, "doorbell:house_sleep_airconditioners", theConfig.house_sleep_airconditioners);
    iniparser_set(cfgIni, "doorbell:house_sleep_curtains", theConfig.house_sleep_curtains);
    iniparser_set(cfgIni, "doorbell:house_sleep_lights", theConfig.house_sleep_lights);
    iniparser_set(cfgIni, "doorbell:house_sleep_plugs", theConfig.house_sleep_plugs);
    iniparser_set(cfgIni, "doorbell:house_sleep_tvs", theConfig.house_sleep_tvs);

    iniparser_set(cfgIni, "doorbell:airconditioner_celsius", theConfig.airconditioner_celsius ? "y" : "n");

    // setting
    iniparser_set(cfgIni, "doorbell:engineer_password", theConfig.engineer_password);
    iniparser_set(cfgIni, "doorbell:outdoor_ipaddr", theConfig.outdoor_ipaddr);

    // login
    iniparser_set(cfgIni, "elevator:user_id", theConfig.user_id);
    iniparser_set(cfgIni, "elevator:user_password", theConfig.user_password);

    //wifi    
    // SSID
    iniparser_set(cfgIni, "wifi:ssid", theConfig.ssid);
    // Password    
    iniparser_set(cfgIni, "wifi:password", theConfig.password);
    // Security mode    
    iniparser_set(cfgIni, "wifi:secumode", theConfig.secumode);

    // misc
    sprintf(buf, "%d", theConfig.dial_time);
    iniparser_set(cfgIni, "doorbell:dial_time", buf);

    sprintf(buf, "%d", theConfig.calling_time);
    iniparser_set(cfgIni, "doorbell:calling_time", buf);

    sprintf(buf, "%d", theConfig.touch_calibration);
    iniparser_set(cfgIni, "doorbell:touch_calibration", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_mode);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_mode", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_option);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_option", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_start_hour);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_start_hour", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_start_min);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_start_min", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_end_hour);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_end_hour", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_end_min);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_end_min", buf);

    sprintf(buf, "%d", theConfig.bgcolor_red);
    iniparser_set(cfgIni, "doorbell:bgcolor_red", buf);

    sprintf(buf, "%d", theConfig.bgcolor_green);
    iniparser_set(cfgIni, "doorbell:bgcolor_green", buf);

    sprintf(buf, "%d", theConfig.bgcolor_blue);
    iniparser_set(cfgIni, "doorbell:bgcolor_blue", buf);

    iniparser_set(cfgIni, "doorbell:wallpaper", theConfig.wallpaper);

    sprintf(buf, "%d", theConfig.audiolevel);
    iniparser_set(cfgIni, "doorbell:audiolevel", buf);

    // mobile
    iniparser_set(cfgIni, "doorbell:mobile_user0", theConfig.mobile_user0);
    iniparser_set(cfgIni, "doorbell:mobile_user1", theConfig.mobile_user1);
    iniparser_set(cfgIni, "doorbell:mobile_user2", theConfig.mobile_user2);
    iniparser_set(cfgIni, "doorbell:mobile_user3", theConfig.mobile_user3);
    iniparser_set(cfgIni, "doorbell:mobile_user4", theConfig.mobile_user4);

#ifdef BACKUP_FLAG
    // temp
    sprintf(buf, "%d", theConfig.missed_call_count);
    iniparser_set(cfgIni, "doorbell:missed_call_count", buf);
    iniparser_set(cfgIni, "doorbell:muted", theConfig.muted ? "y" : "n");
#endif

    // save to file
    f = fopen(CFG_PUBLIC_DRIVE ":/" INI_FILENAME, "wb");
	if (!f)
    {
	    printf("cannot open ini file: %s\n", CFG_PUBLIC_DRIVE ":/" INI_FILENAME);
        return;
    }

    iniparser_dump_ini(cfgIni, f);
    fclose(f);
}

static void SavePrivate(void)
{
    FILE* f;

    // setting
    iniparser_set(cfgPrivateIni, "doorbell:area", theConfig.area);
    iniparser_set(cfgPrivateIni, "doorbell:building", theConfig.building);
    iniparser_set(cfgPrivateIni, "doorbell:unit", theConfig.unit);
    iniparser_set(cfgPrivateIni, "doorbell:floor", theConfig.floor);
    iniparser_set(cfgPrivateIni, "doorbell:room", theConfig.room);
    iniparser_set(cfgPrivateIni, "doorbell:ext", theConfig.ext);

    // save to file
    f = fopen(CFG_PRIVATE_DRIVE ":/" INI_FILENAME, "wb");
	if (!f)
    {
	    printf("cannot open ini file: %s\n", CFG_PRIVATE_DRIVE ":/" INI_FILENAME);
        return;
    }

    iniparser_dump_ini(cfgPrivateIni, f);
    fclose(f);
}

static void SaveTemp(void)
{
    FILE* f;
    char buf[8];

    // temp
    sprintf(buf, "%d", theConfig.missed_call_count);
    iniparser_set(cfgTempIni, "doorbell:missed_call_count", buf);

    iniparser_set(cfgTempIni, "doorbell:muted", theConfig.muted ? "y" : "n");

    // save to file
    f = fopen(BACKUP_DRIVE ":/" INI_FILENAME, "wb");
	if (!f)
    {
	    printf("cannot open ini file: %s\n", BACKUP_DRIVE ":/" INI_FILENAME);
        return;
    }

    iniparser_dump_ini(cfgTempIni, f);
    fclose(f);
}

static void* ConfigSaveTask(void* arg)
{
    int* args = (int*) arg;
    ConfigAction action = (ConfigAction) args[0];
    char* filepath = (char*) args[1];

    pthread_mutex_lock(&cfgMutex);

    if (action == CONFIG_SAVE)
    {
        SavePublic();
#ifndef WIN32
        SaveGuardRecord();
#endif
    }
    else if (action == CONFIG_SAVE_PRIVATE)
    {
        SavePrivate();
    }
    else if (action == CONFIG_SAVE_TEMP)
    {
        SaveTemp();
    }
#ifdef BACKUP_FLAG
    else if (action == CONFIG_SAVE_TEMP_BACKUP)
    {
        SavePublic();
    }
#endif

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    if (action != CONFIG_SAVE_PRIVATE)
        UpgradeSetFileCrc(filepath);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    #ifdef BACKUP_FLAG
        if (action == CONFIG_SAVE || action == CONFIG_SAVE_TEMP_BACKUP)
            BackupSave();
        else if (action == CONFIG_CRC)
            if (memcmp(filepath, CFG_PUBLIC_DRIVE ":/",
                    strlen(CFG_PUBLIC_DRIVE ":/")) == 0)
                BackupSave();
    #endif
#endif

    pthread_mutex_unlock(&cfgMutex);

    return NULL;
}

void ConfigUpdateCrc(char* filepath)
{
#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    static int args[2];
    static char path[256];

    strcpy(path, filepath);

    args[0] = CONFIG_CRC;
    args[1] = (int)path;

    CreateWorkerThread(ConfigSaveTask, args);
#endif // CFG_CHECK_FILES_CRC_ON_BOOTING
}

void ConfigSave(void)
{
    static int args[2];

    args[0] = CONFIG_SAVE;
    args[1] = (int)CFG_PUBLIC_DRIVE ":/" INI_FILENAME;

    CreateWorkerThread(ConfigSaveTask, args);
}

void ConfigSavePrivate(void)
{
    static int args[2];

    args[0] = CONFIG_SAVE_PRIVATE;
    args[1] = (int)CFG_PRIVATE_DRIVE ":/" INI_FILENAME;

    CreateWorkerThread(ConfigSaveTask, args);
}

void ConfigSaveTemp(void)
{
    static int args[2];

    args[0] = CONFIG_SAVE_TEMP;
    args[1] = (int)BACKUP_DRIVE ":/" INI_FILENAME;

    CreateWorkerThread(ConfigSaveTask, args);
}

#ifdef BACKUP_FLAG
void ConfigSaveTempForBackup(void)
{
    static int args[2];

    args[0] = CONFIG_SAVE_TEMP_BACKUP;
    args[1] = (int)CFG_PUBLIC_DRIVE ":/" INI_FILENAME;

    CreateWorkerThread(ConfigSaveTask, args);
}
#endif
