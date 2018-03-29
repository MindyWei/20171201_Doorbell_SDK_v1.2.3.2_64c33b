#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "pthread.h"

static int addLongPress = 0;
static int subLongPress = 0;

static int firstTempAngle = 0;
static int curTempAngle = 0;
static int relativeTempAngle = 0;

static int lastTimerAngle = 0;
static int curTimerAngle = 0;

static ITUCoverFlow *mainCoverFlow = 0;
static ITUBackground *mainTimer1Background = 0;
static ITUBackground *mainTimer2Background = 0;
static ITUBackground *mainTimer3Background = 0;
static ITUBackground *mainTimer4Background = 0;
static ITUBackground *mainTimer5Background = 0;
static ITUBackground *mainTimer6Background = 0;
static ITUBackground *mainTimer7Background = 0;
static ITUBackground *mainTimer8Background = 0;
static ITUBackground *mainTimer9Background = 0;
static ITUBackground *mainTimer10Background = 0;
static ITUBackground *mainTimer11Background = 0;
static ITUBackground *mainTimer12Background = 0;
static ITUBackground *mainTimer13Background = 0;
static ITUBackground *mainTimer14Background = 0;
static ITUBackground *mainTimer15Background = 0;
static ITUBackground *mainTimer16Background = 0;
static ITUBackground *mainTimer17Background = 0;
static ITUBackground *mainTimer18Background = 0;
static ITUBackground *mainTimer19Background = 0;
static ITUBackground *mainTimer20Background = 0;
static ITUBackground *mainTimer21Background = 0;
static ITUBackground *mainTimer22Background = 0;
static ITUBackground *mainTimer23Background = 0;
static ITUBackground *mainTimer24Background = 0;
static ITUBackground *redFireBackground = 0;
static ITUBackground *mainTimerBlackBackground = 0;
static ITUSprite *mainTempLSprite = 0;
static ITUSprite *mainTempRSprite = 0;
static ITUSprite *mainTempCycleSprite = 0;
static ITUSprite *mainTempFanSprite = 0;
static ITUSprite *mainTempWaterSprite = 0;
static ITUAnimation *mainTempFireAnimation = 0;


#define PRESS_CNT 5
static void GotoMessageChkLayer(void)
{
    ITULayer* messageChkLayer = ituSceneFindWidget(&theScene, "messageChkLayer");
    assert(messageChkLayer);
    ituLayerGoto(messageChkLayer);
}

static void GotoErrorCodeLayer(void)
{
	ITULayer* errCodeLayer = ituSceneFindWidget(&theScene, "errCodeLayer");
	assert(errCodeLayer);
	ituLayerGoto(errCodeLayer);
}

void TempNumChange()
{
	int dLnum = 0, dRnum = 0; // L:7~3(0=7, 1=6..., 5=3); R:9~0(0=9, 1=8..., 9=0)
	if (relativeTempAngle == 0)
	{
		dLnum = 3; dRnum = 5;//9; // 7, 0
	}
	else if (relativeTempAngle > 0)
	{
		switch (relativeTempAngle)
		{
		case 1:
		case 2:
		case 3:
		case 4:
			dLnum = 3; dRnum = relativeTempAngle + 5;  // 3, 9~6
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			dLnum = 4; dRnum = relativeTempAngle - 5;  // 4, 9~0
			break;
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
			dLnum = 5; dRnum = relativeTempAngle - 15;  // 5, 9~0
			break;
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
			dLnum = 6; dRnum = relativeTempAngle - 25;  // 6, 9~0
			break;
		default:
			dLnum = 7; dRnum = 0;
			break;
		}
	}

	ituSpriteGoto(mainTempLSprite, dLnum);
	ituSpriteGoto(mainTempRSprite, dRnum);
}

bool msgChkButtonOnPress(ITUWidget* widget, char* param)
{
    static int cnt = 0;
    
    printf("%s %d", __func__, __LINE__);
    cnt++;
    if (PRESS_CNT == cnt)
    {
        cnt = 0;
        GotoMessageChkLayer();
    }
	return true;
}

bool msgChk2ButtonOnPress(ITUWidget* widget, char* param)
{
	static int cnt = 0;

	printf("%s %d", __func__, __LINE__);
	cnt++;
	if (PRESS_CNT == cnt)
	{
		cnt = 0;
		GotoErrorCodeLayer();
	}
	return true;
}

