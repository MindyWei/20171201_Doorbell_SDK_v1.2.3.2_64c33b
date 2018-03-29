#include <sys/ioctl.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "ite/itp.h"
#include "libxml/xpath.h"
#include "doorbell.h"
#include "scene.h"

typedef struct
{
    bool enabled;
    bool alarming;
    unsigned long time;
    int alarmDelay;
} GuardSensorStatus;

static GuardSensorStatus guardSensorStatusArray[GUARD_SENSOR_COUNT];

#ifdef CFG_GUARD_ENABLE
#ifdef CFG_GPIO_EXPANDER_ENABLE
static int guardSensorGpioTable[GUARD_SENSOR_COUNT] = { CFG_GPIO_GUARD_SENSORS };
#else
static int guardSensorGpioTable[GUARD_SENSOR_COUNT] = { CFG_GPIO_GUARD_SENSORS };
#endif
#endif

static bool guardSensorAlarmedTable[GUARD_SENSOR_COUNT];
static unsigned int guardSensorLastValue = 0;
static bool guardAlarming;

#define MAX_GUARD_LOG_COUNT    999
#define GUARD_ALARM_DELAY      (1 * 1000 / MS_PER_FRAME)  // 1 sec

int guardLogCount = 0;
GuardLog guardLogArray[MAX_GUARD_LOG_COUNT] = {0};
static pthread_mutex_t guardLogMutex;

#ifdef CFG_GUARD_ENABLE
#ifdef CFG_GPIO_EXPANDER_ENABLE
static void GuardSensorDeferIntrHandler( void *pvParameter1, uint32_t ulParameter2)
{
    int pin;
    int DiffPins;
    DiffPins = itpIOExpanderGetInputStatus(IOEXPANDER_0);

    for (pin = 0; pin < GUARD_SENSOR_COUNT; pin++)
    {
        if (DiffPins & (0x1 << pin) )
        {
            if (itpIOExpanderGet(IOEXPANDER_0,pin))
                guardSensorLastValue |= 0x1 << pin;
            else
                guardSensorLastValue &= ~(0x1 << pin);
            //ithPrintf("itpIOExpanderGet(%d)= %d,DiffPins=0x%x,guardSensorLastValue=0x%x\n", pin, itpIOExpanderGet(pin), DiffPins, guardSensorLastValue);
            break;
        }
    }
}
#else
static void GuardSensorIntrHandler(unsigned int pin, void* arg)
{
    int i;
    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensorGpioTable[i] == pin)
        {
            if (ithGpioGet(pin))
                guardSensorLastValue |= 0x1 << i;
            else
                guardSensorLastValue &= ~(0x1 << i);
            break;
        }
    }
    ithPrintf("ithGpioGet(%d)=0x%x,guardSensorLastValue=0x%x\n", pin,ithGpioGet(pin), guardSensorLastValue);     
    ithGpioClearIntr(pin);
}
#endif
#endif // CFG_GUARD_ENABLE

static void GuardWriteLog(GuardSensor sensor, GuardStatus status, unsigned long timestamp)
{
    GuardLog* entry;

    pthread_mutex_lock(&guardLogMutex);

    if (guardLogCount >= MAX_GUARD_LOG_COUNT)
        GuardDeleteLog(guardLogCount - 1);

    entry = &guardLogArray[guardLogCount];

    memset(entry, 0, sizeof (GuardLog));

    entry->timestamp = timestamp;
    entry->sensor = sensor;
    entry->status = status;

    guardLogCount++;
    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);

    ConfigSave();
    pthread_mutex_unlock(&guardLogMutex);
}

static void GuardWriteSyncLog(GuardLog* log)
{
    GuardLog* entry;

    pthread_mutex_lock(&guardLogMutex);

    if (guardLogCount >= MAX_GUARD_LOG_COUNT)
        GuardDeleteLog(guardLogCount - 1);

    entry = &guardLogArray[guardLogCount];

    memcpy(entry, log, sizeof (GuardLog));

    guardLogCount++;
    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);

    ConfigSave();
    pthread_mutex_unlock(&guardLogMutex);
}

