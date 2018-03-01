#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "scene.h"

#define INI_FILENAME "doorbell_lobby.ini"

typedef enum
{
    CONFIG_SAVE,
    CONFIG_SAVE_ALL
} ConfigAction;

Config theConfig;
static dictionary* cfgIni;
static dictionary* cfgPrivateIni;
static bool cfgIsSaving;
static pthread_mutex_t cfgMutex  = PTHREAD_MUTEX_INITIALIZER;

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

    // private
    strcpy(theConfig.local_pos, iniparser_getstring(cfgPrivateIni, "doorbell:local_pos", ""));
    strcpy(theConfig.local_ext, iniparser_getstring(cfgPrivateIni, "doorbell:local_ext", ""));

    // network
    theConfig.dhcp = iniparser_getboolean(cfgIni, "tcpip:dhcp", 1);
    strncpy(theConfig.ipaddr, iniparser_getstring(cfgIni, "tcpip:ipaddr", "192.168.1.1"), sizeof (theConfig.ipaddr) - 1);
    strncpy(theConfig.netmask, iniparser_getstring(cfgIni, "tcpip:netmask", "255.255.255.0"), sizeof (theConfig.netmask) - 1);
    strncpy(theConfig.gw, iniparser_getstring(cfgIni, "tcpip:gw", "192.168.1.254"), sizeof (theConfig.gw) - 1);
    strncpy(theConfig.dns, iniparser_getstring(cfgIni, "tcpip:dns", "192.168.1.254"), sizeof (theConfig.dns) - 1);

    // scene
    strcpy(theConfig.itufile, iniparser_getstring(cfgIni, "doorbell:itufile", "doorbell_lobby_chs.itu"));

    // settings
    strcpy(theConfig.engineer_password, iniparser_getstring(cfgIni, "doorbell:engineer_password", "666666"));
    strcpy(theConfig.door_password, iniparser_getstring(cfgIni, "doorbell:door_password", "666666"));
    theConfig.voicememo_time = iniparser_getint(cfgIni, "doorbell:voicememo_time", 30);
    theConfig.door_delay = iniparser_getint(cfgIni, "doorbell:door_delay", 30);
    theConfig.local_type = iniparser_getint(cfgIni, "doorbell:local_type", 4);
    theConfig.brightness = iniparser_getint(cfgIni, "doorbell:brightness", 8);
    theConfig.screensaver_time = iniparser_getint(cfgIni, "doorbell:screensaver_time", 30);
    theConfig.screensaver_type = iniparser_getint(cfgIni, "doorbell:screensaver_type", SCREENSAVER_BLANK);
    theConfig.warnlevel = iniparser_getint(cfgIni, "doorbell:warnlevel", 100);
    theConfig.keylevel = iniparser_getint(cfgIni, "doorbell:keylevel", 80);
    strcpy(theConfig.ringsound, iniparser_getstring(cfgIni, "doorbell:ringsound", "ring1.wav"));
    strcpy(theConfig.warnsound, iniparser_getstring(cfgIni, "doorbell:warnsound", "warn1.wav"));
    strcpy(theConfig.keysound, iniparser_getstring(cfgIni, "doorbell:keysound", "key1.wav"));
    strcpy(theConfig.help_tel, iniparser_getstring(cfgIni, "doorbell:help_tel", ""));
    theConfig.dial_time = iniparser_getint(cfgIni, "doorbell:dial_time", 60);
    theConfig.calling_time = iniparser_getint(cfgIni, "doorbell:calling_time", 60);
    strcpy(theConfig.lang, iniparser_getstring(cfgIni, "doorbell:lang", "chs"));

    // guard
    strcpy(theConfig.guard_sensor_initvalues, iniparser_getstring(cfgIni, "doorbell:guard_sensor_initvalues", "00"));
    strcpy(theConfig.guard_sensors, iniparser_getstring(cfgIni, "doorbell:guard_sensors", "10"));
    theConfig.guard_time = iniparser_getint(cfgIni, "doorbell:guard_time", 60);
}