static void* PlayKeySound2Task(void* arg)
{
	static int cnt = 300;
    for (;;)
    {
		if(cnt-- <= 0)
		{
			AudioPlayKeySound();
			cnt = 300;
		}
		else
		    AudioPlayKeySound2();
		usleep(10000);
	}
	return NULL;
}

bool MainOnEnter(ITUWidget* widget, char* param)
{
    if (!mainCoverFlow)
    {
		pthread_t task;
		pthread_create(&task, NULL, PlayKeySound2Task, NULL);
		
        mainCoverFlow = ituSceneFindWidget(&theScene, "mainCoverFlow");
        assert(mainCoverFlow);

        mainTimer1Background = ituSceneFindWidget(&theScene, "mainTimer1Background");
        assert(mainTimer1Background);

        mainTimer2Background = ituSceneFindWidget(&theScene, "mainTimer2Background");
        assert(mainTimer2Background);

        mainTimer3Background = ituSceneFindWidget(&theScene, "mainTimer3Background");
        assert(mainTimer3Background);

        mainTimer4Background = ituSceneFindWidget(&theScene, "mainTimer4Background");
        assert(mainTimer4Background);

        mainTimer5Background = ituSceneFindWidget(&theScene, "mainTimer5Background");
        assert(mainTimer5Background);

        mainTimer6Background = ituSceneFindWidget(&theScene, "mainTimer6Background");
        assert(mainTimer6Background);

        mainTimer7Background = ituSceneFindWidget(&theScene, "mainTimer7Background");
        assert(mainTimer7Background);

        mainTimer8Background = ituSceneFindWidget(&theScene, "mainTimer8Background");
        assert(mainTimer8Background);

        mainTimer9Background = ituSceneFindWidget(&theScene, "mainTimer9Background");
        assert(mainTimer9Background);

        mainTimer10Background = ituSceneFindWidget(&theScene, "mainTimer10Background");
        assert(mainTimer10Background);

        mainTimer11Background = ituSceneFindWidget(&theScene, "mainTimer11Background");
        assert(mainTimer11Background);

        mainTimer12Background = ituSceneFindWidget(&theScene, "mainTimer12Background");
        assert(mainTimer12Background);

        mainTimer13Background = ituSceneFindWidget(&theScene, "mainTimer13Background");
        assert(mainTimer13Background);

        mainTimer14Background = ituSceneFindWidget(&theScene, "mainTimer14Background");
        assert(mainTimer14Background);

        mainTimer15Background = ituSceneFindWidget(&theScene, "mainTimer15Background");
        assert(mainTimer15Background);

        mainTimer16Background = ituSceneFindWidget(&theScene, "mainTimer16Background");
        assert(mainTimer16Background);

        mainTimer17Background = ituSceneFindWidget(&theScene, "mainTimer17Background");
        assert(mainTimer17Background);

        mainTimer18Background = ituSceneFindWidget(&theScene, "mainTimer18Background");
        assert(mainTimer18Background);

        mainTimer19Background = ituSceneFindWidget(&theScene, "mainTimer19Background");
        assert(mainTimer19Background);

        mainTimer20Background = ituSceneFindWidget(&theScene, "mainTimer20Background");
        assert(mainTimer20Background);

        mainTimer21Background = ituSceneFindWidget(&theScene, "mainTimer21Background");
        assert(mainTimer21Background);

        mainTimer22Background = ituSceneFindWidget(&theScene, "mainTimer22Background");
        assert(mainTimer22Background);

        mainTimer23Background = ituSceneFindWidget(&theScene, "mainTimer23Background");
        assert(mainTimer23Background);

        mainTimer24Background = ituSceneFindWidget(&theScene, "mainTimer24Background");
        assert(mainTimer24Background);

        redFireBackground = ituSceneFindWidget(&theScene, "redFireBackground");
        assert(redFireBackground);
        
      /*  mainTimerBlackBackground = ituSceneFindWidget(&theScene, "mainTimerBlackBackground");
        assert(mainTimerBlackBackground);*/

        mainTempLSprite = ituSceneFindWidget(&theScene, "mainTempLSprite");
        assert(mainTempLSprite);

        mainTempRSprite = ituSceneFindWidget(&theScene, "mainTempRSprite");
        assert(mainTempRSprite);

		mainTempCycleSprite = ituSceneFindWidget(&theScene, "mainTempCycleSprite");
		assert(mainTempCycleSprite);

		mainTempFanSprite = ituSceneFindWidget(&theScene, "mainTempFanSprite");
		assert(mainTempFanSprite);

		mainTempWaterSprite = ituSceneFindWidget(&theScene, "mainTempWaterSprite");
		assert(mainTempWaterSprite);

		mainTempFireAnimation = ituSceneFindWidget(&theScene, "mainTempFireAnimation");
		assert(mainTempFireAnimation);
    }

	ituCoverFlowGoto(mainCoverFlow, 0);
	ituSpriteGoto(mainTempLSprite, 3);
	ituSpriteGoto(mainTempRSprite, 5);
	ScreenSaverRefresh();

	firstTempAngle = 0;
 	relativeTempAngle = 0; 
	lastTimerAngle = -1;

   return true;
}