static void GuardEnableSensor(GuardSensor sensor)
{
    if (!guardSensorStatusArray[sensor].enabled)
    {
        struct timeval tv;

    #ifdef CFG_GUARD_ENABLE
    #ifdef CFG_GPIO_EXPANDER_ENABLE
        unsigned int pin = guardSensorGpioTable[sensor];
        //itpRegisterIOExpanderIntrHandler(pin,GuardSensorIntrHandler);
        itpRegisterIOExpanderDeferIntrHandler(GuardSensorDeferIntrHandler);
        itpIOExpanderSetIn(IOEXPANDER_0,pin);
        itpIOExpanderSet(IOEXPANDER_0,pin);

        if (itpIOExpanderGet(IOEXPANDER_0,pin))
        {
            guardSensorLastValue |= 0x1 << pin;         
        }  
    #else
        unsigned int pin = guardSensorGpioTable[sensor];
        ithGpioSetMode(pin, ITH_GPIO_MODE0);
        ithGpioSetIn(pin);

        ithGpioRegisterIntrHandler(pin, GuardSensorIntrHandler, NULL);
        ithGpioCtrlDisable(pin, ITH_GPIO_INTR_BOTHEDGE);      //single edge
        ithGpioCtrlDisable(pin, ITH_GPIO_INTR_LEVELTRIGGER);   //Set interrupt mode as "edge trigger"
        ithGpioCtrlEnable(pin, ITH_GPIO_INTR_TRIGGERFALLING); // set to falling edge
        ithGpioEnableIntr(pin);

        if (theConfig.guard_sensor_initvalues[sensor] == '1')
        {
            ithGpioCtrlEnable(pin, ITH_GPIO_PULL_ENABLE); 	//To enable the PULL function of this GPIO pin
	        ithGpioCtrlEnable(pin, ITH_GPIO_PULL_UP);		//To set "PULL UP" of this GPIO pin
        }
        else
        {
            ithGpioCtrlEnable(pin, ITH_GPIO_PULL_ENABLE); 
	        ithGpioCtrlDisable(pin, ITH_GPIO_PULL_UP);		
        }
        ithGpioEnable(pin);

        if (ithGpioGet(pin))
        {
            guardSensorLastValue |= 0x1 << sensor;         
        }  
    #endif
    #endif // CFG_GUARD_ENABLE

        guardSensorStatusArray[sensor].enabled = true;
        guardSensorStatusArray[sensor].alarmDelay = -1;

        gettimeofday(&tv, NULL);
        guardSensorStatusArray[sensor].time = tv.tv_sec;

        GuardWriteLog(sensor, GUARD_ENABLE, tv.tv_sec);
        EventWriteGuardLog(sensor, GUARD_ENABLE, tv.tv_sec);
    }
}

static void GuardDisableSensor(GuardSensor sensor)
{
    if (guardSensorStatusArray[sensor].enabled)
    {
        struct timeval tv;

    #ifdef CFG_GUARD_ENABLE
    #ifdef CFG_GPIO_EXTENDER_ENABLE
        unsigned int pin = guardSensorGpioTable[sensor];
    
    #else
        unsigned int pin = guardSensorGpioTable[sensor];
        ithGpioDisableIntr(pin);
    #endif 
    #endif // CFG_GUARD_ENABLE

        gettimeofday(&tv, NULL);

        guardSensorStatusArray[sensor].enabled = false;
        guardSensorStatusArray[sensor].alarmDelay = -1;
        guardSensorStatusArray[sensor].time = tv.tv_sec;

        if (guardSensorStatusArray[sensor].alarming)
        {
            guardSensorStatusArray[sensor].alarming = false;
            GuardWriteLog(sensor, GUARD_UNALARM, tv.tv_sec);
            EventWriteGuardLog(sensor, GUARD_UNALARM, tv.tv_sec);
        }
        GuardWriteLog(sensor, GUARD_DISABLE, tv.tv_sec);
        EventWriteGuardLog(sensor, GUARD_DISABLE, tv.tv_sec);
    }
}

