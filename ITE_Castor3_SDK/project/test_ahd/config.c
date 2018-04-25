#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "iniparser/iniparser.h"
#include "ite/itp.h"
#include "project.h"

#define INI_FILENAME "config.ini"

Config theConfig;
static dictionary* cfgIni;
static bool cfgIsSaving;
static int cfgSavingCount;
static pthread_mutex_t cfgMutex  = PTHREAD_MUTEX_INITIALIZER;

void Config_Reinit(void)
{
	//[record]
	theConfig.zidong = 0;
	theConfig.md = 0;
	theConfig.mdcam = 0;
	theConfig.mdsave = 0;
	theConfig.mdsensitive = 1;
	theConfig.mdtime = 0;
	theConfig.mdsyear = 2018;
	theConfig.mdsmonth = 1;
	theConfig.mdsday = 1;
	theConfig.mdshour = 0;
	theConfig.mdsmin = 0;
	theConfig.mdssec = 0;
	theConfig.mdeyear = 2018;
	theConfig.mdemonth = 1;
	theConfig.mdeday = 2;
	theConfig.mdehour = 0;
	theConfig.mdemin = 0;
	theConfig.mdesec = 0;

	//[volume]
	theConfig.mute = 0;
	theConfig.ringvol = 2;
	theConfig.talkvol = 2;
	theConfig.Iringvol = 2;
	theConfig.Italkvol = 2;
	theConfig.keyvol = 1;
	theConfig.interphone = 0;
	theConfig.door1 = 0;
	theConfig.door2 = 0;

	//[display]
	theConfig.brightness = 50;
	theConfig.contrast = 50;
	theConfig.hue = 50;
	theConfig.brightness_c = 50;
	theConfig.contrast_c = 50;
	theConfig.hue_c = 50;
	theConfig.lcdout = 1;
	theConfig.lcdouttime = 0;
	theConfig.infoframe = 0;

	//[other]
	theConfig.language = 0;
	theConfig.id = 0;
	theConfig.open = 0;

	//[user]
	theConfig.firstboot = 0;
	
	ConfigSave();
}

void ConfigInit(void)
{
	cfgIni = iniparser_load(CFG_PUBLIC_DRIVE ":/" INI_FILENAME);
	if (!cfgIni)
	{
		cfgIni = dictionary_new(0);
		assert(cfgIni);

		dictionary_set(cfgIni, "record", NULL);
		dictionary_set(cfgIni, "volume", NULL);
		dictionary_set(cfgIni, "display", NULL);
		dictionary_set(cfgIni, "other", NULL);
		dictionary_set(cfgIni, "user", NULL);
	}
	//[record]
	theConfig.zidong = iniparser_getint(cfgIni, "record:auto", 2);
	theConfig.md = iniparser_getint(cfgIni, "record:md", 0);
	theConfig.mdcam = iniparser_getint(cfgIni, "record:mdcam", 0);
	theConfig.mdsave = iniparser_getint(cfgIni, "record:mdsave", 0);
	theConfig.mdsensitive = iniparser_getint(cfgIni, "record:mdsensitive", 1);
	theConfig.mdtime = iniparser_getint(cfgIni, "record:mdtime", 0);
	theConfig.mdsyear = iniparser_getint(cfgIni, "record:mdsyear", 2018);
	theConfig.mdsmonth = iniparser_getint(cfgIni, "record:mdsmonth", 1);
	theConfig.mdsday = iniparser_getint(cfgIni, "record:mdsday", 1);
	theConfig.mdshour = iniparser_getint(cfgIni, "record:mdshour", 0);
	theConfig.mdsmin = iniparser_getint(cfgIni, "record:mdsmin", 0);
	theConfig.mdssec = iniparser_getint(cfgIni, "record:mdssec",0);
	theConfig.mdeyear = iniparser_getint(cfgIni, "record:mdeyear", 2018);
	theConfig.mdemonth = iniparser_getint(cfgIni, "record:mdemonth", 1);
	theConfig.mdeday = iniparser_getint(cfgIni, "record:mdeday", 2);
	theConfig.mdehour = iniparser_getint(cfgIni, "record:mdehour", 0);
	theConfig.mdemin = iniparser_getint(cfgIni, "record:mdemin", 0);
	theConfig.mdesec = iniparser_getint(cfgIni, "record:mdesec", 0);

	//[volume]
	theConfig.mute = iniparser_getint(cfgIni, "volume:mute", 0);
	theConfig.ringvol = iniparser_getint(cfgIni, "volume:ringvol", 2);
	theConfig.talkvol = iniparser_getint(cfgIni, "volume:talkvol", 2);
	theConfig.Iringvol = iniparser_getint(cfgIni, "volume:Iringvol", 2);
	theConfig.Italkvol = iniparser_getint(cfgIni, "volume:Italkvol", 2);
	theConfig.keyvol = iniparser_getint(cfgIni, "volume:keyvol", 1);
	theConfig.interphone = iniparser_getint(cfgIni, "volume:interphone", 0);
	theConfig.door1 = iniparser_getint(cfgIni, "volume:door1", 0);
	theConfig.door2 = iniparser_getint(cfgIni, "volume:door2", 0);

	//[display]
	theConfig.brightness = iniparser_getint(cfgIni, "display:brightness", 50);
	theConfig.contrast = iniparser_getint(cfgIni, "display:contrast", 50);
	theConfig.hue = iniparser_getint(cfgIni, "display:hue", 50);
	theConfig.brightness_c = iniparser_getint(cfgIni, "display:brightness_c", 50);
	theConfig.contrast_c = iniparser_getint(cfgIni, "display:contrast_c", 50);
	theConfig.hue_c = iniparser_getint(cfgIni, "display:hue_c", 50);
	theConfig.lcdout = iniparser_getint(cfgIni, "display:lcdout", 1);
	theConfig.lcdouttime = iniparser_getint(cfgIni, "display:lcdouttime", 0);
	theConfig.infoframe = iniparser_getint(cfgIni, "display:infoframe", 0);

	//[other]
	theConfig.language = iniparser_getint(cfgIni, "other:language", 0);
	theConfig.id = iniparser_getint(cfgIni, "other:id", 0);
	theConfig.open = iniparser_getint(cfgIni, "other:open", 0);

	//[user]
	theConfig.firstboot = iniparser_getint(cfgIni, "user:firstboot", 1);
	cfgSavingCount = 0;
}