#define MAX_VALUE 35

bool MainTempMeterOnChanged(ITUWidget* widget, char* param)
{
	curTempAngle = atoi(param);
	//printf("curTempAngle = %d, firstTempAngle = %d, relativeTempAngle = %d\n", curTempAngle, firstTempAngle, relativeTempAngle);
    AudioPlayKeySound2();
	if (abs(curTempAngle-firstTempAngle)>1) {
		firstTempAngle = curTempAngle;
	} else  {
		relativeTempAngle -= firstTempAngle - curTempAngle; // curTempAngle - firstTempAngle; //counterclockwise
		if (relativeTempAngle >= MAX_VALUE)
			relativeTempAngle = MAX_VALUE;
		else if (relativeTempAngle<0) 
            relativeTempAngle = 0;
        
		firstTempAngle = curTempAngle;
	}
	TempNumChange();
	return true;
}

#define TIME_CHK_ITEMS 24

bool MainTimerMeterOnChanged(ITUWidget* widget, char* param)
{
    static int isVisible[TIME_CHK_ITEMS];
    static int isDone  = 0;
    int i = 0;
    int isShowRedFireIcon = 0;
	AudioPlayKeySound2();

    if (isDone == 0)
    {
        isDone = 1;
        for(i = 0; i < TIME_CHK_ITEMS; i++)
            isVisible[i] = 0;
    }
	curTimerAngle = atoi(param);
	if (curTimerAngle != lastTimerAngle) {
		lastTimerAngle = curTimerAngle;
		switch (curTimerAngle) {
		case 0:
			if (ituWidgetIsVisible(mainTimer1Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer1Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer1Background, true);
			}
			break;
		case 1:
			if (ituWidgetIsVisible(mainTimer2Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer2Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer2Background, true);
			}
			break;
		case 2:
			if (ituWidgetIsVisible(mainTimer3Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer3Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer3Background, true);
			}
			break;
		case 3:
			if (ituWidgetIsVisible(mainTimer4Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer4Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer4Background, true);
			}
			break;
		case 4:
			if (ituWidgetIsVisible(mainTimer5Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer5Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer5Background, true);
			}
			break;
		case 5:
			if (ituWidgetIsVisible(mainTimer6Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer6Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer6Background, true);
			}
			break;
		case 6:
			if (ituWidgetIsVisible(mainTimer7Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer7Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer7Background, true);
			}
			break;
		case 7:
			if (ituWidgetIsVisible(mainTimer8Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer8Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer8Background, true);
			}
			break;
		case 8:
			if (ituWidgetIsVisible(mainTimer9Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer9Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer9Background, true);
			}
			break;
		case 9:
			if (ituWidgetIsVisible(mainTimer10Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer10Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer10Background, true);
			}
			break;
		case 10:
			if (ituWidgetIsVisible(mainTimer11Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer11Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer11Background, true);
			}
			break;
		case 11:
			if (ituWidgetIsVisible(mainTimer12Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer12Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer12Background, true);
			}
			break;
		case 12:
			if (ituWidgetIsVisible(mainTimer13Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer13Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer13Background, true);
			}
			break;
		case 13:
			if (ituWidgetIsVisible(mainTimer14Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer14Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer14Background, true);
			}
			break;
		case 14:
			if (ituWidgetIsVisible(mainTimer15Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer15Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer15Background, true);
			}
			break;
		case 15:
			if (ituWidgetIsVisible(mainTimer16Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer16Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer16Background, true);
			}
			break;
		case 16:
			if (ituWidgetIsVisible(mainTimer17Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer17Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer17Background, true);
			}
			break;
		case 17:
			if (ituWidgetIsVisible(mainTimer18Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer18Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer18Background, true);
			}
			break;
		case 18:
			if (ituWidgetIsVisible(mainTimer19Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer19Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer19Background, true);
			}
			break;
		case 19:
			if (ituWidgetIsVisible(mainTimer20Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer20Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer20Background, true);
			}
			break;
		case 20:
			if (ituWidgetIsVisible(mainTimer21Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer21Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer21Background, true);
			}
			break;
		case 21:
			if (ituWidgetIsVisible(mainTimer22Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer22Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer22Background, true);
			}
			break;
		case 22:
			if (ituWidgetIsVisible(mainTimer23Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer23Background, false);
			}
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer23Background, true);
			}
			break;
		case 23:
			if (ituWidgetIsVisible(mainTimer24Background)==true)
			{
				isVisible[curTimerAngle] = 0;
				ituWidgetSetVisible(mainTimer24Background, false);
		    }
			else
			{
				isVisible[curTimerAngle] = 1;
				ituWidgetSetVisible(mainTimer24Background, true);
			}
			break;
		}
	}

    for (i = 0; i < TIME_CHK_ITEMS; i++)
    {
        isShowRedFireIcon += isVisible[i];
    }
    
    if (isShowRedFireIcon == TIME_CHK_ITEMS)
    {
        printf("%s ShowRedFireIcon !\n", __func__);
        ituWidgetSetVisible(redFireBackground, true);
        //ituWidgetSetVisible(mainTimerBlackBackground, true);
        
    }
    else
    {
        ituWidgetSetVisible(redFireBackground, false);
        //ituWidgetSetVisible(mainTimerBlackBackground, false);
    }
        
    
	printf("curTimerAngle = %d\n", curTimerAngle);

    return true;
}