void GuardInit(void)
{
    int i;
    struct timeval tv;
    pthread_mutexattr_t mattr; 

    guardSensorLastValue = 0;
    //guardLogCount = 0;
    guardAlarming = false;

    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&guardLogMutex, &mattr);

    gettimeofday(&tv, NULL);

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        guardSensorStatusArray[i].enabled = false;
        guardSensorStatusArray[i].time = tv.tv_sec;
        guardSensorStatusArray[i].alarming = false; //Benson
        guardSensorStatusArray[i].alarmDelay = -1;
    }
    GuardSetMode(theConfig.guard_mode);

#ifdef CFG_GUARD_ENABLE
#ifdef CFG_GPIO_EXPANDER_ENABLE
    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensorStatusArray[i].enabled)
        {
            if (itpIOExpanderGet(IOEXPANDER_0,guardSensorGpioTable[i]))
                {
                    guardSensorLastValue |= 0x1 << i;         
                }
        }
    }
#else
    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensorStatusArray[i].enabled)
        {
            if (ithGpioGet(guardSensorGpioTable[i]))
                guardSensorLastValue |= 0x1 << i;
        }
    }
#endif
#endif // CFG_GUARD_ENABLE
}

void GuardExit(void)
{
    pthread_mutex_destroy(&guardLogMutex);
}

void GuardSetMode(GuardMode mode)
{
    int i;
    char* guardSensors = NULL;

    switch (mode)
    {
    case GUARD_INDOOR:
        guardSensors = theConfig.guard_indoor_sensors;
        break;

    case GUARD_OUTDOOR:
        guardSensors = theConfig.guard_outdoor_sensors;
        break;

    case GUARD_SLEEP:
        guardSensors = theConfig.guard_sleep_sensors;
        break;
    }

    if (guardSensors)
    {
        for (i = 0; i < GUARD_SENSOR_COUNT; i++)
        {
            if (guardSensors[i] == '1')
                GuardEnableSensor(i);
            else
                GuardDisableSensor(i);
        }
    }
    else
    {
        for (i = 0; i < GUARD_SENSOR_COUNT; i++)
        {
            GuardDisableSensor(i);
        }
    }
    theConfig.guard_mode = mode;
}