static void ConfigSavePublic(void)
{
    FILE* f;
    char buf[8];

    // network
    iniparser_set(cfgIni, "tcpip:dhcp", theConfig.dhcp ? "y" : "n");
    iniparser_set(cfgIni, "tcpip:ipaddr", theConfig.ipaddr);
    iniparser_set(cfgIni, "tcpip:netmask", theConfig.netmask);
    iniparser_set(cfgIni, "tcpip:gw", theConfig.gw);
    iniparser_set(cfgIni, "tcpip:dns", theConfig.dns);

    // scene
    iniparser_set(cfgIni, "doorbell:itufile", theConfig.itufile);

    // settings
    iniparser_set(cfgIni, "doorbell:engineer_password", theConfig.engineer_password);
    iniparser_set(cfgIni, "doorbell:door_password", theConfig.door_password);

    sprintf(buf, "%d", theConfig.voicememo_time);
    iniparser_set(cfgIni, "doorbell:voicememo_time", buf);

    sprintf(buf, "%d", theConfig.door_delay);
    iniparser_set(cfgIni, "doorbell:door_delay", buf);

    sprintf(buf, "%d", theConfig.local_type);
    iniparser_set(cfgIni, "doorbell:local_type", buf);

    sprintf(buf, "%d", theConfig.brightness);
    iniparser_set(cfgIni, "doorbell:brightness", buf);

    sprintf(buf, "%d", theConfig.screensaver_time);
    iniparser_set(cfgIni, "doorbell:screensaver_time", buf);
    
    sprintf(buf, "%d", theConfig.screensaver_type);
    iniparser_set(cfgIni, "doorbell:screensaver_type", buf);

    sprintf(buf, "%d", theConfig.warnlevel);
    iniparser_set(cfgIni, "doorbell:warnlevel", buf);

    sprintf(buf, "%d", theConfig.keylevel);
    iniparser_set(cfgIni, "doorbell:keylevel", buf);

    iniparser_set(cfgIni, "doorbell:ringsound", theConfig.ringsound);
    iniparser_set(cfgIni, "doorbell:warnsound", theConfig.warnsound);
    iniparser_set(cfgIni, "doorbell:keysound", theConfig.keysound);
    iniparser_set(cfgIni, "doorbell:help_tel", theConfig.help_tel);

    sprintf(buf, "%d", theConfig.dial_time);
    iniparser_set(cfgIni, "doorbell:dial_time", buf);

    sprintf(buf, "%d", theConfig.calling_time);
    iniparser_set(cfgIni, "doorbell:calling_time", buf);

    iniparser_set(cfgIni, "doorbell:lang", theConfig.lang);

    // guard
    iniparser_set(cfgIni, "doorbell:guard_sensor_initvalues", theConfig.guard_sensor_initvalues);
    iniparser_set(cfgIni, "doorbell:guard_sensors", theConfig.guard_sensors);

    sprintf(buf, "%d", theConfig.guard_time);
    iniparser_set(cfgIni, "doorbell:guard_time", buf);

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

static void ConfigSavePrivate(void)
{
    FILE* f;

    // setting
    iniparser_set(cfgPrivateIni, "doorbell:local_pos", theConfig.local_pos);
    iniparser_set(cfgPrivateIni, "doorbell:local_ext", theConfig.local_ext);

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

static void* ConfigSaveTask(void* arg)
{
    int* args = (int*) arg;
    ConfigAction action = (ConfigAction) args[0];
    char* filepath = (char*) args[1];

    cfgIsSaving = true;

    ConfigSavePublic();

    if (action == CONFIG_SAVE_ALL)
        ConfigSavePrivate();

    pthread_mutex_lock(&cfgMutex);

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    UpgradeSetFileCrc(filepath);

    #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
    #endif

    BackupSave();
#else
    #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
    #endif
#endif
    pthread_mutex_unlock(&cfgMutex);

    cfgIsSaving = false;

    return NULL;
}

void ConfigUpdateCrc(char* filepath)
{
#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    pthread_mutex_lock(&cfgMutex);

    if (filepath)
        UpgradeSetFileCrc(filepath);
    else
        UpgradeSetFileCrc(CFG_PUBLIC_DRIVE ":/" INI_FILENAME);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    if (!filepath || memcmp(filepath, CFG_PUBLIC_DRIVE ":/",
            strlen(CFG_PUBLIC_DRIVE ":/")) == 0)
        BackupSave();

    pthread_mutex_unlock(&cfgMutex);
#endif // CFG_CHECK_FILES_CRC_ON_BOOTING
}

void ConfigSave(void)
{
    pthread_t task;
    pthread_attr_t attr;
    static int args[2];

    if (cfgIsSaving)
        return;

    args[0] = CONFIG_SAVE;
    args[1] = (int)CFG_PUBLIC_DRIVE ":/" INI_FILENAME;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, ConfigSaveTask, args);
}

void ConfigSaveAll(void)
{
    pthread_t task;
    pthread_attr_t attr;
    static int args[2];

    if (cfgIsSaving)
        return;

    args[0] = CONFIG_SAVE_ALL;
    args[1] = (int)CFG_PUBLIC_DRIVE ":/" INI_FILENAME;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, ConfigSaveTask, args);
}
