#include <assert.h>
#include "user_function.h"

static char switch_str[2][5]		= {"OFF","ON"};
static char type_str[2][10] 		= {"photo","video"};
static char channel_str[4][10] 	= {"Door1","Door2","CCTV1","CCTV2"};
static char leavel_str[3][10] 		= {"high","middle","low"};
static char switch_2_str[2][10] 	= {"no use","use"};
static char vol_str[2][20] 		= {"volume","receiving volume"};
static char sound_str[6][10] 		= {"sound1","sound2","sound3","sound4","sound5","sound6"};
static char timg_str[3][5] 		= {"1min","2min","3min"};
static char language_str[10][10] 	= {"English"};

static char id_str[4][10] 		= {"No. 1","No. 2","No. 3","No. 4"};
static char open_str[2][10] 		= {"Standard","Fast"};
static char ver_str[10] 			= "Ver 6.8";

static char ring_str[6][25] 		= {"A:/sounds/sound1.mp3",
                                       "A:/sounds/sound2.mp3",
                                       "A:/sounds/sound3.mp3",
                                       "A:/sounds/sound4.mp3",
                                       "A:/sounds/sound5.mp3",
                                       "A:/sounds/sound6.mp3"
                                };

char Feb_28_str[28][3] 		= {"1","2","3","4","5","6","7","8","9","10",
                               "11","12","13","14","15","16","17","18","19","20",
                               "21","22","23","24","25","26","27","28"
                           };
char Feb_29_str[29][3] 		= {"1","2","3","4","5","6","7","8","9","10",
                               "11","12","13","14","15","16","17","18","19","20",
                               "21","22","23","24","25","26","27","28","29"
                           };
char month_30_str[30][3] 		= {"1","2","3","4","5","6","7","8","9","10",
                                   "11","12","13","14","15","16","17","18","19","20",
                                   "21","22","23","24","25","26","27","28","29","30"
                             };
char month_31_str[31][3] 		= {"1","2","3","4","5","6","7","8","9","10",
                                   "11","12","13","14","15","16","17","18","19","20",
                                   "21","22","23","24","25","26","27","28","29","30","31"
                             };

char md_btn_str[24][15]		= {"MD_5_BTN_1","MD_5_BTN_2","MD_5_BTN_3","MD_5_BTN_4","MD_5_BTN_5","MD_5_BTN_6",
                               "MD_5_BTN_7","MD_5_BTN_8","MD_5_BTN_9","MD_5_BTN_10","MD_5_BTN_11","MD_5_BTN_12",
                               "MD_5_BTN_13","MD_5_BTN_14","MD_5_BTN_15","MD_5_BTN_16","MD_5_BTN_17","MD_5_BTN_18",
                               "MD_5_BTN_19","MD_5_BTN_20","MD_5_BTN_21","MD_5_BTN_22","MD_5_BTN_23","MD_5_BTN_24"
                           };

char lang_text_str[12][15]		= {"LANG_TEXT_1","LANG_TEXT_2","LANG_TEXT_3","LANG_TEXT_4","LANG_TEXT_5","LANG_TEXT_6",
                                   "LANG_TEXT_7","LANG_TEXT_8","LANG_TEXT_9","LANG_TEXT_10","LANG_TEXT_11","LANG_TEXT_12"
                              };

#define LANGUAGE_MAX			8


static ITUBackground* SET_MAIN_BG_1;
static ITUBackground* SET_MAIN_BG_1_1;
static ITUBackground* SET_MAIN_BG_2;

static ITUText* SET_MAIN_1_1_1_TEXT1;
static ITUText* SET_MAIN_1_1_1_TEXT2;
static ITUText* SET_MAIN_1_1_1_TEXT3;
static ITUText* SET_MAIN_1_2_1_TEXT1;
static ITUText* SET_MAIN_1_2_1_TEXT2;
static ITUText* SET_MAIN_1_1_1_TEXT_1;
static ITUText* SET_MAIN_1_1_1_TEXT_2;
static ITUText* SET_MAIN_1_1_1_TEXT_3;

//static ITUText* SET_MAIN_2_1_1_TEXT;
static ITUText* SET_MAIN_2_2_1_TEXT1;
static ITUText* SET_MAIN_2_2_1_TEXT2;
static ITUText* SET_MAIN_2_3_1_TEXT1;
static ITUText* SET_MAIN_2_3_1_TEXT2;
static ITUText* SET_MAIN_2_3_1_TEXT3;
static ITUText* SET_MAIN_2_3_1_TEXT4;
static ITUText* SET_MAIN_2_3_1_TEXT5;
static ITUText* SET_MAIN_2_3_1_TEXT6;
static ITUText* SET_MAIN_2_4_1_TEXT1;
static ITUText* SET_MAIN_2_4_1_TEXT2;
static ITUText* SET_MAIN_2_4_1_TEXT3;
static ITUText* SET_MAIN_2_4_1_TEXT4;
static ITUText* SET_MAIN_2_4_1_TEXT5;
static ITUText* SET_MAIN_2_4_1_TEXT6;
static ITUText* SET_MAIN_2_5_1_TEXT1;
static ITUText* SET_MAIN_2_5_1_TEXT2;
static ITUText* SET_MAIN_2_5_1_TEXT3;
static ITUText* SET_MAIN_2_5_1_TEXT4;
static ITUText* SET_MAIN_2_5_1_TEXT5;
static ITUText* SET_MAIN_2_5_1_TEXT6;

static ITUText* SET_MAIN_5_2_1_TEXT0;
static ITUText* SET_MAIN_5_2_1_TEXT1;
static ITUText* SET_MAIN_5_2_1_TEXT2;
static ITUText* SET_MAIN_5_2_1_TEXT3;

static ITUText* SET_MAIN_6_1_1_TEXT;
static ITUText* SET_MAIN_6_2_1_TEXT_1;
static ITUText* SET_MAIN_6_2_1_TEXT_2;
static ITUText* SET_MAIN_6_2_1_TEXT_3;
static ITUText* SET_MAIN_6_2_1_TEXT_4;
static ITUText* SET_MAIN_6_3_1_TEXT1;
static ITUText* SET_MAIN_6_3_1_TEXT2;
static ITUText* SET_MAIN_6_5_1_TEXT;

static ITUCheckBox* AUTO_CHBOX_1;
static ITUCheckBox* AUTO_CHBOX_2;
static ITUCheckBox* AUTO_CHBOX_3;

static ITUCheckBox* MD_CHBOX_SW;
static ITUText* MD_MAIN_TEXT_2_1_1;
static ITUText* MD_MAIN_TEXT_2_1_2;
static ITUText* MD_MAIN_TEXT_2_1_3;
static ITUText* MD_MAIN_TEXT_2_1_4;
static ITUText* MD_MAIN_TEXT_3_1_1;
static ITUText* MD_MAIN_TEXT_3_1_2;
static ITUText* MD_MAIN_TEXT_4_1_1;
static ITUText* MD_MAIN_TEXT_4_1_2;
static ITUText* MD_MAIN_TEXT_4_1_3;
static ITUText* MD_MAIN_TEXT_5_1_1;
static ITUText* MD_MAIN_TEXT_5_1_2;

static ITUCheckBox* MD_2_CHBOX_1;
static ITUCheckBox* MD_2_CHBOX_2;
static ITUCheckBox* MD_2_CHBOX_3;
static ITUCheckBox* MD_2_CHBOX_4;
static ITUCheckBox* MD_3_CHBOX_1;
static ITUCheckBox* MD_3_CHBOX_2;
static ITUCheckBox* MD_4_CHBOX_1;
static ITUCheckBox* MD_4_CHBOX_2;
static ITUCheckBox* MD_4_CHBOX_3;
static ITUCheckBox* MD_5_CHBOX_SW;
static ITUWheel* MD_5_WHEEL_1;
static ITUWheel* MD_5_WHEEL_2;
static ITUWheel* MD_5_WHEEL_3;
static ITUWheel* MD_5_WHEEL_4;
static ITUWheel* MD_5_WHEEL_5;
static ITUWheel* MD_5_WHEEL_6;
static ITUWheel* MD_5_WHEEL_7;
static ITUWheel* MD_5_WHEEL_8;
static ITUWheel* MD_5_WHEEL_9;
static ITUWheel* MD_5_WHEEL_10;
static ITUWheel* MD_5_WHEEL_11;
static ITUWheel* MD_5_WHEEL_12;
static ITUBackground* MD_MAIN_BG_CLOSE;
static ITUButton* MD_MAIN_BTN_2_;
static ITUButton* MD_MAIN_BTN_3_;
static ITUButton* MD_MAIN_BTN_4_;
static ITUButton* MD_MAIN_BTN_5_;
static ITUBackground* MD_5_BG_CLOSE;
static ITUButton* MD_5_BTN[24];

static ITUTrackBar* VOL_RING_TR_BAR;
static ITUProgressBar* VOL_RING_PR_BAR;
static ITUText* VOL_TEXT_1;
static ITUTrackBar* VOL_TALK_TR_BAR;
static ITUProgressBar* VOL_TALK_PR_BAR;
static ITUText* VOL_TEXT_2;
static ITUTrackBar* VOL_RING_TR_BAR_;
static ITUProgressBar* VOL_RING_PR_BAR_;
static ITUText* VOL_TEXT_3;
static ITUTrackBar* VOL_TALK_TR_BAR_;
static ITUProgressBar* VOL_TALK_PR_BAR_;
static ITUText* VOL_TEXT_4;

static ITUIcon* VOL_RING_ICON_1;
static ITUIcon* VOL_RING_ICON_2;
static ITUIcon* VOL_RING_ICON_3;
static ITUIcon* VOL_RING_ICON_4;
static ITUIcon* VOL_TALK_ICON_1;
static ITUIcon* VOL_TALK_ICON_2;
static ITUIcon* VOL_TALK_ICON_3;
static ITUIcon* VOL_TALK_ICON_4;

static ITUIcon* VOL_RING_ICON_1_;
static ITUIcon* VOL_RING_ICON_2_;
static ITUIcon* VOL_RING_ICON_3_;
static ITUIcon* VOL_RING_ICON_4_;
static ITUIcon* VOL_TALK_ICON_1_;
static ITUIcon* VOL_TALK_ICON_2_;
static ITUIcon* VOL_TALK_ICON_3_;
static ITUIcon* VOL_TALK_ICON_4_;


static ITUCheckBox* TO_CHBOX_SW;
static ITUText* TO_MAIN_TEXT_2_1_1;
static ITUText* TO_MAIN_TEXT_2_1_2;
static ITUText* TO_MAIN_TEXT_2_1_3;
//static ITUText* TO_MAIN_TEXT_31;
static ITUCheckBox* TO_2_CHBOX_1;
static ITUCheckBox* TO_2_CHBOX_2;
static ITUCheckBox* TO_2_CHBOX_3;
static ITUCheckBox* TO_3_CHBOX_1;
static ITUCheckBox* TO_3_CHBOX_2;
static ITUBackground* TO_MAIN_BG_CLOSE;
static ITUButton* TO_MAIN_BTN_2_;
/*
static ITUCheckBox* LANG_CHBOX_1;
static ITUCheckBox* LANG_CHBOX_2;
static ITUCheckBox* LANG_CHBOX_3;
static ITUCheckBox* LANG_CHBOX_4;
static ITUCheckBox* LANG_CHBOX_5;
static ITUCheckBox* LANG_CHBOX_6;
*/
static ITUScrollListBox* LANG_SLISTBOX_0;
static ITUScrollListBox* LANG_SLISTBOX_1;
static ITUText* LANG_TEXT[12];

static ITUCheckBox* ID_CHBOX_1;
static ITUCheckBox* ID_CHBOX_2;
static ITUCheckBox* ID_CHBOX_3;
static ITUCheckBox* ID_CHBOX_4;

static ITUCheckBox* OPEN_CHBOX_1;
static ITUCheckBox* OPEN_CHBOX_2;

static ITUText* WASH_TEXT_3;
static ITUBackground* SET_MAIN_5_WASH;
static ITUBackground* SET_BG;
static ITUIcon* DIS_ICON_WIN;

static ITUTrackBar* DIS_TR_BAR_BRI;
static ITUTrackBar* DIS_TR_BAR_CON;
static ITUTrackBar* DIS_TR_BAR_HUE;
static ITUProgressBar* DIS_PR_BAR_BRI;
static ITUProgressBar* DIS_PR_BAR_CON;
static ITUProgressBar* DIS_PR_BAR_HUE;

static ITUText* FORMAT_TEXT_TITEL;
static ITUButton* FORMAT_BTN_YES;
static ITUButton* FORMAT_BTN_NO;
static ITUText* FORMAT_TEXT_TITEL_ING;
static ITUSprite* FORMAT_SPRITE;
static ITUText* FORMAT_TEXT_NO;
static ITUText* FORMAT_TEXT_YES;

static ITUCheckBox* DEL_CHBOX_1;
static ITUCheckBox* DEL_CHBOX_2;
static ITUText* DEL_TEXT_1;
static ITUText* DEL_TEXT_2;
static ITUButton* DEL_BTN_YES;
static ITUButton* DEL_BTN_NO;
static ITUText* DEL_TEXT_TITEL_ING;
static ITUSprite* DEL_SPRITE;
static ITUText* DEL_TEXT_NO;
static ITUText* DEL_TEXT_YES;


