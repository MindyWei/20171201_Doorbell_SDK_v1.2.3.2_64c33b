#include <assert.h>
#include "user_function.h"

bool MainMenuOnEnter(ITUWidget* widget, char* param)
{
#if TEST_CAM
	return true;
#endif
	int i;
	cur_page  = page_home;
	
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

