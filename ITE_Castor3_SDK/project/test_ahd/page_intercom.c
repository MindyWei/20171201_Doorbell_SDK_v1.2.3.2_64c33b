#include <assert.h>
#include "user_function.h"

static char id_str[4][5]		= {"ID 1","ID 2","ID 3","ID 4"};
static int id_call[4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

static ITUText* INTERCOM_TEXT_ID_1;
static ITUText* INTERCOM_TEXT_ID_2;
static ITUText* INTERCOM_TEXT_ID_3;
static ITUText* INTERCOM_TEXT_ID_4;
static ITUCheckBox* INTERCOM_CHBOX_ID1;
static ITUCheckBox* INTERCOM_CHBOX_ID2;
static ITUCheckBox* INTERCOM_CHBOX_ID3;
static ITUText* INTERCOM_TEXT_1;
static ITUText* INTERCOM_TEXT_2;
static ITUText* INTERCOM_TEXT_3;
static ITUText* INTERCOM_TEXT_4;

void intercom_itu_init()
{
	ituWidgetSetVisible(INTERCOM_TEXT_ID_1,false);
	ituWidgetSetVisible(INTERCOM_TEXT_ID_2,false);
	ituWidgetSetVisible(INTERCOM_TEXT_ID_3,false);
	ituWidgetSetVisible(INTERCOM_TEXT_ID_4,false);
	ituWidgetSetVisible(INTERCOM_TEXT_1,true);
	ituWidgetSetVisible(INTERCOM_TEXT_2,true);
	ituWidgetSetVisible(INTERCOM_TEXT_3,true);
	ituWidgetSetVisible(INTERCOM_TEXT_4,true);
	if(theConfig.id == 0)
	{
		ituWidgetSetVisible(INTERCOM_TEXT_ID_1,true);
		ituWidgetSetVisible(INTERCOM_TEXT_1,false);
		ituWidgetSetY(INTERCOM_TEXT_2,18);
		ituWidgetSetY(INTERCOM_TEXT_3,101);
		ituWidgetSetY(INTERCOM_TEXT_4,193);
	}
	else if(theConfig.id == 1)
	{
		ituWidgetSetVisible(INTERCOM_TEXT_ID_2,true);
		ituWidgetSetVisible(INTERCOM_TEXT_2,false);
		ituWidgetSetY(INTERCOM_TEXT_1,18);
		ituWidgetSetY(INTERCOM_TEXT_3,101);
		ituWidgetSetY(INTERCOM_TEXT_4,193);
	}
	else if(theConfig.id == 2)
	{
		ituWidgetSetVisible(INTERCOM_TEXT_ID_3,true);
		ituWidgetSetVisible(INTERCOM_TEXT_3,false);
		ituWidgetSetY(INTERCOM_TEXT_1,18);
		ituWidgetSetY(INTERCOM_TEXT_2,101);
		ituWidgetSetY(INTERCOM_TEXT_4,193);
	}
	else if(theConfig.id == 3)
	{
		ituWidgetSetVisible(INTERCOM_TEXT_ID_4,true);
		ituWidgetSetVisible(INTERCOM_TEXT_4,false);
		ituWidgetSetY(INTERCOM_TEXT_1,18);
		ituWidgetSetY(INTERCOM_TEXT_2,101);
		ituWidgetSetY(INTERCOM_TEXT_3,193);
	}
}
bool intercom_init(ITUWidget* widget, char* param)
{
	int i;
	if (!INTERCOM_TEXT_ID_1)
	{
		INTERCOM_TEXT_ID_1 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_ID_1");
		assert(INTERCOM_TEXT_ID_1);
		INTERCOM_TEXT_ID_2 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_ID_2");
		assert(INTERCOM_TEXT_ID_2);
		INTERCOM_TEXT_ID_3 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_ID_3");
		assert(INTERCOM_TEXT_ID_3);
		INTERCOM_TEXT_ID_4 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_ID_4");
		assert(INTERCOM_TEXT_ID_4);
		INTERCOM_CHBOX_ID1 = ituSceneFindWidget(&theScene, "INTERCOM_CHBOX_ID1");
		assert(INTERCOM_CHBOX_ID1);
		INTERCOM_CHBOX_ID2 = ituSceneFindWidget(&theScene, "INTERCOM_CHBOX_ID2");
		assert(INTERCOM_CHBOX_ID2);
		INTERCOM_CHBOX_ID3 = ituSceneFindWidget(&theScene, "INTERCOM_CHBOX_ID3");
		assert(INTERCOM_CHBOX_ID3);
		INTERCOM_TEXT_1 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_1");
		assert(INTERCOM_TEXT_1);
		INTERCOM_TEXT_2 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_2");
		assert(INTERCOM_TEXT_2);
		INTERCOM_TEXT_3 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_3");
		assert(INTERCOM_TEXT_3);
		INTERCOM_TEXT_4 = ituSceneFindWidget(&theScene, "INTERCOM_TEXT_4");
		assert(INTERCOM_TEXT_4);
	}
	intercom_itu_init();
    return true;
}

bool intercom_timer(ITUWidget* widget, char* param)
{
    return false;
}

bool intercom_leave(ITUWidget* widget, char* param)
{
    return true;
}

bool inter_call(ITUWidget* widget, char* param)
{
	interphone_number = 0;
	if(INTERCOM_CHBOX_ID1->checked)
		interphone_number |= 0x1 << id_call[theConfig.id][0];
	if(INTERCOM_CHBOX_ID2->checked)
		interphone_number |= 0x1 << id_call[theConfig.id][1];
	if(INTERCOM_CHBOX_ID3->checked)
		interphone_number |= 0x1 << id_call[theConfig.id][2];
	
	if(interphone_number)
		uart_set_mode(UART_INTER_CALL);
	return true;
}