static ITUBackground* SET_MAIN_1_AUTO;

static ITUBackground* SET_MAIN_1_MD;
static ITUBackground* MD_2;
static ITUBackground* MD_3;
static ITUBackground* MD_4;
static ITUBackground* MD_5;
static ITUBackground* MD_BG_HEAD;
static ITUBackground* MD_BG_MAIN;

static ITUBackground* SET_MAIN_2_VOL;

static ITUBackground* SET_MAIN_3_FORMAT;

static ITUBackground* SET_MAIN_3_DEL;

static ITUBackground* SET_MAIN_5_DIS;

static ITUBackground* SET_MAIN_5_TO;
static ITUBackground* TO_2;
static ITUBackground* TO_3;
static ITUBackground* TO_BG_HEAD;
static ITUBackground* TO_BG_MAIN;

static ITUBackground* SET_MAIN_6_LANG;
static ITUBackground* SET_MAIN_6_ID;
static ITUBackground* SET_MAIN_6_RSET;
static ITUBackground* SET_MAIN_6_OPEN;

static ITUBackground* SET_MAIN_4_TIME;
static ITUWheel* TIME_WHEEL_1;
static ITUWheel* TIME_WHEEL_2;
static ITUWheel* TIME_WHEEL_3;
static ITUWheel* TIME_WHEEL_4;
static ITUWheel* TIME_WHEEL_5;
static ITUWheel* TIME_WHEEL_6;

static bool key_sound = false;
static uint32_t wash_tick;
static uint8_t wash_time = 0;

static bool md_wheel_change = false;

static bool user_listbox_load(ITUWidget* widget, char* param,int listbox_num)
{
	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;

	entryCount = LANGUAGE_MAX;
	count = ituScrollListBoxGetItemCount(slistbox);
	node = ituScrollListBoxGetLastPageItem(slistbox);
	listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

	if (listbox->pageIndex == 0)
	{
		// initialize
		listbox->pageIndex = 1;
		listbox->focusIndex = -1;
	}

	if (listbox->pageIndex <= 1)
	{
		for (i = 0; i < count; i++)
		{
			ITUScrollText* scrolltext = (ITUScrollText*) node;
			ituScrollTextSetString(scrolltext, "");

			node = node->sibling;
		}
	}

	i = 0;
	j = count * (listbox->pageIndex - 2);
	if (j < 0)
		j = 0;

	for (; j < entryCount; j++)
	{
		ITUScrollText* scrolltext = (ITUScrollText*) node;

		if(listbox_num == 0)
		{
			if(theConfig.language == j)
				ituTextSetString(scrolltext, "1");
			else
				ituTextSetString(scrolltext, "0");
		}
		else if(listbox_num == 1)

		{
			ituTextSetString(scrolltext, ituTextGetString(LANG_TEXT[j]));
		}

		ituWidgetSetCustomData(scrolltext, j);
		i++;

		node = node->sibling;

		if (node == NULL)
			break;
	}

	for (; node; node = node->sibling)
	{
		ITUScrollText* scrolltext = (ITUScrollText*) node;
		ituScrollTextSetString(scrolltext, "");
	}

	if (listbox->pageIndex == listbox->pageCount)
	{
		if (i == 0)
			listbox->itemCount = i;
		else
		{
			listbox->itemCount = i % count;
			if (listbox->itemCount == 0)
				listbox->itemCount = count;
		}
	}
	else
		listbox->itemCount = count;
	//printf("user_listbox_load...........................%d\n",listbox_num);
	return true;
}
bool lang_slistbox_0_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,0);
}


bool lang_slistbox_1_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,1);
}

void set_all_init()
{
	if(ituWidgetIsVisible(SET_MAIN_1_AUTO))
		ituWidgetSetVisible(SET_MAIN_1_AUTO, false);

	if(ituWidgetIsVisible(SET_MAIN_1_MD))
	{
		ituWidgetSetVisible(SET_MAIN_1_MD, false);
		ituWidgetEnable(SET_BG);
		ituWidgetSetVisible(MD_2, false);
		ituWidgetSetVisible(MD_3, false);
		ituWidgetSetVisible(MD_4, false);
		ituWidgetSetVisible(MD_5, false);
		ituWidgetEnable(MD_BG_HEAD);
		ituWidgetEnable(MD_BG_MAIN);
	}

	if(ituWidgetIsVisible(SET_MAIN_2_VOL))
		ituWidgetSetVisible(SET_MAIN_2_VOL, false);

	if(ituWidgetIsVisible(SET_MAIN_3_FORMAT))
		ituWidgetSetVisible(SET_MAIN_3_FORMAT, false);

	if(ituWidgetIsVisible(SET_MAIN_3_DEL))
		ituWidgetSetVisible(SET_MAIN_3_DEL, false);

	if(ituWidgetIsVisible(SET_MAIN_5_DIS))
		ituWidgetSetVisible(SET_MAIN_5_DIS, false);

	if(ituWidgetIsVisible(SET_MAIN_5_TO))
	{
		ituWidgetSetVisible(SET_MAIN_5_TO, false);
		ituWidgetSetVisible(TO_2, false);
		ituWidgetSetVisible(TO_3, false);
		ituWidgetEnable(TO_BG_HEAD);
		ituWidgetEnable(TO_BG_MAIN);
	}

	if(ituWidgetIsVisible(SET_MAIN_4_TIME))
		ituWidgetSetVisible(SET_MAIN_4_TIME, false);

	if(ituWidgetIsVisible(SET_MAIN_5_WASH))
		ituWidgetSetVisible(SET_MAIN_5_WASH, false);

	if(ituWidgetIsVisible(SET_MAIN_6_LANG))
		ituWidgetSetVisible(SET_MAIN_6_LANG, false);

	if(ituWidgetIsVisible(SET_MAIN_6_ID))
		ituWidgetSetVisible(SET_MAIN_6_ID, false);

	if(ituWidgetIsVisible(SET_MAIN_6_RSET))
		ituWidgetSetVisible(SET_MAIN_6_RSET, false);

	if(ituWidgetIsVisible(SET_MAIN_6_OPEN))
		ituWidgetSetVisible(SET_MAIN_6_OPEN, false);

	ituWidgetEnable(SET_BG);

}
void set_main_init()
{
	char vol_str_s[30] = {'\0'};
	char val_str_s[5] = {'\0'};

	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT2, false);
	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT3, false);
	if(theConfig.zidong == 0)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT1, true);
	else if(theConfig.zidong == 1)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT2, true);
	else if(theConfig.zidong == 2)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT3, true);
	
	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_1, false);
	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_2, false);
	ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_3, false);
	if(theConfig.zidong == 0)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_1, true);
	else if(theConfig.zidong == 1)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_2, true);
	else if(theConfig.zidong == 2)
		ituWidgetSetVisible(SET_MAIN_1_1_1_TEXT_3, true);
	
	ituWidgetSetVisible(SET_MAIN_1_2_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_1_2_1_TEXT2, false);
	printf("theConfig.md:%d\r\n", theConfig.md);
	if(theConfig.md == 0)
		ituWidgetSetVisible(SET_MAIN_1_2_1_TEXT2, true);
	else if(theConfig.md == 1)
		ituWidgetSetVisible(SET_MAIN_1_2_1_TEXT1, true);
	
	ituWidgetSetVisible(SET_MAIN_2_2_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_2_2_1_TEXT2, false);
	if(theConfig.keyvol == 0)
		ituWidgetSetVisible(SET_MAIN_2_2_1_TEXT2, true);
	else if(theConfig.keyvol == 1)
		ituWidgetSetVisible(SET_MAIN_2_2_1_TEXT1, true);
	
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT2, false);
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT3, false);
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT4, false);
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT5, false);
	ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT6, false);
	if(theConfig.interphone == 0)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT1, true);
	else if(theConfig.interphone == 1)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT2, true);
	else if(theConfig.interphone == 2)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT3, true);
	else if(theConfig.interphone == 3)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT4, true);
	else if(theConfig.interphone == 4)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT5, true);
	else if(theConfig.interphone == 5)
		ituWidgetSetVisible(SET_MAIN_2_3_1_TEXT6, true);
	
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT2, false);
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT3, false);
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT4, false);
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT5, false);
	ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT6, false);
	if(theConfig.door1 == 0)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT1, true);
	else if(theConfig.door1 == 1)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT2, true);
	else if(theConfig.door1 == 2)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT3, true);
	else if(theConfig.door1 == 3)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT4, true);
	else if(theConfig.door1 == 4)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT5, true);
	else if(theConfig.door1 == 5)
		ituWidgetSetVisible(SET_MAIN_2_4_1_TEXT6, true);
	
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT2, false);
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT3, false);
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT4, false);
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT5, false);
	ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT6, false);
	if(theConfig.door2 == 0)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT1, true);
	else if(theConfig.door2 == 1)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT2, true);
	else if(theConfig.door2 == 2)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT3, true);
	else if(theConfig.door2 == 3)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT4, true);
	else if(theConfig.door2 == 4)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT5, true);
	else if(theConfig.door2 == 5)
		ituWidgetSetVisible(SET_MAIN_2_5_1_TEXT6, true);

	ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT0, false);
	ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT2, false);
	ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT3, false);
	if(theConfig.lcdout == 0)
		ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT0, true);
	else if(theConfig.lcdouttime == 0)
		ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT1, true);
	else if(theConfig.lcdouttime == 1)
		ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT2, true);
	else if(theConfig.lcdouttime == 2)
		ituWidgetSetVisible(SET_MAIN_5_2_1_TEXT3, true);

	ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_1, false);
	ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_2, false);
	ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_3, false);
	ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_4, false);
	if(theConfig.id == 0)
		ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_1, true);
	else if(theConfig.id == 1)
		ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_2, true);
	else if(theConfig.id == 2)
		ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_3, true);
	else if(theConfig.id == 3)
		ituWidgetSetVisible(SET_MAIN_6_2_1_TEXT_4, true);
	//ituTextSetString(SET_MAIN_6_2_1_TEXT, id_str[theConfig.id]);

	ituWidgetSetVisible(SET_MAIN_6_3_1_TEXT1, false);
	ituWidgetSetVisible(SET_MAIN_6_3_1_TEXT2, false);
	if(theConfig.open == 0)
		ituWidgetSetVisible(SET_MAIN_6_3_1_TEXT1, true);
	else if(theConfig.open == 1)
		ituWidgetSetVisible(SET_MAIN_6_3_1_TEXT2, true);
	//ituTextSetString(SET_MAIN_6_3_1_TEXT, open_str[theConfig.open]);

	ituTextSetString(SET_MAIN_6_5_1_TEXT, ver_str);
}

void set_auto_init()
{
	ituCheckBoxSetChecked(AUTO_CHBOX_1,false);
	ituCheckBoxSetChecked(AUTO_CHBOX_2,false);
	ituCheckBoxSetChecked(AUTO_CHBOX_3,false);
	if(theConfig.zidong == 0)
		ituCheckBoxSetChecked(AUTO_CHBOX_1,true);
	else if(theConfig.zidong == 1)
		ituCheckBoxSetChecked(AUTO_CHBOX_2,true);
	else if(theConfig.zidong == 2)
		ituCheckBoxSetChecked(AUTO_CHBOX_3,true);
}