bool mainTempWaterOnPress(ITUWidget* widget, char* param)
{
	if (mainTempWaterSprite->playing == 1)
	{
		ituSpriteStop(mainTempWaterSprite);
		ituWidgetSetVisible(mainTempWaterSprite, false);
	}
	else
	{
		ituWidgetSetVisible(mainTempWaterSprite, true);
		ituSpritePlay(mainTempWaterSprite, -1);
	}
	return true;
}
bool mainTempFanOnPress(ITUWidget* widget, char* param)
{
	if (mainTempFanSprite->playing == 1)
	{
		ituSpriteStop(mainTempFanSprite);
		ituWidgetSetVisible(mainTempFanSprite, false);
	}
		
	else
	{
		ituWidgetSetVisible(mainTempFanSprite, true);
		ituSpritePlay(mainTempFanSprite, -1);
	}
	return true;
}
bool mainTempCycleOnPress(ITUWidget* widget, char* param)
{
	if (mainTempCycleSprite->playing == 1)
	{
		ituSpriteStop(mainTempCycleSprite);
		ituWidgetSetVisible(mainTempCycleSprite, false);
	}
		
	else
	{
		ituWidgetSetVisible(mainTempCycleSprite, true);
		ituSpritePlay(mainTempCycleSprite, -1);
	}
	return true;
}
bool mainTempFireOnPress(ITUWidget* widget, char* param)
{
	if (mainTempFireAnimation->playing == 1)
	{
		ituAnimationStop(mainTempFireAnimation);
		ituWidgetSetVisible(mainTempFireAnimation, false);
	}

	else
	{
		ituWidgetSetVisible(mainTempFireAnimation, true);
		ituAnimationPlay(mainTempFireAnimation, 0);
	}
	return true;
}
bool mainTempUpButtonOnPress(ITUWidget* widget, char* param)
{
	if (addLongPress == 1) {
	}
	else
	{
		if (relativeTempAngle <= MAX_VALUE)
		{
			++relativeTempAngle;
			TempNumChange();
		}
	}
	addLongPress = 0;
	return true;
}
bool mainTempUpButtonLongPress(ITUWidget* widget, char* param)
{
	printf("Long press");
	addLongPress = 1;
	return true;
}
bool mainTempDownButtonOnPress(ITUWidget* widget, char* param)
{
	if (subLongPress == 1) {
	}
	else
	{
		if (relativeTempAngle > 0)
		{
			--relativeTempAngle;
			TempNumChange();
		}
	}
	subLongPress = 0;
	return true;
}
bool mainTempDownButtonLongPress(ITUWidget* widget, char* param)
{
	printf("Long press");
	subLongPress = 1;
	return true;
}
bool MainOnTimer(ITUWidget* widget, char* param)
{
	bool updated = false;
	if (addLongPress == 1 || subLongPress == 1) 
	{
		relativeTempAngle = (addLongPress) ? relativeTempAngle + 1 : relativeTempAngle - 1;
		if (relativeTempAngle>MAX_VALUE) relativeTempAngle = MAX_VALUE;
		else if (relativeTempAngle<0) relativeTempAngle = 0;
		TempNumChange();
		updated = true;
	}
	return updated;
}