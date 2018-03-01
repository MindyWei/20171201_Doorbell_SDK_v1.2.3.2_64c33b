#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "scene.h"

#define INI_FILENAME "doorbell_admin.ini"
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

    // display
    theConfig.lang = iniparser_getint(cfgIni, "doorbell:lang", LANG_ENG);
    theConfig.brightness = iniparser_getint(cfgIni, "doorbell:brightness", 8);
    theConfig.screensaver_time = iniparser_getint(cfgIni, "doorbell:screensaver_time", 30);
    theConfig.screensaver_type = iniparser_getint(cfgIni, "doorbell:screensaver_type", SCREENSAVER_BLANK);

    // phone
    strcpy(theConfig.ringsound, iniparser_getstring(cfgIni, "doorbell:ringsound", "ring1.wav"));
    strcpy(theConfig.keysound, iniparser_getstring(cfgIni, "doorbell:keysound", "key1.wav"));
    theConfig.keylevel = iniparser_getint(cfgIni, "doorbell:keylevel", 80);
    strncpy(theConfig.ipcam_ipaddr, iniparser_getstring(cfgIni, "doorbell:ipcam_ipaddr", ""), sizeof (theConfig.ipcam_ipaddr) - 1);

    // setting
    strcpy(theConfig.engineer_password, iniparser_getstring(cfgIni, "doorbell:engineer_password", "666666"));

    // misc
    theConfig.dial_time = iniparser_getint(cfgIni, "doorbell:dial_time", 60);
    theConfig.calling_time = iniparser_getint(cfgIni, "doorbell:calling_time", 60);
    theConfig.missed_call_count = iniparser_getint(cfgIni, "doorbell:missed_call_count", 0);
    theConfig.muted = iniparser_getboolean(cfgIni, "doorbell:muted", 0);
    theConfig.touch_calibration = iniparser_getint(cfgIni, "doorbell:touch_calibration", 1);
    theConfig.do_not_disturb_mode = iniparser_getint(cfgIni, "doorbell:do_not_disturb_mode", DONOT_DISTURB_NONE);
    strcpy(theConfig.help_tel, iniparser_getstring(cfgIni, "doorbell:help_tel", ""));
    theConfig.videomemo_time = iniparser_getint(cfgIni, "doorbell:videomemo_time", 30);
    strcpy(theConfig.intercom_ipaddr, iniparser_getstring(cfgIni, "doorbell:intercom_ipaddr", "192.168.1.2"));
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

    iniparser_set(cfgIni, "doorbell:ipcam_ipaddr", theConfig.ipcam_ipaddr);

    // setting
    iniparser_set(cfgIni, "doorbell:engineer_password", theConfig.engineer_password);

    // misc
    sprintf(buf, "%d", theConfig.dial_time);
    iniparser_set(cfgIni, "doorbell:dial_time", buf);

    sprintf(buf, "%d", theConfig.calling_time);
    iniparser_set(cfgIni, "doorbell:calling_time", buf);

    sprintf(buf, "%d", theConfig.missed_call_count);
    iniparser_set(cfgIni, "doorbell:missed_call_count", buf);

    iniparser_set(cfgIni, "doorbell:muted", theConfig.muted ? "y" : "n");

    sprintf(buf, "%d", theConfig.touch_calibration);
    iniparser_set(cfgIni, "doorbell:touch_calibration", buf);

    sprintf(buf, "%d", theConfig.do_not_disturb_mode);
    iniparser_set(cfgIni, "doorbell:do_not_disturb_mode", buf);

    iniparser_set(cfgIni, "doorbell:help_tel", theConfig.help_tel);

    sprintf(buf, "%d", theConfig.videomemo_time);
    iniparser_set(cfgIni, "doorbell:videomemo_time", buf);

    iniparser_set(cfgIni, "doorbell:intercom_ipaddr", theConfig.intercom_ipaddr);

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
#endif

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
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