void set_md_init()
{
	ituCheckBoxSetChecked(MD_CHBOX_SW,(bool)theConfig.md);
#if 1
	ituWidgetSetVisible(MD_MAIN_TEXT_2_1_1, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_2_1_2, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_2_1_3, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_2_1_4, false);
	if(theConfig.mdcam == 0)
		ituWidgetSetVisible(MD_MAIN_TEXT_2_1_1, true);
	else if(theConfig.mdcam == 1)
		ituWidgetSetVisible(MD_MAIN_TEXT_2_1_2, true);
	else if(theConfig.mdcam == 2)
		ituWidgetSetVisible(MD_MAIN_TEXT_2_1_3, true);
	else if(theConfig.mdcam == 3)
		ituWidgetSetVisible(MD_MAIN_TEXT_2_1_4, true);

	ituWidgetSetVisible(MD_MAIN_TEXT_3_1_1, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_3_1_2, false);
	if(theConfig.mdsave == 0)
		ituWidgetSetVisible(MD_MAIN_TEXT_3_1_1, true);
	else if(theConfig.mdsave == 1)
		ituWidgetSetVisible(MD_MAIN_TEXT_3_1_2, true);

	ituWidgetSetVisible(MD_MAIN_TEXT_4_1_1, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_4_1_2, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_4_1_3, false);
	if(theConfig.mdsensitive == 0)
		ituWidgetSetVisible(MD_MAIN_TEXT_4_1_1, true);
	else if(theConfig.mdsensitive == 1)
		ituWidgetSetVisible(MD_MAIN_TEXT_4_1_2, true);
	else if(theConfig.mdsensitive == 2)
		ituWidgetSetVisible(MD_MAIN_TEXT_4_1_3, true);

	ituWidgetSetVisible(MD_MAIN_TEXT_5_1_1, false);
	ituWidgetSetVisible(MD_MAIN_TEXT_5_1_2, false);
	if(theConfig.mdtime == 0)
		ituWidgetSetVisible(MD_MAIN_TEXT_5_1_1, true);
	else if(theConfig.mdtime == 1)
		ituWidgetSetVisible(MD_MAIN_TEXT_5_1_2, true);
	
#endif
	ituCheckBoxSetChecked(MD_2_CHBOX_1,false);
	ituCheckBoxSetChecked(MD_2_CHBOX_2,false);
	ituCheckBoxSetChecked(MD_2_CHBOX_3,false);
	ituCheckBoxSetChecked(MD_2_CHBOX_4,false);
	ituCheckBoxSetChecked(MD_3_CHBOX_1,false);
	ituCheckBoxSetChecked(MD_3_CHBOX_2,false);
	ituCheckBoxSetChecked(MD_4_CHBOX_1,false);
	ituCheckBoxSetChecked(MD_4_CHBOX_2,false);
	ituCheckBoxSetChecked(MD_4_CHBOX_3,false);
#if 1
	if(theConfig.mdcam == 0)
		ituCheckBoxSetChecked(MD_2_CHBOX_1,true);
	else if(theConfig.mdcam == 1)
		ituCheckBoxSetChecked(MD_2_CHBOX_2,true);
	else if(theConfig.mdcam == 2)
		ituCheckBoxSetChecked(MD_2_CHBOX_3,true);
	else if(theConfig.mdcam == 3)
		ituCheckBoxSetChecked(MD_2_CHBOX_4,true);

	if(theConfig.mdsave == 0)
		ituCheckBoxSetChecked(MD_3_CHBOX_1,true);
	else if(theConfig.mdsave == 1)
		ituCheckBoxSetChecked(MD_3_CHBOX_2,true);

	if(theConfig.mdsensitive == 0)
		ituCheckBoxSetChecked(MD_4_CHBOX_1,true);
	else if(theConfig.mdsensitive == 1)
		ituCheckBoxSetChecked(MD_4_CHBOX_2,true);
	else if(theConfig.mdsensitive == 2)
		ituCheckBoxSetChecked(MD_4_CHBOX_3,true);
#endif
	ituCheckBoxSetChecked(MD_5_CHBOX_SW,(bool)theConfig.mdtime);

	int i;
	if(theConfig.md)
	{
		ituWidgetSetVisible(MD_MAIN_BG_CLOSE, false);
		ituWidgetEnable(MD_MAIN_BTN_2_);
		ituWidgetEnable(MD_MAIN_BTN_3_);
		ituWidgetEnable(MD_MAIN_BTN_4_);
		ituWidgetEnable(MD_MAIN_BTN_5_);
	}
	else
	{
		ituWidgetSetVisible(MD_MAIN_BG_CLOSE, true);
		ituWidgetDisable(MD_MAIN_BTN_2_);
		ituWidgetDisable(MD_MAIN_BTN_3_);
		ituWidgetDisable(MD_MAIN_BTN_4_);
		ituWidgetDisable(MD_MAIN_BTN_5_);
	}

	if(theConfig.mdtime)
	{
		ituWidgetSetVisible(MD_5_BG_CLOSE, false);
		for(i = 0; i < 24; i++)
			ituWidgetEnable(MD_5_BTN[i]);
		ituWidgetEnable(MD_5_WHEEL_1);
		ituWidgetEnable(MD_5_WHEEL_2);
		ituWidgetEnable(MD_5_WHEEL_3);
		ituWidgetEnable(MD_5_WHEEL_4);
		ituWidgetEnable(MD_5_WHEEL_5);
		ituWidgetEnable(MD_5_WHEEL_6);
		ituWidgetEnable(MD_5_WHEEL_7);
		ituWidgetEnable(MD_5_WHEEL_8);
		ituWidgetEnable(MD_5_WHEEL_9);
		ituWidgetEnable(MD_5_WHEEL_10);
		ituWidgetEnable(MD_5_WHEEL_11);
		ituWidgetEnable(MD_5_WHEEL_12);
	}
	else
	{
		ituWidgetSetVisible(MD_5_BG_CLOSE, true);
		for(i = 0; i < 24; i++)
			ituWidgetDisable(MD_5_BTN[i]);
		ituWidgetDisable(MD_5_WHEEL_1);
		ituWidgetDisable(MD_5_WHEEL_2);
		ituWidgetDisable(MD_5_WHEEL_3);
		ituWidgetDisable(MD_5_WHEEL_4);
		ituWidgetDisable(MD_5_WHEEL_5);
		ituWidgetDisable(MD_5_WHEEL_6);
		ituWidgetDisable(MD_5_WHEEL_7);
		ituWidgetDisable(MD_5_WHEEL_8);
		ituWidgetDisable(MD_5_WHEEL_9);
		ituWidgetDisable(MD_5_WHEEL_10);
		ituWidgetDisable(MD_5_WHEEL_11);
		ituWidgetDisable(MD_5_WHEEL_12);
	}
}
void _volume_bar()
{
	if(theConfig.ringvol == 0)
	{
		ituWidgetSetVisible(VOL_RING_ICON_1, true);
		ituWidgetSetVisible(VOL_RING_ICON_2, true);
		ituWidgetSetVisible(VOL_RING_ICON_3, false);
		ituWidgetSetVisible(VOL_RING_ICON_4, false);
	}
	else if(theConfig.ringvol == 3)
	{
		ituWidgetSetVisible(VOL_RING_ICON_1, false);
		ituWidgetSetVisible(VOL_RING_ICON_2, false);
		ituWidgetSetVisible(VOL_RING_ICON_3, true);
		ituWidgetSetVisible(VOL_RING_ICON_4, true);
	}
	else
	{
		ituWidgetSetVisible(VOL_RING_ICON_1, false);
		ituWidgetSetVisible(VOL_RING_ICON_2, true);
		ituWidgetSetVisible(VOL_RING_ICON_3, true);
		ituWidgetSetVisible(VOL_RING_ICON_4, false);
	}

	if(theConfig.talkvol == 0)
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1, true);
		ituWidgetSetVisible(VOL_TALK_ICON_2, true);
		ituWidgetSetVisible(VOL_TALK_ICON_3, false);
		ituWidgetSetVisible(VOL_TALK_ICON_4, false);
	}
	else if(theConfig.talkvol == 3)
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1, false);
		ituWidgetSetVisible(VOL_TALK_ICON_2, false);
		ituWidgetSetVisible(VOL_TALK_ICON_3, true);
		ituWidgetSetVisible(VOL_TALK_ICON_4, true);
	}
	else
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1, false);
		ituWidgetSetVisible(VOL_TALK_ICON_2, true);
		ituWidgetSetVisible(VOL_TALK_ICON_3, true);
		ituWidgetSetVisible(VOL_TALK_ICON_4, false);
	}

	if(theConfig.Iringvol == 0)
	{
		ituWidgetSetVisible(VOL_RING_ICON_1_, true);
		ituWidgetSetVisible(VOL_RING_ICON_2_, true);
		ituWidgetSetVisible(VOL_RING_ICON_3_, false);
		ituWidgetSetVisible(VOL_RING_ICON_4_, false);
	}
	else if(theConfig.Iringvol == 3)
	{
		ituWidgetSetVisible(VOL_RING_ICON_1_, false);
		ituWidgetSetVisible(VOL_RING_ICON_2_, false);
		ituWidgetSetVisible(VOL_RING_ICON_3_, true);
		ituWidgetSetVisible(VOL_RING_ICON_4_, true);
	}
	else
	{
		ituWidgetSetVisible(VOL_RING_ICON_1_, false);
		ituWidgetSetVisible(VOL_RING_ICON_2_, true);
		ituWidgetSetVisible(VOL_RING_ICON_3_, true);
		ituWidgetSetVisible(VOL_RING_ICON_4_, false);
	}

	if(theConfig.Italkvol == 0)
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1_, true);
		ituWidgetSetVisible(VOL_TALK_ICON_2_, true);
		ituWidgetSetVisible(VOL_TALK_ICON_3_, false);
		ituWidgetSetVisible(VOL_TALK_ICON_4_, false);
	}
	else if(theConfig.Italkvol == 3)
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1_, false);
		ituWidgetSetVisible(VOL_TALK_ICON_2_, false);
		ituWidgetSetVisible(VOL_TALK_ICON_3_, true);
		ituWidgetSetVisible(VOL_TALK_ICON_4_, true);
	}
	else
	{
		ituWidgetSetVisible(VOL_TALK_ICON_1_, false);
		ituWidgetSetVisible(VOL_TALK_ICON_2_, true);
		ituWidgetSetVisible(VOL_TALK_ICON_3_, true);
		ituWidgetSetVisible(VOL_TALK_ICON_4_, false);
	}
}
void set_volume_init()
{
	char val_str[5] = {'\0'};

	ituTrackBarSetValue(VOL_RING_TR_BAR,theConfig.ringvol);
	ituProgressBarSetValue(VOL_RING_PR_BAR,theConfig.ringvol);
	sprintf(val_str,"%d",theConfig.ringvol);
	ituTextSetString(VOL_TEXT_1,val_str);

	ituTrackBarSetValue(VOL_TALK_TR_BAR,theConfig.talkvol);
	ituProgressBarSetValue(VOL_TALK_PR_BAR,theConfig.talkvol);
	sprintf(val_str,"%d",theConfig.talkvol);
	ituTextSetString(VOL_TEXT_2,val_str);

	ituTrackBarSetValue(VOL_RING_TR_BAR_,theConfig.Iringvol);
	ituProgressBarSetValue(VOL_RING_PR_BAR_,theConfig.Iringvol);
	sprintf(val_str,"%d",theConfig.Iringvol);
	ituTextSetString(VOL_TEXT_3,val_str);

	ituTrackBarSetValue(VOL_TALK_TR_BAR_,theConfig.Italkvol);
	ituProgressBarSetValue(VOL_TALK_PR_BAR_,theConfig.Italkvol);
	sprintf(val_str,"%d",theConfig.Italkvol);
	ituTextSetString(VOL_TEXT_4,val_str);

	_volume_bar();
}


void set_save_init()
{

	ituWidgetSetVisible(FORMAT_TEXT_TITEL, true);
	ituWidgetSetVisible(FORMAT_BTN_YES, true);
	ituWidgetSetVisible(FORMAT_BTN_NO, true);
	ituWidgetSetVisible(FORMAT_TEXT_NO, true);
	ituWidgetSetVisible(FORMAT_TEXT_YES, true);

	ituWidgetSetVisible(FORMAT_TEXT_TITEL_ING, false);
	ituWidgetSetVisible(FORMAT_SPRITE, false);

	ituWidgetSetVisible(DEL_CHBOX_1, true);
	ituWidgetSetVisible(DEL_TEXT_1, true);
	ituWidgetSetVisible(DEL_CHBOX_2, true);
	ituWidgetSetVisible(DEL_TEXT_2, true);
	ituWidgetSetVisible(DEL_BTN_YES, true);
	ituWidgetSetVisible(DEL_BTN_NO, true);
	ituWidgetSetVisible(DEL_TEXT_NO, true);
	ituWidgetSetVisible(DEL_TEXT_YES, true);

	ituWidgetSetVisible(DEL_TEXT_TITEL_ING, false);
	ituWidgetSetVisible(DEL_SPRITE, false);


}

void set_standby_init()
{
	ituCheckBoxSetChecked(TO_CHBOX_SW,(bool)theConfig.lcdout);

	ituWidgetSetVisible(TO_MAIN_TEXT_2_1_1, false);
	ituWidgetSetVisible(TO_MAIN_TEXT_2_1_2, false);
	ituWidgetSetVisible(TO_MAIN_TEXT_2_1_3, false);
	if(theConfig.lcdouttime == 0)
		ituWidgetSetVisible(TO_MAIN_TEXT_2_1_1, true);
	else if(theConfig.lcdouttime == 1)
		ituWidgetSetVisible(TO_MAIN_TEXT_2_1_2, true);
	else if(theConfig.lcdouttime == 2)
		ituWidgetSetVisible(TO_MAIN_TEXT_2_1_3, true);
	//ituTextSetString(TO_MAIN_TEXT_21, timg_str[theConfig.lcdouttime]);
	//ituTextSetString(TO_MAIN_TEXT_31, switch_str[theConfig.infoframe]);

	ituCheckBoxSetChecked(TO_2_CHBOX_1,false);
	ituCheckBoxSetChecked(TO_2_CHBOX_2,false);
	ituCheckBoxSetChecked(TO_2_CHBOX_3,false);
	ituCheckBoxSetChecked(TO_3_CHBOX_1,false);
	ituCheckBoxSetChecked(TO_3_CHBOX_2,false);

	if(theConfig.lcdouttime == 0)
		ituCheckBoxSetChecked(TO_2_CHBOX_1,true);
	else if(theConfig.lcdouttime == 1)
		ituCheckBoxSetChecked(TO_2_CHBOX_2,true);
	else if(theConfig.lcdouttime == 2)
		ituCheckBoxSetChecked(TO_2_CHBOX_3,true);

	if(theConfig.infoframe == 0)
		ituCheckBoxSetChecked(TO_3_CHBOX_1,true);
	else if(theConfig.infoframe == 1)
		ituCheckBoxSetChecked(TO_3_CHBOX_2,true);

	if(theConfig.lcdout)
	{
		ituWidgetSetVisible(TO_MAIN_BG_CLOSE, false);
		ituWidgetEnable(TO_MAIN_BTN_2_);
	}
	else
	{
		ituWidgetSetVisible(TO_MAIN_BG_CLOSE, true);
		ituWidgetDisable(TO_MAIN_BTN_2_);
	}

}