int GuardCheck(void)
{
    uint8_t i;
    char* guardSensors;
    int result = 0;

    switch (theConfig.guard_mode)
    {
    case GUARD_INDOOR:
        guardSensors = theConfig.guard_indoor_sensors;
        break;

    case GUARD_OUTDOOR:
        guardSensors = theConfig.guard_outdoor_sensors;
        break;

    case GUARD_SLEEP:
        guardSensors = theConfig.guard_sleep_sensors;
        break;

    default:
        return 0;
    }

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensors[i] == '1')
        {
            if (guardSensorLastValue & (0x1 << i))
            { 
                if (theConfig.guard_sensor_initvalues[i] == '0')
                {
                    if (guardSensorStatusArray[i].alarmDelay == -1)
                    {
                        guardSensorStatusArray[i].alarmDelay = GUARD_ALARM_DELAY;
                    }
                    else if (guardSensorStatusArray[i].alarmDelay > 0)
                    {
                        guardSensorStatusArray[i].alarmDelay--;
                    }
                    
                    if (guardSensorStatusArray[i].alarmDelay == 0)
                    {
                        if (!guardSensorStatusArray[i].alarming)
                        {
                            struct timeval tv;
                            gettimeofday(&tv, NULL);

                            guardSensorStatusArray[i].alarming = true;
                            guardSensorStatusArray[i].time = tv.tv_sec;
                            GuardWriteLog(i, GUARD_ALARM, tv.tv_sec);
                            EventWriteGuardLog(i, GUARD_ALARM, tv.tv_sec);
                        }
                        if (i == GUARD_ROB)
                        {
                            result = 1;
                        }
                        else if (result == 0)
                        {
                            result = -1;
                        }
                    }
                }
                else
                {
                    if (guardSensorStatusArray[i].alarming)
                    {
                        if (i == GUARD_ROB)
                        {
                            result = 1;
                        }
                        else if (result == 0)
                        {
                            result = -1;
                        }
                    }
                    else
                    {
                        guardSensorStatusArray[i].alarmDelay = -1;
                    }
                }
            }
            else
            {
                if (theConfig.guard_sensor_initvalues[i] == '1')
                {
                    if (guardSensorStatusArray[i].alarmDelay == -1)
                    {
                        guardSensorStatusArray[i].alarmDelay = GUARD_ALARM_DELAY;
                    }
                    else if (guardSensorStatusArray[i].alarmDelay > 0)
                    {
                        guardSensorStatusArray[i].alarmDelay--;
                    }
                    
                    if (guardSensorStatusArray[i].alarmDelay == 0)
                    {
                        if (!guardSensorStatusArray[i].alarming)
                        {
                            struct timeval tv;
                            gettimeofday(&tv, NULL);

                            guardSensorStatusArray[i].alarming = true;
                            guardSensorStatusArray[i].time = tv.tv_sec;
                            GuardWriteLog(i, GUARD_ALARM, tv.tv_sec);
                            EventWriteGuardLog(i, GUARD_ALARM, tv.tv_sec);
                        }
                        if (i == GUARD_ROB)
                        {
                            result = 1;
                        }
                        else if (result == 0)
                        {
                            result = -1;
                        }
                    }
                }
                else
                {
                    if (guardSensorStatusArray[i].alarming)
                    {
                        if (i == GUARD_ROB)
                        {
                            result = 1;
                        }
                        else if (result == 0)
                        {
                            result = -1;
                        }
                    }
                    else
                    {
                        guardSensorStatusArray[i].alarmDelay = -1;
                    }
                }
            }
        }
    }
    return result;
}

GuardStatus GuardGetSensorStatus(GuardSensor sensor)
{
    GuardSensorStatus* status = &guardSensorStatusArray[sensor];

    if (status->enabled && status->alarming)
        return GUARD_ALARM;
    else if (status->enabled)
        return GUARD_ENABLE;
    else
        return GUARD_DISABLE;
}

unsigned long GuardGetSensorTime(GuardSensor sensor)
{
    return guardSensorStatusArray[sensor].time;
}

int GuardGetLogCount(void)
{
    return guardLogCount;
}

GuardLog* GuardGetLog(int index)
{
    int i;

    if (guardLogCount == 0 || index >= guardLogCount)
        return NULL;

    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);
    assert(index < guardLogCount);

    i = guardLogCount - 1 - index;
    return &guardLogArray[i];
}

void GuardDeleteLog(int index)
{
    int i, endIndex;

    if (guardLogCount == 0 || index >= guardLogCount)
        return;

    pthread_mutex_lock(&guardLogMutex);

    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);
    assert(index < guardLogCount);

    i = guardLogCount - 1 - index;
    endIndex = guardLogCount - 1;

    assert(i <= endIndex);

    memmove(&guardLogArray[i], &guardLogArray[i + 1], sizeof(GuardLog) * (endIndex - i));

    guardLogCount--;
    assert(guardLogCount >= 0);

    pthread_mutex_unlock(&guardLogMutex);
}

void GuardClearLog(void)
{
    guardLogCount = 0;
    ConfigSave();
}

bool GuardIsAlarming(void)
{
    return guardAlarming;
}

void GuardSetAlarming(bool alarming)
{
    guardAlarming = alarming;
}

