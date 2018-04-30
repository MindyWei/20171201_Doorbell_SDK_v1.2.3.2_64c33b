#include <assert.h>
#include "user_function.h"

static ITUCheckBox* mainMenuMuteCheckBox;

bool MainMenuOnEnter(ITUWidget* widget, char* param)
{
#if TEST_CAM
	event_call_s = 1;
	return true;
#endif
	int i;
	cur_page  = page_home;
	if(!mainMenuMuteCheckBox)
	{
		mainMenuMuteCheckBox = ituSceneFindWidget(&theScene, "mainMenuMuteCheckBox");
	}
	
	if(theConfig.mute)
		ituCheckBoxSetChecked(mainMenuMuteCheckBox,true);
	else
		ituCheckBoxSetChecked(mainMenuMuteCheckBox,false);
	
	return true;
}

bool MainMenuOnTimer(ITUWidget* widget, char* param)
{
	return false;
}

bool MainMenuOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