void set_ohter_init()
{
	/*
	ituCheckBoxSetChecked(LANG_CHBOX_1,false);
	ituCheckBoxSetChecked(LANG_CHBOX_2,false);
	ituCheckBoxSetChecked(LANG_CHBOX_3,false);
	ituCheckBoxSetChecked(LANG_CHBOX_4,false);
	ituCheckBoxSetChecked(LANG_CHBOX_5,false);
	ituCheckBoxSetChecked(LANG_CHBOX_6,false);
	*/
	/*
	if(theConfig.language == 0)
		ituCheckBoxSetChecked(LANG_CHBOX_1,true);
	else if(theConfig.language == 1)
		ituCheckBoxSetChecked(LANG_CHBOX_2,true);
	else if(theConfig.language == 2)
		ituCheckBoxSetChecked(LANG_CHBOX_3,true);
	else if(theConfig.language == 3)
		ituCheckBoxSetChecked(LANG_CHBOX_4,true);
	else if(theConfig.language == 4)
		ituCheckBoxSetChecked(LANG_CHBOX_5,true);
	else if(theConfig.language == 5)
		ituCheckBoxSetChecked(LANG_CHBOX_6,true);
	*/
	/*
	printf("LANG_SLISTBOX_1->listbox.pageIndex = %d\n",LANG_SLISTBOX_1->listbox.pageIndex);
	printf("LANG_SLISTBOX_1->listbox.pageCount = %d\n",LANG_SLISTBOX_1->listbox.pageCount);
	printf("LANG_SLISTBOX_1->listbox.focusIndex = %d\n",LANG_SLISTBOX_1->listbox.focusIndex);
	printf("LANG_SLISTBOX_1->listbox.itemCount = %d\n",LANG_SLISTBOX_1->listbox.itemCount);

	LANG_SLISTBOX_0->listbox.pageIndex = 0;
	LANG_SLISTBOX_0->listbox.focusIndex = -1;
	LANG_SLISTBOX_0->listbox.pageCount = 2;
	LANG_SLISTBOX_0->listbox.itemCount = 6;
	LANG_SLISTBOX_1->listbox.pageIndex = 0;
	LANG_SLISTBOX_1->listbox.focusIndex = -1;
	LANG_SLISTBOX_1->listbox.pageCount = 2;
	LANG_SLISTBOX_1->listbox.itemCount = 6;

	printf("LANG_SLISTBOX_1->listbox.pageIndex = %d\n",LANG_SLISTBOX_1->listbox.pageIndex);
	printf("LANG_SLISTBOX_1->listbox.pageCount = %d\n",LANG_SLISTBOX_1->listbox.pageCount);
	printf("LANG_SLISTBOX_1->listbox.focusIndex = %d\n",LANG_SLISTBOX_1->listbox.focusIndex);
	printf("LANG_SLISTBOX_1->listbox.itemCount = %d\n",LANG_SLISTBOX_1->listbox.itemCount);
	*/
	ituCheckBoxSetChecked(ID_CHBOX_1,false);
	ituCheckBoxSetChecked(ID_CHBOX_2,false);
	ituCheckBoxSetChecked(ID_CHBOX_3,false);
	ituCheckBoxSetChecked(ID_CHBOX_4,false);

	ituCheckBoxSetChecked(OPEN_CHBOX_1,false);
	ituCheckBoxSetChecked(OPEN_CHBOX_2,false);

	if(theConfig.id == 0)
		ituCheckBoxSetChecked(ID_CHBOX_1,true);
	else if(theConfig.id == 1)
		ituCheckBoxSetChecked(ID_CHBOX_2,true);
	else if(theConfig.id == 2)
		ituCheckBoxSetChecked(ID_CHBOX_3,true);
	else if(theConfig.id == 3)
		ituCheckBoxSetChecked(ID_CHBOX_4,true);

	if(theConfig.open == 0)
		ituCheckBoxSetChecked(OPEN_CHBOX_1,true);
	else if(theConfig.open  == 1)
		ituCheckBoxSetChecked(OPEN_CHBOX_2,true);

}

bool set_main_bg_1_show(ITUWidget* widget, char* param)
{
	printf("set_main_bg_1_show.........................................\n");

	ITUBackground* SET_MAIN_BG_1_s = ituSceneFindWidget(&theScene, "SET_MAIN_BG_1");
	assert(SET_MAIN_BG_1_s);
	ITUBackground* SET_MAIN_BG_1_1_s = ituSceneFindWidget(&theScene, "SET_MAIN_BG_1_1");
	assert(SET_MAIN_BG_1_1_s);

	if(theConfig.id == 0)
		ituWidgetSetVisible(SET_MAIN_BG_1_s, true);
	else
		ituWidgetSetVisible(SET_MAIN_BG_1_1_s, true);

	return true;
}

