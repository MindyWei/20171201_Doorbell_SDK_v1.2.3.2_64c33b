#include "ite/itu.h"

extern bool MainOnEnter(ITUWidget* widget, char* param);
extern bool MainOnTimer(ITUWidget* widget, char* param);
extern bool MainTimerMeterOnChanged(ITUWidget* widget, char* param);
extern bool msgChk2ButtonOnPress(ITUWidget* widget, char* param);
extern bool mainTempUpButtonOnPress(ITUWidget* widget, char* param);
extern bool mainTempUpButtonLongPress(ITUWidget* widget, char* param);
extern bool mainTempDownButtonOnPress(ITUWidget* widget, char* param);
extern bool mainTempDownButtonLongPress(ITUWidget* widget, char* param);
extern bool mainTempWaterOnPress(ITUWidget* widget, char* param);
extern bool mainTempFanOnPress(ITUWidget* widget, char* param);
extern bool mainTempCycleOnPress(ITUWidget* widget, char* param);
extern bool mainTempFireOnPress(ITUWidget* widget, char* param);
extern bool msgChkButtonOnPress(ITUWidget* widget, char* param);
extern bool MainTempMeterOnChanged(ITUWidget* widget, char* param);
extern bool E1PlayAudio(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "MainOnEnter", MainOnEnter,
    "MainOnTimer", MainOnTimer,
    "MainTimerMeterOnChanged", MainTimerMeterOnChanged,
    "msgChk2ButtonOnPress", msgChk2ButtonOnPress,
    "mainTempUpButtonOnPress", mainTempUpButtonOnPress,
    "mainTempUpButtonLongPress", mainTempUpButtonLongPress,
    "mainTempDownButtonOnPress", mainTempDownButtonOnPress,
    "mainTempDownButtonLongPress", mainTempDownButtonLongPress,
    "mainTempWaterOnPress", mainTempWaterOnPress,
    "mainTempFanOnPress", mainTempFanOnPress,
    "mainTempCycleOnPress", mainTempCycleOnPress,
    "mainTempFireOnPress", mainTempFireOnPress,
    "msgChkButtonOnPress", msgChkButtonOnPress,
    "MainTempMeterOnChanged", MainTempMeterOnChanged,
    "E1PlayAudio", E1PlayAudio,
    NULL, NULL
};