void GuardReceiveSyncLog(char* xml, int size)
{
    xmlDocPtr doc;

    doc = xmlParseMemory(xml, size);
    if (doc)
    {
        xmlXPathContext* xpathCtx = xmlXPathNewContext(doc);
        if (xpathCtx)
        {
            xmlXPathObject* xpathObj;

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/GuardLogSyncList/item", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                int i, len;

                len = xpathObj->nodesetval->nodeNr;
                for (i = 0; i < len; i++)
                {
                    GuardLog log;
                    xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                    xmlAttr* attribute = node->properties;
                    char area[4], building[4], unit[4], floor[4], room[4], ext[4];

                    area[0] = building[0] = unit[0] = floor[0] = room[0] = ext[0] = '\0';

                    memset(&log, 0, sizeof (GuardLog));

                    while (attribute && attribute->name && attribute->children)
                    {
                        xmlChar* value = NULL;

                        if (strcmp(attribute->name, "addr") == 0)
                        {
                            char* token, *saveptr;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            token = strtok_r(value, "-", &saveptr);
                            if (token)
                            {
                                area[0] = token[0];
                                area[1] = token[1];
                                area[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    building[0] = token[0];
                                    building[1] = token[1];
                                    building[2] = '\0';

                                    token = strtok_r(NULL, "-", &saveptr);
                                    if (token)
                                    {
                                        unit[0] = token[0];
                                        unit[1] = token[1];
                                        unit[2] = '\0';

                                        token = strtok_r(NULL, "-", &saveptr);
                                        if (token)
                                        {
                                            floor[0] = token[0];
                                            floor[1] = token[1];
                                            floor[2] = '\0';

                                            token = strtok_r(NULL, "-", &saveptr);
                                            if (token)
                                            {
                                                room[0] = token[0];
                                                room[1] = token[1];
                                                room[2] = '\0';

                                                token = strtok_r(NULL, "-", &saveptr);
                                                if (token)
                                                {
                                                    ext[0] = token[0];
                                                    ext[1] = token[1];
                                                    ext[2] = '\0';
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                        else if (strcmp(attribute->name, "time") == 0)
                        {
                            struct tm tm;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            strptime(value, "%Y-%m-%d %H:%M:%S", &tm);

                            log.timestamp = (unsigned long)mktime(&tm);
                        }
                        else if (strcmp(attribute->name, "type") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            if (strcmp(value, "emergency") == 0)
                                log.type = GUARD_TYPE_EMERGENCY;
                            else if (strcmp(value, "infrared") == 0)
                                log.type = GUARD_TYPE_INFRARED;
                            else if (strcmp(value, "door") == 0)
                                log.type = GUARD_TYPE_DOOR;
                            else if (strcmp(value, "window") == 0)
                                log.type = GUARD_TYPE_WINDOW;
                            else if (strcmp(value, "smoke") == 0)
                                log.type = GUARD_TYPE_SMOKE;
                            else if (strcmp(value, "gas") == 0)
                                log.type = GUARD_TYPE_GAS;
                            else if (strcmp(value, "area") == 0)
                                log.type = GUARD_TYPE_AREA;
                            else if (strcmp(value, "rob") == 0)
                                log.type = GUARD_TYPE_ROB;
                            else if (strcmp(value, "opendoor") == 0)
                                log.type = GUARD_TYPE_OPENDOOR;
                        }
                        else if (strcmp(attribute->name, "state") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            log.state = atoi(value);
                        }
                        xmlFree(value);

                        attribute = attribute->next;
                    }
                    if (!strcmp(area, theDeviceInfo.area) &&
                        !strcmp(building, theDeviceInfo.building) &&
                        !strcmp(unit, theDeviceInfo.unit) &&
                        !strcmp(floor, theDeviceInfo.floor) &&
                        !strcmp(room, theDeviceInfo.room) &&
                        !strcmp(ext, theDeviceInfo.ext))
                    {
                        GuardWriteSyncLog(&log);
                    }
                }
            }
            xmlXPathFreeObject(xpathObj);
            xmlXPathFreeContext(xpathCtx);
        }
        xmlFreeDoc(doc);
    }
}