bool set_main_bg_1_hide(ITUWidget* widget, char* param)
{
	printf("set_main_bg_1_hide.........................................\n");

	ITUBackground* SET_MAIN_BG_1_s = ituSceneFindWidget(&theScene, "SET_MAIN_BG_1");
	assert(SET_MAIN_BG_1_s);
	ITUBackground* SET_MAIN_BG_1_1_s = ituSceneFindWidget(&theScene, "SET_MAIN_BG_1_1");
	assert(SET_MAIN_BG_1_1_s);

	ituWidgetSetVisible(SET_MAIN_BG_1_s, false);
	ituWidgetSetVisible(SET_MAIN_BG_1_1_s, false);

	return true;
}

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;
	destx = rect->x + x;
	desty = rect->y + y;
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}
bool set_init(ITUWidget* widget, char* param)
{
	int i;
	if(!SET_MAIN_1_1_1_TEXT1)
	{
		SET_MAIN_1_1_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT1");
		assert(SET_MAIN_1_1_1_TEXT1);
		SET_MAIN_1_1_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT2");
		assert(SET_MAIN_1_1_1_TEXT2);
		SET_MAIN_1_1_1_TEXT3 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT3");
		assert(SET_MAIN_1_1_1_TEXT3);
		SET_MAIN_1_2_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_1_2_1_TEXT1");
		assert(SET_MAIN_1_2_1_TEXT1);
		SET_MAIN_1_2_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_1_2_1_TEXT2");
		assert(SET_MAIN_1_2_1_TEXT2);
		SET_MAIN_1_1_1_TEXT_1 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT_1");
		assert(SET_MAIN_1_1_1_TEXT_1);
		SET_MAIN_1_1_1_TEXT_2 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT_2");
		assert(SET_MAIN_1_1_1_TEXT_2);
		SET_MAIN_1_1_1_TEXT_3 = ituSceneFindWidget(&theScene, "SET_MAIN_1_1_1_TEXT_3");
		assert(SET_MAIN_1_1_1_TEXT_3);

		SET_MAIN_2_2_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_2_2_1_TEXT1");
		assert(SET_MAIN_2_2_1_TEXT1);
		SET_MAIN_2_2_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_2_2_1_TEXT2");
		assert(SET_MAIN_2_2_1_TEXT2);
		SET_MAIN_2_3_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT1");
		assert(SET_MAIN_2_3_1_TEXT1);
		SET_MAIN_2_3_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT2");
		assert(SET_MAIN_2_3_1_TEXT2);
		SET_MAIN_2_3_1_TEXT3 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT3");
		assert(SET_MAIN_2_3_1_TEXT3);
		SET_MAIN_2_3_1_TEXT4 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT4");
		assert(SET_MAIN_2_3_1_TEXT4);
		SET_MAIN_2_3_1_TEXT5 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT5");
		assert(SET_MAIN_2_3_1_TEXT5);
		SET_MAIN_2_3_1_TEXT6 = ituSceneFindWidget(&theScene, "SET_MAIN_2_3_1_TEXT6");
		assert(SET_MAIN_2_3_1_TEXT6);
		SET_MAIN_2_4_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT1");
		assert(SET_MAIN_2_4_1_TEXT1);
		SET_MAIN_2_4_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT2");
		assert(SET_MAIN_2_4_1_TEXT2);
		SET_MAIN_2_4_1_TEXT3 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT3");
		assert(SET_MAIN_2_4_1_TEXT3);
		SET_MAIN_2_4_1_TEXT4 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT4");
		assert(SET_MAIN_2_4_1_TEXT4);
		SET_MAIN_2_4_1_TEXT5 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT5");
		assert(SET_MAIN_2_4_1_TEXT5);
		SET_MAIN_2_4_1_TEXT6 = ituSceneFindWidget(&theScene, "SET_MAIN_2_4_1_TEXT6");
		assert(SET_MAIN_2_4_1_TEXT6);
		SET_MAIN_2_5_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT1");
		assert(SET_MAIN_2_5_1_TEXT1);
		SET_MAIN_2_5_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT2");
		assert(SET_MAIN_2_5_1_TEXT2);
		SET_MAIN_2_5_1_TEXT3 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT3");
		assert(SET_MAIN_2_5_1_TEXT3);
		SET_MAIN_2_5_1_TEXT4 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT4");
		assert(SET_MAIN_2_5_1_TEXT4);
		SET_MAIN_2_5_1_TEXT5 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT5");
		assert(SET_MAIN_2_5_1_TEXT5);
		SET_MAIN_2_5_1_TEXT6 = ituSceneFindWidget(&theScene, "SET_MAIN_2_5_1_TEXT6");
		assert(SET_MAIN_2_5_1_TEXT6);

		SET_MAIN_5_2_1_TEXT0 = ituSceneFindWidget(&theScene, "SET_MAIN_5_2_1_TEXT0");
		assert(SET_MAIN_5_2_1_TEXT0);
		SET_MAIN_5_2_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_5_2_1_TEXT1");
		assert(SET_MAIN_5_2_1_TEXT1);
		SET_MAIN_5_2_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_5_2_1_TEXT2");
		assert(SET_MAIN_5_2_1_TEXT2);
		SET_MAIN_5_2_1_TEXT3 = ituSceneFindWidget(&theScene, "SET_MAIN_5_2_1_TEXT3");
		assert(SET_MAIN_5_2_1_TEXT3);
		
		SET_MAIN_6_1_1_TEXT = ituSceneFindWidget(&theScene, "SET_MAIN_6_1_1_TEXT");
		assert(SET_MAIN_6_1_1_TEXT);
		SET_MAIN_6_2_1_TEXT_1 = ituSceneFindWidget(&theScene, "SET_MAIN_6_2_1_TEXT_1");
		assert(SET_MAIN_6_2_1_TEXT_1);
		SET_MAIN_6_2_1_TEXT_2 = ituSceneFindWidget(&theScene, "SET_MAIN_6_2_1_TEXT_2");
		assert(SET_MAIN_6_2_1_TEXT_2);
		SET_MAIN_6_2_1_TEXT_3 = ituSceneFindWidget(&theScene, "SET_MAIN_6_2_1_TEXT_3");
		assert(SET_MAIN_6_2_1_TEXT_3);
		SET_MAIN_6_2_1_TEXT_4 = ituSceneFindWidget(&theScene, "SET_MAIN_6_2_1_TEXT_4");
		assert(SET_MAIN_6_2_1_TEXT_4);
		SET_MAIN_6_3_1_TEXT1 = ituSceneFindWidget(&theScene, "SET_MAIN_6_3_1_TEXT1");
		assert(SET_MAIN_6_3_1_TEXT1);
		SET_MAIN_6_3_1_TEXT2 = ituSceneFindWidget(&theScene, "SET_MAIN_6_3_1_TEXT2");
		assert(SET_MAIN_6_3_1_TEXT2);
		SET_MAIN_6_5_1_TEXT = ituSceneFindWidget(&theScene, "SET_MAIN_6_5_1_TEXT");
		assert(SET_MAIN_6_5_1_TEXT);

		AUTO_CHBOX_1 = ituSceneFindWidget(&theScene, "AUTO_CHBOX_1");
		assert(AUTO_CHBOX_1);
		AUTO_CHBOX_2 = ituSceneFindWidget(&theScene, "AUTO_CHBOX_2");
		assert(AUTO_CHBOX_2);
		AUTO_CHBOX_3 = ituSceneFindWidget(&theScene, "AUTO_CHBOX_3");
		assert(AUTO_CHBOX_3);

		MD_CHBOX_SW = ituSceneFindWidget(&theScene, "MD_CHBOX_SW");
		assert(MD_CHBOX_SW);
		MD_MAIN_TEXT_2_1_1 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_2_1_1");
		assert(MD_MAIN_TEXT_2_1_1);
		MD_MAIN_TEXT_2_1_2 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_2_1_2");
		assert(MD_MAIN_TEXT_2_1_2);
		MD_MAIN_TEXT_2_1_3 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_2_1_3");
		assert(MD_MAIN_TEXT_2_1_3);
		MD_MAIN_TEXT_2_1_4 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_2_1_4");
		assert(MD_MAIN_TEXT_2_1_4);
		MD_MAIN_TEXT_3_1_1 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_3_1_1");
		assert(MD_MAIN_TEXT_3_1_1);
		MD_MAIN_TEXT_3_1_2 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_3_1_2");
		assert(MD_MAIN_TEXT_3_1_2);
		MD_MAIN_TEXT_4_1_1 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_4_1_1");
		assert(MD_MAIN_TEXT_4_1_1);
		MD_MAIN_TEXT_4_1_2 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_4_1_2");
		assert(MD_MAIN_TEXT_4_1_2);
		MD_MAIN_TEXT_4_1_3 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_4_1_3");
		assert(MD_MAIN_TEXT_4_1_3);
		MD_MAIN_TEXT_5_1_1 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_5_1_1");
		assert(MD_MAIN_TEXT_5_1_1);
		MD_MAIN_TEXT_5_1_2 = ituSceneFindWidget(&theScene, "MD_MAIN_TEXT_5_1_2");
		assert(MD_MAIN_TEXT_5_1_2);

		MD_2_CHBOX_1 = ituSceneFindWidget(&theScene, "MD_2_CHBOX_1");
		assert(MD_2_CHBOX_1);
		MD_2_CHBOX_2 = ituSceneFindWidget(&theScene, "MD_2_CHBOX_2");
		assert(MD_2_CHBOX_2);
		MD_2_CHBOX_3 = ituSceneFindWidget(&theScene, "MD_2_CHBOX_3");
		assert(MD_2_CHBOX_3);
		MD_2_CHBOX_4 = ituSceneFindWidget(&theScene, "MD_2_CHBOX_4");
		assert(MD_2_CHBOX_4);
		MD_3_CHBOX_1 = ituSceneFindWidget(&theScene, "MD_3_CHBOX_1");
		assert(MD_3_CHBOX_1);
		MD_3_CHBOX_2 = ituSceneFindWidget(&theScene, "MD_3_CHBOX_2");
		assert(MD_3_CHBOX_2);
		MD_4_CHBOX_1 = ituSceneFindWidget(&theScene, "MD_4_CHBOX_1");
		assert(MD_4_CHBOX_1);
		MD_4_CHBOX_2 = ituSceneFindWidget(&theScene, "MD_4_CHBOX_2");
		assert(MD_4_CHBOX_2);
		MD_4_CHBOX_3 = ituSceneFindWidget(&theScene, "MD_4_CHBOX_3");
		assert(MD_4_CHBOX_3);
		MD_5_CHBOX_SW = ituSceneFindWidget(&theScene, "MD_5_CHBOX_SW");
		assert(MD_5_CHBOX_SW);
		MD_5_WHEEL_1 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_1");
		assert(MD_5_WHEEL_1);
		MD_5_WHEEL_2 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_2");
		assert(MD_5_WHEEL_2);
		MD_5_WHEEL_3 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_3");
		assert(MD_5_WHEEL_3);
		MD_5_WHEEL_4 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_4");
		assert(MD_5_WHEEL_4);
		MD_5_WHEEL_5 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_5");
		assert(MD_5_WHEEL_5);
		MD_5_WHEEL_6 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_6");
		assert(MD_5_WHEEL_6);
		MD_5_WHEEL_7 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_7");
		assert(MD_5_WHEEL_7);
		MD_5_WHEEL_8 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_8");
		assert(MD_5_WHEEL_8);
		MD_5_WHEEL_9 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_9");
		assert(MD_5_WHEEL_9);
		MD_5_WHEEL_10 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_10");
		assert(MD_5_WHEEL_10);
		MD_5_WHEEL_11 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_11");
		assert(MD_5_WHEEL_11);
		MD_5_WHEEL_12 = ituSceneFindWidget(&theScene, "MD_5_WHEEL_12");
		assert(MD_5_WHEEL_12);

		MD_MAIN_BG_CLOSE = ituSceneFindWidget(&theScene, "MD_MAIN_BG_CLOSE");
		assert(MD_MAIN_BG_CLOSE);
		MD_MAIN_BTN_2_ = ituSceneFindWidget(&theScene, "MD_MAIN_BTN_2_");
		assert(MD_MAIN_BTN_2_);
		MD_MAIN_BTN_3_ = ituSceneFindWidget(&theScene, "MD_MAIN_BTN_3_");
		assert(MD_MAIN_BTN_3_);
		MD_MAIN_BTN_4_ = ituSceneFindWidget(&theScene, "MD_MAIN_BTN_4_");
		assert(MD_MAIN_BTN_4_);
		MD_MAIN_BTN_5_ = ituSceneFindWidget(&theScene, "MD_MAIN_BTN_5_");
		assert(MD_MAIN_BTN_5_);
		MD_5_BG_CLOSE = ituSceneFindWidget(&theScene, "MD_5_BG_CLOSE");
		assert(MD_5_BG_CLOSE);

		for(i = 0; i < 24; i++)
		{
			MD_5_BTN[i] = ituSceneFindWidget(&theScene, md_btn_str[i]);
			assert(MD_5_BTN[i]);
		}

		VOL_RING_TR_BAR = ituSceneFindWidget(&theScene, "VOL_RING_TR_BAR");
		assert(VOL_RING_TR_BAR);
		VOL_RING_PR_BAR = ituSceneFindWidget(&theScene, "VOL_RING_PR_BAR");
		assert(VOL_RING_PR_BAR);
		VOL_TEXT_1 = ituSceneFindWidget(&theScene, "VOL_TEXT_1");
		assert(VOL_TEXT_1);
		VOL_TALK_TR_BAR = ituSceneFindWidget(&theScene, "VOL_TALK_TR_BAR");
		assert(VOL_TALK_TR_BAR);
		VOL_TALK_PR_BAR = ituSceneFindWidget(&theScene, "VOL_TALK_PR_BAR");
		assert(VOL_TALK_PR_BAR);
		VOL_TEXT_2 = ituSceneFindWidget(&theScene, "VOL_TEXT_2");
		assert(VOL_TEXT_2);

		VOL_RING_TR_BAR_ = ituSceneFindWidget(&theScene, "VOL_RING_TR_BAR_");
		assert(VOL_RING_TR_BAR_);
		VOL_RING_PR_BAR_ = ituSceneFindWidget(&theScene, "VOL_RING_PR_BAR_");
		assert(VOL_RING_PR_BAR_);
		VOL_TEXT_3 = ituSceneFindWidget(&theScene, "VOL_TEXT_3");
		assert(VOL_TEXT_3);
		VOL_TALK_TR_BAR_ = ituSceneFindWidget(&theScene, "VOL_TALK_TR_BAR_");
		assert(VOL_TALK_TR_BAR_);
		VOL_TALK_PR_BAR_ = ituSceneFindWidget(&theScene, "VOL_TALK_PR_BAR_");
		assert(VOL_TALK_PR_BAR_);
		VOL_TEXT_4 = ituSceneFindWidget(&theScene, "VOL_TEXT_4");
		assert(VOL_TEXT_4);

		VOL_RING_ICON_1 = ituSceneFindWidget(&theScene, "VOL_RING_ICON_1");
		assert(VOL_RING_ICON_1);
		VOL_RING_ICON_2 = ituSceneFindWidget(&theScene, "VOL_RING_ICON_2");
		assert(VOL_RING_ICON_2);
		VOL_RING_ICON_3 = ituSceneFindWidget(&theScene, "VOL_RING_ICON_3");
		assert(VOL_RING_ICON_3);
		VOL_RING_ICON_4 = ituSceneFindWidget(&theScene, "VOL_RING_ICON_4");
		assert(VOL_RING_ICON_4);
		VOL_TALK_ICON_1 = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_1");
		assert(VOL_TALK_ICON_1);
		VOL_TALK_ICON_2 = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_2");
		assert(VOL_TALK_ICON_2);
		VOL_TALK_ICON_3 = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_3");
		assert(VOL_TALK_ICON_3);
		VOL_TALK_ICON_4 = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_4");
		assert(VOL_TALK_ICON_4);

		VOL_RING_ICON_1_ = ituSceneFindWidget(&theScene, "VOL_RING_ICON_1_");
		assert(VOL_RING_ICON_1_);
		VOL_RING_ICON_2_ = ituSceneFindWidget(&theScene, "VOL_RING_ICON_2_");
		assert(VOL_RING_ICON_2_);
		VOL_RING_ICON_3_ = ituSceneFindWidget(&theScene, "VOL_RING_ICON_3_");
		assert(VOL_RING_ICON_3_);
		VOL_RING_ICON_4_ = ituSceneFindWidget(&theScene, "VOL_RING_ICON_4_");
		assert(VOL_RING_ICON_4_);
		VOL_TALK_ICON_1_ = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_1_");
		assert(VOL_TALK_ICON_1_);
		VOL_TALK_ICON_2_ = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_2_");
		assert(VOL_TALK_ICON_2_);
		VOL_TALK_ICON_3_ = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_3_");
		assert(VOL_TALK_ICON_3_);
		VOL_TALK_ICON_4_ = ituSceneFindWidget(&theScene, "VOL_TALK_ICON_4_");
		assert(VOL_TALK_ICON_4_);

		SET_MAIN_4_TIME = ituSceneFindWidget(&theScene, "SET_MAIN_4_TIME");
		assert(SET_MAIN_4_TIME);
		TIME_WHEEL_1 = ituSceneFindWidget(&theScene, "TIME_WHEEL_1");
		assert(TIME_WHEEL_1);
		TIME_WHEEL_2 = ituSceneFindWidget(&theScene, "TIME_WHEEL_2");
		assert(TIME_WHEEL_2);
		TIME_WHEEL_3 = ituSceneFindWidget(&theScene, "TIME_WHEEL_3");
		assert(TIME_WHEEL_3);
		TIME_WHEEL_4 = ituSceneFindWidget(&theScene, "TIME_WHEEL_4");
		assert(TIME_WHEEL_4);
		TIME_WHEEL_5 = ituSceneFindWidget(&theScene, "TIME_WHEEL_5");
		assert(TIME_WHEEL_5);
		TIME_WHEEL_6 = ituSceneFindWidget(&theScene, "TIME_WHEEL_6");
		assert(TIME_WHEEL_6);

		TO_CHBOX_SW = ituSceneFindWidget(&theScene, "TO_CHBOX_SW");
		assert(TO_CHBOX_SW);
		TO_MAIN_TEXT_2_1_1 = ituSceneFindWidget(&theScene, "TO_MAIN_TEXT_2_1_1");
		assert(TO_MAIN_TEXT_2_1_1);
		TO_MAIN_TEXT_2_1_2 = ituSceneFindWidget(&theScene, "TO_MAIN_TEXT_2_1_2");
		assert(TO_MAIN_TEXT_2_1_2);
		TO_MAIN_TEXT_2_1_3 = ituSceneFindWidget(&theScene, "TO_MAIN_TEXT_2_1_3");
		assert(TO_MAIN_TEXT_2_1_3);
		/*
		TO_MAIN_TEXT_31 = ituSceneFindWidget(&theScene, "TO_MAIN_TEXT_31");
		assert(TO_MAIN_TEXT_31);
		*/
		TO_2_CHBOX_1 = ituSceneFindWidget(&theScene, "TO_2_CHBOX_1");
		assert(TO_2_CHBOX_1);
		TO_2_CHBOX_2 = ituSceneFindWidget(&theScene, "TO_2_CHBOX_2");
		assert(TO_2_CHBOX_2);
		TO_2_CHBOX_3 = ituSceneFindWidget(&theScene, "TO_2_CHBOX_3");
		assert(TO_2_CHBOX_3);
		TO_3_CHBOX_1 = ituSceneFindWidget(&theScene, "TO_3_CHBOX_1");
		assert(TO_3_CHBOX_1);
		TO_3_CHBOX_2 = ituSceneFindWidget(&theScene, "TO_3_CHBOX_2");
		assert(TO_3_CHBOX_2);
		TO_MAIN_BG_CLOSE = ituSceneFindWidget(&theScene, "TO_MAIN_BG_CLOSE");
		assert(TO_MAIN_BG_CLOSE);
		TO_MAIN_BTN_2_ = ituSceneFindWidget(&theScene, "TO_MAIN_BTN_2_");
		assert(TO_MAIN_BTN_2_);
		/*
		LANG_CHBOX_1 = ituSceneFindWidget(&theScene, "LANG_CHBOX_1");
		assert(LANG_CHBOX_1);
		LANG_CHBOX_2 = ituSceneFindWidget(&theScene, "LANG_CHBOX_2");
		assert(LANG_CHBOX_2);
		LANG_CHBOX_3 = ituSceneFindWidget(&theScene, "LANG_CHBOX_3");
		assert(LANG_CHBOX_3);
		LANG_CHBOX_4 = ituSceneFindWidget(&theScene, "LANG_CHBOX_4");
		assert(LANG_CHBOX_4);
		LANG_CHBOX_5 = ituSceneFindWidget(&theScene, "LANG_CHBOX_5");
		assert(LANG_CHBOX_5);
		LANG_CHBOX_6 = ituSceneFindWidget(&theScene, "LANG_CHBOX_6");
		assert(LANG_CHBOX_6);
		*/
		ID_CHBOX_1 = ituSceneFindWidget(&theScene, "ID_CHBOX_1");
		assert(ID_CHBOX_1);
		ID_CHBOX_2 = ituSceneFindWidget(&theScene, "ID_CHBOX_2");
		assert(ID_CHBOX_2);
		ID_CHBOX_3 = ituSceneFindWidget(&theScene, "ID_CHBOX_3");
		assert(ID_CHBOX_3);
		ID_CHBOX_4 = ituSceneFindWidget(&theScene, "ID_CHBOX_4");
		assert(ID_CHBOX_4);

		OPEN_CHBOX_1 = ituSceneFindWidget(&theScene, "OPEN_CHBOX_1");
		assert(OPEN_CHBOX_1);
		OPEN_CHBOX_2 = ituSceneFindWidget(&theScene, "OPEN_CHBOX_2");
		assert(OPEN_CHBOX_2);

		WASH_TEXT_3 = ituSceneFindWidget(&theScene, "WASH_TEXT_3");
		assert(WASH_TEXT_3);
		SET_MAIN_5_WASH = ituSceneFindWidget(&theScene, "SET_MAIN_5_WASH");
		assert(SET_MAIN_5_WASH);
		SET_BG = ituSceneFindWidget(&theScene, "SET_BG");
		assert(SET_BG);
		DIS_ICON_WIN = ituSceneFindWidget(&theScene, "DIS_ICON_WIN");
		assert(DIS_ICON_WIN);
		ituWidgetSetDraw(DIS_ICON_WIN, VideoPlayerViewBackgroundDraw);

		DIS_TR_BAR_BRI = ituSceneFindWidget(&theScene, "DIS_TR_BAR_BRI");
		assert(DIS_TR_BAR_BRI);
		DIS_TR_BAR_CON = ituSceneFindWidget(&theScene, "DIS_TR_BAR_CON");
		assert(DIS_TR_BAR_CON);
		DIS_TR_BAR_HUE = ituSceneFindWidget(&theScene, "DIS_TR_BAR_HUE");
		assert(DIS_TR_BAR_HUE);
		DIS_PR_BAR_BRI = ituSceneFindWidget(&theScene, "DIS_PR_BAR_BRI");
		assert(DIS_PR_BAR_BRI);
		DIS_PR_BAR_CON = ituSceneFindWidget(&theScene, "DIS_PR_BAR_CON");
		assert(DIS_PR_BAR_CON);
		DIS_PR_BAR_HUE = ituSceneFindWidget(&theScene, "DIS_PR_BAR_HUE");
		assert(DIS_PR_BAR_HUE);

		FORMAT_TEXT_TITEL = ituSceneFindWidget(&theScene, "FORMAT_TEXT_TITEL");
		assert(FORMAT_TEXT_TITEL);
		FORMAT_BTN_YES = ituSceneFindWidget(&theScene, "FORMAT_BTN_YES");
		assert(FORMAT_BTN_YES);
		FORMAT_BTN_NO = ituSceneFindWidget(&theScene, "FORMAT_BTN_NO");
		assert(FORMAT_BTN_NO);
		FORMAT_TEXT_TITEL_ING = ituSceneFindWidget(&theScene, "FORMAT_TEXT_TITEL_ING");
		assert(FORMAT_TEXT_TITEL_ING);
		FORMAT_SPRITE = ituSceneFindWidget(&theScene, "FORMAT_SPRITE");
		assert(FORMAT_SPRITE);
		FORMAT_TEXT_NO = ituSceneFindWidget(&theScene, "FORMAT_TEXT_NO");
		assert(FORMAT_TEXT_NO);
		FORMAT_TEXT_YES = ituSceneFindWidget(&theScene, "FORMAT_TEXT_YES");
		assert(FORMAT_TEXT_YES);

		DEL_CHBOX_1 = ituSceneFindWidget(&theScene, "DEL_CHBOX_1");
		assert(DEL_CHBOX_1);
		DEL_CHBOX_2 = ituSceneFindWidget(&theScene, "DEL_CHBOX_2");
		assert(DEL_CHBOX_2);
		DEL_TEXT_1 = ituSceneFindWidget(&theScene, "DEL_TEXT_1");
		assert(DEL_TEXT_1);
		DEL_TEXT_2 = ituSceneFindWidget(&theScene, "DEL_TEXT_2");
		assert(DEL_TEXT_2);
		DEL_BTN_YES = ituSceneFindWidget(&theScene, "DEL_BTN_YES");
		assert(DEL_BTN_YES);
		DEL_BTN_NO = ituSceneFindWidget(&theScene, "DEL_BTN_NO");
		assert(DEL_BTN_NO);
		DEL_TEXT_TITEL_ING = ituSceneFindWidget(&theScene, "DEL_TEXT_TITEL_ING");
		assert(DEL_TEXT_TITEL_ING);
		DEL_SPRITE = ituSceneFindWidget(&theScene, "DEL_SPRITE");
		assert(DEL_SPRITE);
		DEL_TEXT_NO = ituSceneFindWidget(&theScene, "DEL_TEXT_NO");
		assert(DEL_TEXT_NO);
		DEL_TEXT_YES = ituSceneFindWidget(&theScene, "DEL_TEXT_YES");
		assert(DEL_TEXT_YES);

		SET_MAIN_1_AUTO = ituSceneFindWidget(&theScene, "SET_MAIN_1_AUTO");
		assert(SET_MAIN_1_AUTO);

		SET_MAIN_1_MD = ituSceneFindWidget(&theScene, "SET_MAIN_1_MD");
		assert(SET_MAIN_1_MD);
		MD_2 = ituSceneFindWidget(&theScene, "MD_2");
		assert(MD_2);
		MD_3 = ituSceneFindWidget(&theScene, "MD_3");
		assert(MD_3);
		MD_4 = ituSceneFindWidget(&theScene, "MD_4");
		assert(MD_4);
		MD_5 = ituSceneFindWidget(&theScene, "MD_5");
		assert(MD_5);
		MD_BG_HEAD = ituSceneFindWidget(&theScene, "MD_BG_HEAD");
		assert(MD_BG_HEAD);
		MD_BG_MAIN = ituSceneFindWidget(&theScene, "MD_BG_MAIN");
		assert(MD_BG_MAIN);

		SET_MAIN_2_VOL = ituSceneFindWidget(&theScene, "SET_MAIN_2_VOL");
		assert(SET_MAIN_2_VOL);

		SET_MAIN_3_FORMAT = ituSceneFindWidget(&theScene, "SET_MAIN_3_FORMAT");
		assert(SET_MAIN_3_FORMAT);

		SET_MAIN_3_DEL = ituSceneFindWidget(&theScene, "SET_MAIN_3_DEL");
		assert(SET_MAIN_3_DEL);

		SET_MAIN_5_DIS = ituSceneFindWidget(&theScene, "SET_MAIN_5_DIS");
		assert(SET_MAIN_5_DIS);

		SET_MAIN_5_TO = ituSceneFindWidget(&theScene, "SET_MAIN_5_TO");
		assert(SET_MAIN_5_TO);
		TO_2 = ituSceneFindWidget(&theScene, "TO_2");
		assert(TO_2);
		TO_3 = ituSceneFindWidget(&theScene, "TO_3");
		assert(TO_3);
		TO_BG_HEAD = ituSceneFindWidget(&theScene, "TO_BG_HEAD");
		assert(TO_BG_HEAD);
		TO_BG_MAIN = ituSceneFindWidget(&theScene, "TO_BG_MAIN");
		assert(TO_BG_MAIN);

		SET_MAIN_6_LANG = ituSceneFindWidget(&theScene, "SET_MAIN_6_LANG");
		assert(SET_MAIN_6_LANG);

		SET_MAIN_6_ID = ituSceneFindWidget(&theScene, "SET_MAIN_6_ID");
		assert(SET_MAIN_6_ID);

		SET_MAIN_6_RSET = ituSceneFindWidget(&theScene, "SET_MAIN_6_RSET");
		assert(SET_MAIN_6_RSET);

		SET_MAIN_6_OPEN = ituSceneFindWidget(&theScene, "SET_MAIN_6_OPEN");
		assert(SET_MAIN_6_OPEN);
		LANG_SLISTBOX_0 = ituSceneFindWidget(&theScene, "LANG_SLISTBOX_0");
		assert(LANG_SLISTBOX_0);
		LANG_SLISTBOX_1 = ituSceneFindWidget(&theScene, "LANG_SLISTBOX_1");
		assert(LANG_SLISTBOX_1);
		for(i =0; i<12; i++)
		{
			LANG_TEXT[i] = ituSceneFindWidget(&theScene, lang_text_str[i]);
			assert(LANG_TEXT[i]);
		}
	}

	user_amp_off();

	set_all_init();

	set_main_init();
	set_auto_init();
	set_md_init();
	set_volume_init();
	set_save_init();
	set_standby_init();
	set_ohter_init();

	set_main_bg_1_hide(widget,param);
	set_main_bg_1_show(widget,param);
	wash_time = 0;

	return true;
}

