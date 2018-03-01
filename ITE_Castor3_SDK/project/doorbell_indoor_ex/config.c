#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "config.h"

#define INI_FILENAME "doorbell_indoor.ini"

Config theConfig;
static dictionary* cfgIni;
static bool cfgIsSaving;

void ConfigLoad(void)
{
    cfgIni = iniparser_load(CFG_PRIVATE_DRIVE ":/" INI_FILENAME);
    if (!cfgIni)
    {
        cfgIni = dictionary_new(0);
        assert(cfgIni);

        dictionary_set(cfgIni, "sound", NULL);
    }

    theConfig.play_lev = iniparser_getint(cfgIni, "sound:play_lev", 50);
    theConfig.ring_lev = iniparser_getint(cfgIni, "sound:ring_lev", 50);
    theConfig.rec_lev  = iniparser_getint(cfgIni, "sound:rec_lev", 50);
}

static void ConfigSavePrivate(void)
{
    FILE* f;
    char buf[64];

    sprintf(buf, "%d", theConfig.play_lev);
    iniparser_set(cfgIni, "sound:play_lev", buf);
    
    sprintf(buf, "%d", theConfig.ring_lev);
    iniparser_set(cfgIni, "sound:ring_lev", buf);

    sprintf(buf, "%d", theConfig.rec_lev);
    iniparser_set(cfgIni, "sound:rec_lev", buf);    

    // save to file
    f = fopen(CFG_PRIVATE_DRIVE ":/" INI_FILENAME, "wb");
	if (!f)
    {
	    printf("cannot open ini file: %s\n", CFG_PRIVATE_DRIVE ":/" INI_FILENAME);
        return;
    }

    iniparser_dump_ini(cfgIni, f);
    fclose(f);
}

static void* ConfigSaveTask(void* arg)
{
    cfgIsSaving = true;

    ConfigSavePrivate();

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    cfgIsSaving = false;

    return NULL;
}

void ConfigSave(void)
{
    pthread_t task;
    pthread_attr_t attr;

    if (cfgIsSaving)
        return;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, ConfigSaveTask, NULL);
}