void ConfigExit(void)
{
	iniparser_freedict(cfgIni);
	cfgIni = NULL;
}

user_iniparser_set_int(dictionary * ini, char * entry, int val)
{
	char buf[8];
	sprintf(buf, "%d", val);
	iniparser_set(ini, entry, buf);
}

static void ConfigSavePublic(void)
{
	FILE* f;
	char buf[8];

	//[record]
	user_iniparser_set_int(cfgIni, "record:auto", theConfig.zidong);
	user_iniparser_set_int(cfgIni, "record:md", theConfig.md);
	user_iniparser_set_int(cfgIni, "record:mdcam", theConfig.mdcam);
	user_iniparser_set_int(cfgIni, "record:mdsave", theConfig.mdsave);
	user_iniparser_set_int(cfgIni, "record:mdsensitive", theConfig.mdsensitive);
	user_iniparser_set_int(cfgIni, "record:mdtime", theConfig.mdtime);
	user_iniparser_set_int(cfgIni, "record:mdsyear", theConfig.mdsyear);
	user_iniparser_set_int(cfgIni, "record:mdsmonth", theConfig.mdsmonth);
	user_iniparser_set_int(cfgIni, "record:mdsday", theConfig.mdsday);
	user_iniparser_set_int(cfgIni, "record:mdshour", theConfig.mdshour);
	user_iniparser_set_int(cfgIni, "record:mdsmin", theConfig.mdsmin);
	user_iniparser_set_int(cfgIni, "record:mdssec",theConfig.mdssec);
	user_iniparser_set_int(cfgIni, "record:mdeyear", theConfig.mdeyear);
	user_iniparser_set_int(cfgIni, "record:mdemonth", theConfig.mdemonth);
	user_iniparser_set_int(cfgIni, "record:mdeday", theConfig.mdeday);
	user_iniparser_set_int(cfgIni, "record:mdehour",theConfig.mdehour);
	user_iniparser_set_int(cfgIni, "record:mdemin",theConfig.mdemin);
	user_iniparser_set_int(cfgIni, "record:mdesec", theConfig.mdesec);

	//[volume]
	user_iniparser_set_int(cfgIni, "volume:mute", theConfig.mute);
	user_iniparser_set_int(cfgIni, "volume:ringvol", theConfig.ringvol);
	user_iniparser_set_int(cfgIni, "volume:talkvol", theConfig.talkvol);
	user_iniparser_set_int(cfgIni, "volume:Iringvol", theConfig.Iringvol);
	user_iniparser_set_int(cfgIni, "volume:Italkvol", theConfig.Italkvol);
	user_iniparser_set_int(cfgIni, "volume:keyvol", theConfig.keyvol);
	user_iniparser_set_int(cfgIni, "volume:interphone", theConfig.interphone);
	user_iniparser_set_int(cfgIni, "volume:door1", theConfig.door1);
	user_iniparser_set_int(cfgIni, "volume:door2", theConfig.door2);

	//[display]
	user_iniparser_set_int(cfgIni, "display:brightness", theConfig.brightness);
	user_iniparser_set_int(cfgIni, "display:contrast", theConfig.contrast);
	user_iniparser_set_int(cfgIni, "display:hue", theConfig.hue);
	user_iniparser_set_int(cfgIni, "display:brightness_c", theConfig.brightness_c);
	user_iniparser_set_int(cfgIni, "display:contrast_c", theConfig.contrast_c);
	user_iniparser_set_int(cfgIni, "display:hue_c", theConfig.hue_c);
	user_iniparser_set_int(cfgIni, "display:lcdout", theConfig.lcdout);
	user_iniparser_set_int(cfgIni, "display:lcdouttime", theConfig.lcdouttime);
	user_iniparser_set_int(cfgIni, "display:infoframe", theConfig.infoframe);

	//[other]
	user_iniparser_set_int(cfgIni, "other:language", theConfig.language);
	user_iniparser_set_int(cfgIni, "other:id", theConfig.id);
	user_iniparser_set_int(cfgIni, "other:open", theConfig.open);
	
	//[user]
	user_iniparser_set_int(cfgIni, "user:firstboot", theConfig.firstboot);

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

static void* ConfigSaveTask(void* arg)
{
	char* filepath = CFG_PUBLIC_DRIVE ":/" INI_FILENAME;
	int savingCount;

	cfgIsSaving = true;

	do
	{
		savingCount = cfgSavingCount;

		ConfigSavePublic();

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

	}
	while (savingCount != cfgSavingCount);

	printf("ConfigSave................2222.................count = %d\n",savingCount);
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

	cfgSavingCount++;
	printf("ConfigSave.................1111................count = %d\n",cfgSavingCount);
	if (cfgIsSaving)
		return;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, ConfigSaveTask, NULL);
}