bool set_timer(ITUWidget* widget, char* param)
{
	if(wash_time)
	{
		uint32_t tick = SDL_GetTicks();
		uint32_t diff = tick - wash_tick;
		if(diff  >= 1000)
		{
			wash_tick = tick;
			wash_time--;
			char str[6] = {'\0'};
			str[0] = '0';
			str[1] = '0';
			str[2] = ':';
			str[3] = '0'+wash_time/10;
			str[4] = '0'+wash_time%10;
			str[5] = '\0';
			ituTextSetString(WASH_TEXT_3, str);
		}
		if(wash_time == 0)
		{
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
	}

	if(format_over)
	{
		format_over = false;
		ituWidgetSetVisible(SET_MAIN_3_FORMAT, false);
		ituWidgetEnable(SET_BG);
		set_save_init();
	}

	if(delete_over)
	{
		delete_over = false;
		ituWidgetSetVisible(SET_MAIN_3_DEL, false);
		ituWidgetEnable(SET_BG);
		set_save_init();
	}
	return true;
}

bool set_leave(ITUWidget* widget, char* param)
{
	if(ituWidgetIsVisible(SET_MAIN_5_DIS) && !door_call_num)
	{
		gState = SEND_STOP;				//停止图像
		if(master_vdp)
		{
			ithGpioSet(cam_gpio[DOOR_1][ON]);
			ithGpioSet(cam_gpio[DOOR_2][ON]);
		}
		cur_signal = 0;					//当前信号标志初始化
		SceneLeaveVideoState();			//切换帧率
		usleep(100*1000);
		PR2000_set_end();
	}
	set_display_signal = false;
	return true;
}

bool set_format(ITUWidget* widget, char* param)
{
	if(StorageGetCurrType() == STORAGE_SD)
		return true;
	else
		return false;
}
void time_wheel_update()
{
	struct timeval tv;
	struct tm *tm, mytime;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	memcpy(&mytime, tm, sizeof (struct tm));

	ituWheelGoto(TIME_WHEEL_1,(mytime.tm_year - 100)-1);
	ituWheelGoto(TIME_WHEEL_2,mytime.tm_mon);
	ituWheelGoto(TIME_WHEEL_3,(mytime.tm_mday -1));
	ituWheelGoto(TIME_WHEEL_4,mytime.tm_hour);
	ituWheelGoto(TIME_WHEEL_5,mytime.tm_min);
	ituWheelGoto(TIME_WHEEL_6,mytime.tm_sec );
}

bool set_to_time_set(ITUWidget* widget, char* param)
{
	time_wheel_update();
	return true;
}

bool set_time_back(ITUWidget* widget, char* param)
{
	struct timeval tv;
	struct tm *tm, mytime;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	memcpy(&mytime, tm, sizeof (struct tm));

	mytime.tm_year = TIME_WHEEL_1->focusIndex + 100+1;
	mytime.tm_mon = TIME_WHEEL_2->focusIndex;
	mytime.tm_mday = TIME_WHEEL_3->focusIndex + 1;
	mytime.tm_hour = TIME_WHEEL_4->focusIndex;
	mytime.tm_min = TIME_WHEEL_5->focusIndex;
	mytime.tm_sec = TIME_WHEEL_6->focusIndex;

	tv.tv_sec = mktime(&mytime);
	tv.tv_usec = 0;
	settimeofday(&tv, NULL);
	return true;
}

bool set_pop_up_cancel(ITUWidget* widget, char* param)
{
	set_main_init();
	set_auto_init();
	set_md_init();
	set_volume_init();
	set_standby_init();
	set_ohter_init();
	return true;
}

bool set_pop_up_confirm(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
	case '1':			//AUTO
		if(ituCheckBoxIsChecked(AUTO_CHBOX_1))
			theConfig.zidong = 0;
		else if(ituCheckBoxIsChecked(AUTO_CHBOX_2))
			theConfig.zidong = 1;
		else if(ituCheckBoxIsChecked(AUTO_CHBOX_3))
			theConfig.zidong = 2;
		set_main_init();
		break;
	case '2':			//MD_CAM
		if(ituCheckBoxIsChecked(MD_2_CHBOX_1))
			theConfig.mdcam = 0;
		else if(ituCheckBoxIsChecked(MD_2_CHBOX_2))
			theConfig.mdcam = 1;
		else if(ituCheckBoxIsChecked(MD_2_CHBOX_3))
			theConfig.mdcam = 2;
		else if(ituCheckBoxIsChecked(MD_2_CHBOX_4))
			theConfig.mdcam = 3;
		set_md_init();
		break;
	case '3':			//MD_TYPE
		if(ituCheckBoxIsChecked(MD_3_CHBOX_1))
			theConfig.mdsave = 0;
		else if(ituCheckBoxIsChecked(MD_3_CHBOX_2))
			theConfig.mdsave = 1;
		set_md_init();
		break;
	case '4':			//MD_SENSITIVITY
		if(ituCheckBoxIsChecked(MD_4_CHBOX_1))
			theConfig.mdsensitive = 0;
		else if(ituCheckBoxIsChecked(MD_4_CHBOX_2))
			theConfig.mdsensitive = 1;
		else if(ituCheckBoxIsChecked(MD_4_CHBOX_3))
			theConfig.mdsensitive= 2;
		set_md_init();
		break;
	case '5':			//FORMAT_SD
		//AudioPlayKeySound();
		//if(storageCurrType == STORAGE_SD)
		{
			printf("format_confirm..............1...............\n");
			user_format_init();
			usleep(100*1000);
			ituWidgetSetVisible(FORMAT_TEXT_NO, false);
			ituWidgetSetVisible(FORMAT_TEXT_YES, false);
			ituWidgetSetVisible(FORMAT_TEXT_TITEL, false);
			ituWidgetSetVisible(FORMAT_BTN_YES, false);
			ituWidgetSetVisible(FORMAT_BTN_NO, false);
			ituWidgetSetVisible(FORMAT_TEXT_TITEL_ING, true);
			ituWidgetSetVisible(FORMAT_SPRITE, true);
			printf("format_confirm...............2..............\n");
		}
		//user_format_sd();
		break;
	case '6':			//DEL_FILE
		//AudioPlayKeySound();
		user_delete_init();
		ituWidgetSetVisible(DEL_TEXT_NO, false);
		ituWidgetSetVisible(DEL_TEXT_YES, false);
		ituWidgetSetVisible(DEL_CHBOX_1, false);
		ituWidgetSetVisible(DEL_TEXT_1, false);
		ituWidgetSetVisible(DEL_CHBOX_2, false);
		ituWidgetSetVisible(DEL_TEXT_2, false);
		ituWidgetSetVisible(DEL_BTN_YES, false);
		ituWidgetSetVisible(DEL_BTN_NO, false);

		ituWidgetSetVisible(DEL_TEXT_TITEL_ING, true);
		ituWidgetSetVisible(DEL_SPRITE, true);
		break;
	case '7':			//STANBY_TIME
		if(ituCheckBoxIsChecked(TO_2_CHBOX_1))
			theConfig.lcdouttime = 0;
		else if(ituCheckBoxIsChecked(TO_2_CHBOX_2))
			theConfig.lcdouttime = 1;
		else if(ituCheckBoxIsChecked(TO_2_CHBOX_3))
			theConfig.lcdouttime= 2;
		set_standby_init();
		set_main_init();
		break;
	case '8':			//STANBY_FRAME
		if(ituCheckBoxIsChecked(TO_3_CHBOX_1))
			theConfig.infoframe = 0;
		else if(ituCheckBoxIsChecked(TO_3_CHBOX_2))
			theConfig.infoframe = 1;
		set_standby_init();
		set_main_init();
		break;
	case '9':			//RESET_SYSTEM
		Config_Reinit();
		set_all_init();
		set_main_init();
		set_auto_init();
		set_md_init();
		set_volume_init();
		set_standby_init();
		set_ohter_init();
		user_set_languaue();
		/*
		struct timeval tv;
		tv.tv_sec = CFG_RTC_DEFAULT_TIMESTAMP;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);
		clear_rtc_reset();
		*/
		break;
	case 'a':			//OPEN MODE
		if(ituCheckBoxIsChecked(OPEN_CHBOX_1))
			theConfig.open= 0;
		else if(ituCheckBoxIsChecked(OPEN_CHBOX_2))
			theConfig.open = 1;
		set_main_init();
		break;
	}
	if(param[0] != '5')
		ConfigSave();
	return true;
}
bool set_modify(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
	case '1':
		theConfig.md = MD_CHBOX_SW->checked;
		set_main_init();
		set_md_init();
		break;
	case '2':
		theConfig.mdtime = MD_5_CHBOX_SW->checked;
		set_md_init();
		set_md_init();
		//if(theConfig.mdtime == 1)
		break;
	case '3':
		theConfig.keyvol = 1- theConfig.keyvol;
		printf("theConfig.keyvol------1----->%d\n",theConfig.keyvol);
		set_main_init();

		break;
	case '4':
		theConfig.interphone++;
		if(theConfig.interphone>5)
			theConfig.interphone = 0;
		set_main_init();
		once_key_ring = 2;
		key_sound_reinit();
		user_amp_on();
		//ring_volume_set(3);
		AudioStop();
		AudioSetVolume(ALC5616_VOL);
		AudioPlay(ring_str[theConfig.interphone], NULL);
		break;
	case '5':
		theConfig.door1++;
		if(theConfig.door1>5)
			theConfig.door1 = 0;
		set_main_init();
		once_key_ring = 2;
		key_sound_reinit();
		user_amp_on();
		//ring_volume_set(3);
		AudioStop();
		AudioSetVolume(ALC5616_VOL);
		AudioPlay(ring_str[theConfig.door1], NULL);
		break;
	case '6':
		theConfig.door2++;
		if(theConfig.door2>5)
			theConfig.door2 = 0;
		set_main_init();
		once_key_ring = 2;
		key_sound_reinit();
		user_amp_on();
		//ring_volume_set(3);
		AudioStop();
		AudioSetVolume(ALC5616_VOL);
		AudioPlay(ring_str[theConfig.door2], NULL);
		user_amp_on();
		break;
	case '7':
		theConfig.lcdout = 1- theConfig.lcdout;
		set_main_init();
		set_standby_init();
		break;
	case '8':
		if(ID_CHBOX_1->checked)
			theConfig.id = 0;
		else if(ID_CHBOX_2->checked)
			theConfig.id = 1;
		else if(ID_CHBOX_3->checked)
			theConfig.id = 2;
		else if(ID_CHBOX_4->checked)
			theConfig.id = 3;
		set_main_init();
		break;
	case '9':
		break;
	case 'a':
		if(OPEN_CHBOX_1->checked)
			theConfig.open = 0;
		else if(OPEN_CHBOX_1->checked)
			theConfig.open = 1;
		break;
	}
	ConfigSave();
	return true;
}
bool volume_set(ITUWidget* widget, char* param)
{
	AudioStop();
	user_amp_off();
	return true;
}

bool set_vol_back(ITUWidget* widget, char* param)
{
	AudioStop();
	user_amp_off();
	return true;
}

bool set_enter_clear_mode(ITUWidget* widget, char* param)
{
	wash_tick = SDL_GetTicks();
	wash_time = 30;
	ituTextSetString(WASH_TEXT_3, "00:30");
	return true;
}
void _set_enter_display()
{
	ituWidgetDisable(SET_BG);
	ituWidgetSetVisible(SET_MAIN_5_DIS, true);
	ituWidgetIsEnabled(SET_BG);
	monitor_signal(cur_signal);
	PR2000_set_start();
	usleep(100*1000);
	gState = SEND_BEGIN;			//显示图像标志
	ituTrackBarSetValue(DIS_TR_BAR_BRI,theConfig.brightness);
	ituProgressBarSetValue(DIS_PR_BAR_BRI,theConfig.brightness);
	ituTrackBarSetValue(DIS_TR_BAR_CON,theConfig.contrast);
	ituProgressBarSetValue(DIS_PR_BAR_CON,theConfig.contrast);
	ituTrackBarSetValue(DIS_TR_BAR_HUE,theConfig.hue);
	ituProgressBarSetValue(DIS_PR_BAR_HUE,theConfig.hue);
}

bool set_enter_display(ITUWidget* widget, char* param)
{
	usleep(theConfig.id*100*1000);

	if(master_vdp || theConfig.id == 0)
	{
		if(signal_insert[DOOR_1] || signal_insert[DOOR_2])
		{
			set_display_signal = true;
			if(signal_insert[DOOR_1])
				cur_signal = signal_door_1;
			else
				cur_signal = signal_door_2;
			_set_enter_display();
		}
	}
	else
	{
		set_display_signal = true;
	}
	return true;
}

bool set_dis_reset(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	theConfig.brightness = 50;
	theConfig.contrast = 50;
	theConfig.hue = 50;
	ConfigSave();
	ituTrackBarSetValue(DIS_TR_BAR_BRI,50);
	ituProgressBarSetValue(DIS_PR_BAR_BRI,50);
	ituTrackBarSetValue(DIS_TR_BAR_CON,50);
	ituProgressBarSetValue(DIS_PR_BAR_CON,50);
	ituTrackBarSetValue(DIS_TR_BAR_HUE,50);
	ituProgressBarSetValue(DIS_PR_BAR_HUE,50);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_BRGT,0X80-20);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_CONT,0X80+20);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_HUE,0X80+40);
	return true;
}

bool set_dis_dajust(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	switch(param[0])
	{
	case '1':
	case '2':
		if(param[0] == '1')
			theConfig.brightness--;
		else
			theConfig.brightness++;
		if(theConfig.brightness < 0)
			theConfig.brightness = 0;
		else if(theConfig.brightness > 100)
			theConfig.brightness = 100;
		ituTrackBarSetValue(DIS_TR_BAR_BRI,theConfig.brightness);
		ituProgressBarSetValue(DIS_PR_BAR_BRI,theConfig.brightness);
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.brightness+58);
		break;
	case '3':
	case '4':
		if(param[0] == '3')
			theConfig.contrast--;
		else
			theConfig.contrast++;
		if(theConfig.contrast < 0)
			theConfig.contrast = 0;
		else if(theConfig.contrast > 100)
			theConfig.contrast = 100;
		ituTrackBarSetValue(DIS_TR_BAR_CON,theConfig.contrast);
		ituProgressBarSetValue(DIS_PR_BAR_CON,theConfig.contrast);
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_CONT,theConfig.contrast+98);
		break;
	case '5':
	case '6':
		if(param[0] == '5')
			theConfig.hue--;
		else
			theConfig.hue++;
		if(theConfig.hue < 0)
			theConfig.hue = 0;
		else if(theConfig.hue > 100)
			theConfig.hue = 100;
		ituTrackBarSetValue(DIS_TR_BAR_HUE,theConfig.hue);
		ituProgressBarSetValue(DIS_PR_BAR_HUE,theConfig.hue);
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_HUE,theConfig.hue+118);
		break;
	}
	ConfigSave();
	return true;
}

bool set_dis_back(ITUWidget* widget, char* param)
{
	gState = SEND_STOP;				//停止图像
	//cam_all_off();					//关闭所有摄像头电源
	cam_power_off();
	cur_signal = 0;					//当前信号标志初始化
	SceneLeaveVideoState();			//切换帧率
	usleep(100*1000);
	PR2000_set_end();
	return true;
}

bool set_dis_ch_bri(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_1->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_BRGT,DIS_TR_BAR_BRI->value+58);
	theConfig.brightness = DIS_TR_BAR_BRI->value;
	ConfigSave();
	return true;
}

bool set_dis_ch_con(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_2->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_CONT,DIS_TR_BAR_CON->value+98);
	theConfig.contrast = DIS_TR_BAR_CON->value;
	ConfigSave();
	return true;
}

bool set_dis_ch_hue(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_3->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_HUE,DIS_TR_BAR_HUE->value+118);
	theConfig.hue = DIS_TR_BAR_HUE->value;
	ConfigSave();
	return true;
}
bool set_vol_adjust(ITUWidget* widget, char* param)
{
	char val_str[5] = {'\0'};

	switch(param[0])
	{
	case '1':
		theConfig.ringvol--;
		if(theConfig.ringvol < 0)
			theConfig.ringvol = 0;
		if(theConfig.ringvol == 0 && theConfig.Iringvol == 0)
			theConfig.mute = 1;
		else
			theConfig.mute = 0;
		ituTrackBarSetValue(VOL_RING_TR_BAR,theConfig.ringvol);
		ituProgressBarSetValue(VOL_RING_PR_BAR,theConfig.ringvol);
		sprintf(val_str,"%d",theConfig.ringvol);
		ituTextSetString(VOL_TEXT_1,val_str);
		ring_volume_set(theConfig.ringvol);
		AudioSetVolume(ALC5616_VOL);
		key_sound_reinit();
		user_amp_on();
		if(theConfig.ringvol == 0)
			AudioStop();
		else
			AudioPlay(ring_str[theConfig.door1], NULL);
		break;
	case '2':
		theConfig.ringvol++;
		if(theConfig.ringvol > 3)
			theConfig.ringvol = 3;
		theConfig.mute = 0;
		ituTrackBarSetValue(VOL_RING_TR_BAR,theConfig.ringvol);
		ituProgressBarSetValue(VOL_RING_PR_BAR,theConfig.ringvol);
		sprintf(val_str,"%d",theConfig.ringvol);
		ituTextSetString(VOL_TEXT_1,val_str);
		ring_volume_set(theConfig.ringvol);
		AudioSetVolume(ALC5616_VOL);
		key_sound_reinit();
		user_amp_on();
		AudioPlay(ring_str[theConfig.door1], NULL);
		break;
	case '3':
		theConfig.talkvol--;
		if(theConfig.talkvol < 1)
			theConfig.talkvol = 1;
		ituTrackBarSetValue(VOL_RING_TR_BAR,theConfig.talkvol);
		ituProgressBarSetValue(VOL_TALK_PR_BAR,theConfig.talkvol);
		sprintf(val_str,"%d",theConfig.talkvol);
		ituTextSetString(VOL_TEXT_2,val_str);
		talk_volume_set(theConfig.talkvol);
		break;
	case '4':
		theConfig.talkvol++;
		if(theConfig.talkvol > 3)
			theConfig.talkvol = 3;
		ituTrackBarSetValue(VOL_RING_TR_BAR,theConfig.talkvol);
		ituProgressBarSetValue(VOL_TALK_PR_BAR,theConfig.talkvol);
		sprintf(val_str,"%d",theConfig.talkvol);
		ituTextSetString(VOL_TEXT_2,val_str);
		talk_volume_set(theConfig.talkvol);
		break;
	case '5':
		theConfig.Iringvol--;
		if(theConfig.Iringvol < 0)
			theConfig.Iringvol = 0;
		if(theConfig.ringvol == 0 && theConfig.Iringvol == 0)
			theConfig.mute = 1;
		else
			theConfig.mute = 0;
		ituTrackBarSetValue(VOL_RING_TR_BAR_,theConfig.Iringvol);
		ituProgressBarSetValue(VOL_RING_PR_BAR_,theConfig.Iringvol);
		sprintf(val_str,"%d",theConfig.Iringvol);
		ituTextSetString(VOL_TEXT_3,val_str);
		ring_volume_set(theConfig.Iringvol);
		AudioSetVolume(ALC5616_VOL);
		key_sound_reinit();
		user_amp_on();
		if(theConfig.Iringvol == 0)
			AudioStop();
		else
			AudioPlay(ring_str[theConfig.door1], NULL);
		break;
	case '6':
		theConfig.Iringvol++;
		if(theConfig.Iringvol > 3)
			theConfig.Iringvol = 3;
		theConfig.mute = 0;
		ituTrackBarSetValue(VOL_RING_TR_BAR_,theConfig.Iringvol);
		ituProgressBarSetValue(VOL_RING_PR_BAR_,theConfig.Iringvol);
		sprintf(val_str,"%d",theConfig.Iringvol);
		ituTextSetString(VOL_TEXT_3,val_str);
		ring_volume_set(theConfig.Iringvol);
		AudioSetVolume(ALC5616_VOL);
		key_sound_reinit();
		user_amp_on();
		AudioPlay(ring_str[theConfig.door1], NULL);
		break;
	case '7':
		theConfig.Italkvol--;
		if(theConfig.Italkvol < 1)
			theConfig.Italkvol = 1;
		ituTrackBarSetValue(VOL_RING_TR_BAR_,theConfig.Italkvol);
		ituProgressBarSetValue(VOL_TALK_PR_BAR_,theConfig.Italkvol);
		sprintf(val_str,"%d",theConfig.Italkvol);
		ituTextSetString(VOL_TEXT_4,val_str);
		talk_volume_set(theConfig.Italkvol);
		break;
	case '8':
		theConfig.Italkvol++;
		if(theConfig.Italkvol > 3)
			theConfig.Italkvol = 3;
		ituTrackBarSetValue(VOL_RING_TR_BAR_,theConfig.Italkvol);
		ituProgressBarSetValue(VOL_TALK_PR_BAR_,theConfig.Italkvol);
		sprintf(val_str,"%d",theConfig.Italkvol);
		ituTextSetString(VOL_TEXT_4,val_str);
		talk_volume_set(theConfig.Italkvol);
		break;
	}
	_volume_bar();
	ConfigSave();
	set_main_init();

	return true;
}

void md_time_wheel_load()
{
	printf("load md time......................................\n");
	ituWheelGoto(MD_5_WHEEL_1,theConfig.mdsyear - 2001);
	ituWheelGoto(MD_5_WHEEL_2,theConfig.mdsmonth -1);
	ituWheelGoto(MD_5_WHEEL_3,theConfig.mdsday-1);
	ituWheelGoto(MD_5_WHEEL_4,theConfig.mdshour);
	ituWheelGoto(MD_5_WHEEL_5,theConfig.mdsmin);
	ituWheelGoto(MD_5_WHEEL_6,theConfig.mdssec);
	printf("md_time_wheel_load--------->%d-%d-%d %d:%d:%d\n",theConfig.mdsyear,theConfig.mdsmonth
	       ,theConfig.mdsday,theConfig.mdshour
	       ,theConfig.mdsmin,theConfig.mdssec);
	printf("md_time_wheel_load--------->%d-%d-%d %d:%d:%d\n",MD_5_WHEEL_1->focusIndex,MD_5_WHEEL_2->focusIndex
	       ,MD_5_WHEEL_3->focusIndex,MD_5_WHEEL_4->focusIndex
	       ,MD_5_WHEEL_5->focusIndex,MD_5_WHEEL_6->focusIndex);
	ituWheelGoto(MD_5_WHEEL_7,theConfig.mdeyear - 2001);
	ituWheelGoto(MD_5_WHEEL_8,theConfig.mdemonth -1);
	ituWheelGoto(MD_5_WHEEL_9,theConfig.mdeday-1);
	ituWheelGoto(MD_5_WHEEL_10,theConfig.mdehour);
	ituWheelGoto(MD_5_WHEEL_11,theConfig.mdemin);
	ituWheelGoto(MD_5_WHEEL_12,theConfig.mdesec);
}

void md_time_wheel_update()
{
	struct timeval tv;
	struct tm *tm, mytime;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	memcpy(&mytime, tm, sizeof (struct tm));

	ituWheelGoto(MD_5_WHEEL_1,(mytime.tm_year - 100)-1);
	ituWheelGoto(MD_5_WHEEL_2,mytime.tm_mon);
	ituWheelGoto(MD_5_WHEEL_3,(mytime.tm_mday -1));
	ituWheelGoto(MD_5_WHEEL_4,mytime.tm_hour);
	ituWheelGoto(MD_5_WHEEL_5,mytime.tm_min);
	ituWheelGoto(MD_5_WHEEL_6,mytime.tm_sec );

	ituWheelGoto(MD_5_WHEEL_7,(mytime.tm_year - 100) -1);
	ituWheelGoto(MD_5_WHEEL_8,mytime.tm_mon);
	ituWheelGoto(MD_5_WHEEL_9,(mytime.tm_mday -1));
	ituWheelGoto(MD_5_WHEEL_10,mytime.tm_hour);
	ituWheelGoto(MD_5_WHEEL_11,mytime.tm_min);
	ituWheelGoto(MD_5_WHEEL_12,mytime.tm_sec );
}

bool set_md_time(ITUWidget* widget, char* param)
{
	//if(!theConfig.mdtime)
	//md_time_wheel_update();
	//else
	md_time_wheel_load();

	return true;
}

bool set_md_time_change(ITUWidget* widget, char* param)
{
#if 0
	if(md_wheel_change)
		return true;
	printf("set_md_time_change......................................\n");
	md_wheel_change = true;

	switch(param[0])
	{
	case '1':
	case 'd':
		if((MD_5_WHEEL_2->focusIndex + 1) == 2)		//年改变    二月处理
		{
			if((MD_5_WHEEL_1->focusIndex + 2001)% 4 == 0)
			{
				ituWheelSetItemTree(MD_5_WHEEL_3, &Feb_29_str,29);
			}
			else
			{
				if((MD_5_WHEEL_3->focusIndex + 1) > 28)
					ituWheelGoto(MD_5_WHEEL_3,28 - 1);
				ituWheelSetItemTree(MD_5_WHEEL_3, &Feb_28_str,28);
			}
		}
		break;
	case '2':
		/*
		if((MD_5_WHEEL_2->focusIndex + 1) == 1 || (MD_5_WHEEL_2->focusIndex + 1) == 3 || (MD_5_WHEEL_2->focusIndex + 1) == 5 ||
			(MD_5_WHEEL_2->focusIndex + 1) == 7 || (MD_5_WHEEL_2->focusIndex + 1) == 8 || (MD_5_WHEEL_2->focusIndex + 1) == 10 ||
			(MD_5_WHEEL_2->focusIndex + 1) == 12)
		{
				ituWheelSetItemTree(MD_5_WHEEL_3, month_31_str,31);
		}
		else if((MD_5_WHEEL_2->focusIndex + 1) == 2)
		{
			if((MD_5_WHEEL_3->focusIndex + 1) > 28)
				ituWheelGoto(MD_5_WHEEL_3,0);
			ituWheelSetItemTree(MD_5_WHEEL_3, Feb_28_str,28);
		}
		else
		{
			if((MD_5_WHEEL_3->focusIndex + 1) > 30)
				ituWheelGoto(MD_5_WHEEL_3,30 - 1);
			ituWheelSetItemTree(MD_5_WHEEL_3, month_30_str,30);
		}
			*/
		break;
	case '3':
		break;
	case '4':
		break;
	case '5':
		break;
	case '6':
		break;

	}

	md_wheel_change = false;
#endif
	return true;
}

bool set_md_time_back(ITUWidget* widget, char* param)
{
	theConfig.mdsyear = MD_5_WHEEL_1->focusIndex + 2001;
	theConfig.mdsmonth = MD_5_WHEEL_2->focusIndex + 1;
	theConfig.mdsday = MD_5_WHEEL_3->focusIndex + 1;
	theConfig.mdshour = MD_5_WHEEL_4->focusIndex;
	theConfig.mdsmin = MD_5_WHEEL_5->focusIndex;
	theConfig.mdssec = MD_5_WHEEL_6->focusIndex;

	theConfig.mdeyear = MD_5_WHEEL_7->focusIndex + 2001;
	theConfig.mdemonth = MD_5_WHEEL_8->focusIndex + 1;
	theConfig.mdeday = MD_5_WHEEL_9->focusIndex + 1;
	theConfig.mdehour = MD_5_WHEEL_10->focusIndex;
	theConfig.mdemin = MD_5_WHEEL_11->focusIndex;
	theConfig.mdesec = MD_5_WHEEL_12->focusIndex;

	ConfigSave();
	return true;
}

bool set_auto_video_check(ITUWidget* widget, char* param)
{
	ituCheckBoxSetChecked(AUTO_CHBOX_2,false);
	if(StorageGetCurrType() == STORAGE_SD)
	{
		ituCheckBoxSetChecked(AUTO_CHBOX_1,false);
		ituCheckBoxSetChecked(AUTO_CHBOX_2,true);
		ituCheckBoxSetChecked(AUTO_CHBOX_3,false);
	}
	return true;
}

bool set_md_video_check(ITUWidget* widget, char* param)
{
	ituCheckBoxSetChecked(MD_3_CHBOX_2,false);
	if(StorageGetCurrType() == STORAGE_SD)
	{
		ituCheckBoxSetChecked(MD_3_CHBOX_1,false);
		ituCheckBoxSetChecked(MD_3_CHBOX_2,true);
	}
	return true;
}

bool set_enter_lang(ITUWidget* widget, char* param)
{
	ituListBoxReload(LANG_SLISTBOX_0);
	ituListBoxReload(LANG_SLISTBOX_1);
	return true;
}

static void SettingLangChangeCommand(int arg)
{
	ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.language, 0, 0);
	ituSceneUpdate(&theScene, ITU_EVENT_LAYOUT, 0, 0, 0);
	//ConfigSave();
}

void user_set_languaue()
{
	ituSceneExecuteCommand(&theScene, 6, SettingLangChangeCommand, 0);
}

int lang_cur_page;
int image_cur_index;
bool set_change_lang(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollText* item;
	int i;

	item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
	i = (int)ituWidgetGetCustomData(item);
	if(i == theConfig.language)
		return true;
	theConfig.language = i;
	lang_cur_page = LANG_SLISTBOX_1->listbox.pageIndex;
	printf("lang ->%d      pageindex ->%d \n",theConfig.language,lang_cur_page);
	ituListBoxReload((ITUListBox*)LANG_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)LANG_SLISTBOX_1);
	if(lang_cur_page == 2)
	{
		ListBoxNextPage(&(LANG_SLISTBOX_0->listbox));
		ListBoxNextPage(&(LANG_SLISTBOX_1->listbox));
		usleep(1000);
		ListBoxNextPage(&(LANG_SLISTBOX_0->listbox));
		ListBoxNextPage(&(LANG_SLISTBOX_1->listbox));
		usleep(1000);
	}
	user_set_languaue();
	ConfigSave();

	return true;
}



