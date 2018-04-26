#include <assert.h>
#include "user_function.h"
#include "castor3player.h"

static char channel_str[4][10] 	= {"Door1","Door2","CCTV1","CCTV2"};
static char type_str[2][10] 	= {"MANUAL","AUTO"};

static ITUBackground* PHOTO_BG;
static ITUBackground* PHOTO_L_BG;

static ITUIcon* MEDIA_ICON_PHOTO_WIN;
static ITUIcon* MEDIA_ICON_VIDEO_WIN;
static ITUBackground* MEDIA_BG_VIDEO_BLACK;
static ITUScrollListBox* PHOTO_L_SLISTBOX_0;
static ITUScrollListBox* PHOTO_L_SLISTBOX_1;
static ITUScrollListBox* PHOTO_L_SLISTBOX_2;
static ITUScrollIconListBox* PHOTO_L_SLISTBOX_3;
static ITUText* PHOTO_TEXT_FILE_NO;
static ITUText* PHOTO_TEXT_FILE_NAME;

static ITUBackground* PHOTO_BG_DEL;
static ITUBackground* PHOTO_HEAD;
static ITUButton* PHOTO_BTN_PREV;
static ITUButton* PHOTO_BTN_NEXT;
static ITUBackgroundButton* PHOTO_BG_FULL_1;

static ITUBackground* VIDEO_BG;
static ITUBackground* VIDEO_L_BG;

static ITUScrollListBox* VIDEO_L_SLISTBOX_0;
static ITUScrollListBox* VIDEO_L_SLISTBOX_1;
static ITUScrollListBox* VIDEO_L_SLISTBOX_2;
static ITUScrollIconListBox* VIDEO_L_SLISTBOX_3;
static ITUText* VIDEO_TEXT_FILE_NO;
static ITUText* VIDEO_TEXT_FILE_NAME;
static ITUButton* VIDEO_BTN_PLAY;
static ITUButton* VIDEO_BTN_STOP;

static ITUTrackBar* VIDEO_TR_BAR;
static ITUProgressBar* VIDEO_PR_BAR;
static ITUText* VIDEO_TEXT_FILE_TIME;

static ITUTrackBar* VIDEO_TR_FULL_BAR;
static ITUProgressBar* VIDEO_PR_FULL_BAR;

static ITUBackground* VIDEO_BG_DEL;
static ITUBackground* VIDEO_HEAD;
static ITUButton* VIDEO_BTN_BACK;
static ITUButton* VIDEO_BTN_PREV;
static ITUButton* VIDEO_BTN_NEXT;
static ITUBackground* VIDEO_BG_BAR;
static ITUBackground* VIDEO_BG_FULL_BAR;


static ITUBackgroundButton* VIDEO_BG_NO_FULL;
static ITUBackgroundButton* VIDEO_BG_NO_FULL_;

static ITUButton* VIDEO_BTN_SLIDER;
static ITUButton* VIDEO_BTN_FULL_SLIDER;

// image---------
char imagePlayerPath[128];
bool imagePlayerLoading, imagePlayerLoaded;
static uint8_t* imagePlayerData;
static int imagePlayerDataSize;
static int imagePlayerNumber;
static int image_cur_page = 0;
static int image_cur_index = 0;

//video-------------
static int x, y, width, height = 0;
static MTAL_SPEC mtal_spec = {0};
int LastMediaPlayerVoice = 0;
bool videoPlayerIsFileEOF = true;
bool videoPlayerIsPlaying = false;
bool videoPlayerIsPause = false;
bool videoPlayerIsFirst = false;
int videoPlayerPercentage = 0;
bool MgrReloadSBCRunning = true;
static int VideoPlayerNumber;
static char video_cur_time[20] = "\0";;
static int video_total_time = 0;
static int video_prev_time = 0;
static long video_sec_time = 0;
static int video_cur_page = 0;
static int video_cur_index = 0;

bool stop_video(ITUWidget* widget, char* param);

bool user_get_videoPlayerIsPlaying()
{
	return videoPlayerIsPlaying;
}

void media_itu_init()
{
	if(ituWidgetIsVisible(PHOTO_BG))
	{
		ituWidgetSetVisible(PHOTO_BG, false);
		ituWidgetSetVisible(PHOTO_L_BG, true);
		ituWidgetSetVisible(PHOTO_BG_DEL, false);
		ituWidgetEnable(PHOTO_HEAD);
		ituWidgetEnable(PHOTO_BTN_PREV);
		ituWidgetEnable(PHOTO_BTN_NEXT);
		ituWidgetEnable(PHOTO_BG_FULL_1);

	}

	if(ituWidgetIsVisible(VIDEO_BG))
	{
		ituWidgetSetVisible(VIDEO_BG, false);
		ituWidgetSetVisible(VIDEO_L_BG, true);
		ituWidgetSetVisible(VIDEO_BG_DEL, false);
		ituWidgetEnable(VIDEO_BTN_PLAY);
		ituWidgetEnable(VIDEO_BTN_BACK);
	//ituWidgetDisable(VIDEO_BTN_BACK);
		ituWidgetEnable(VIDEO_BTN_PREV);
		ituWidgetEnable(VIDEO_BTN_NEXT);
	}

	if(ituWidgetIsVisible(VIDEO_BG_NO_FULL))
	{
		ituWidgetSetVisible(VIDEO_HEAD, true);
		ituWidgetSetVisible(VIDEO_BTN_BACK, true);
		ituWidgetSetVisible(VIDEO_BTN_PREV, true);
		ituWidgetSetVisible(VIDEO_BTN_NEXT, true);
		ituWidgetSetVisible(VIDEO_BG_BAR, true);
		ituWidgetSetVisible(VIDEO_BG_FULL_BAR, false);
		ituWidgetSetVisible(VIDEO_BG_NO_FULL_,true);
		ituWidgetSetVisible(VIDEO_BG_NO_FULL, false);
	}
}

static bool user_listbox_load(ITUWidget* widget, char* param,int listbox_num)
{
	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;
	ImageMemoEntry* entry;

	if(listbox_num < 4)
		entryCount = ImageMemoGetCount();
	else
		entryCount = VideoMemoGetCount();
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
		if(listbox_num < 4)
			entry = ImageMemoGetEntry(j);
		else
			entry = VideoMemoGetEntry(j);

		if(listbox_num == 0 || listbox_num == 4)
			ituTextSetString(scrolltext, entry->name);
		else if(listbox_num == 1 || listbox_num == 5)
			ituTextSetString(scrolltext, channel_str[(entry->cam-1)]);
		else if(listbox_num == 2 || listbox_num == 6)
			ituTextSetString(scrolltext, type_str[(entry->type)]);
		else if(listbox_num == 3 || listbox_num == 7)
		{
			if (entry->played)
				ituTextSetString(scrolltext, "1");
			else
				ituTextSetString(scrolltext, "0");
		}

		ituWidgetSetCustomData(scrolltext, j);
		//ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
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

	return true;
}
bool photo_l_slistbox_0_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,0);
}
bool photo_l_slistbox_1_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,1);
}
bool photo_l_slistbox_2_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,2);
}
bool photo_l_slistbox_3_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,3);
}
bool video_l_slistbox_0_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,4);
}
bool video_l_slistbox_1_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,5);
}
bool video_l_slistbox_2_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,6);
}
bool video_l_slistbox_3_load(ITUWidget* widget, char* param)
{
	return user_listbox_load(widget,param,7);
}
static void imagePlayerLoadCallback(uint8_t* data, int size)
{
	if (data && size > 0)
	{
		if (imagePlayerDataSize > 0)
		{
			free(imagePlayerData);
			imagePlayerData = NULL;
			imagePlayerDataSize = 0;
		}
		imagePlayerData = data;
		imagePlayerDataSize = size;
		imagePlayerLoaded = true;
	}
}

static void EventHandler(MJPEG_PLAYER_EVENT nEventID, void *arg)
{
	switch(nEventID)
	{
	case PLAYER_EVENT_EOF:
		printf("File EOF\n");
		videoPlayerIsFileEOF = true;
		videoPlayerIsPlaying = false;
		user_amp_off();
		//AudioStop();
		//mtal_pb_stop();
		//2018.3.28 my.wei add for UI command queue
#if 0//defined(SCENE_MSG_CMD_QUEUE_ENABLE)
		SceneEventHandler();
#else
		ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
		ituWidgetSetVisible(VIDEO_BTN_STOP, false);
		ituTrackBarSetValue(VIDEO_TR_BAR,952);
		ituProgressBarSetValue(VIDEO_PR_BAR,952);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,952);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,952);
		ituTextSetString(VIDEO_TEXT_FILE_TIME, "00:00");
		ituWidgetSetVisible(VIDEO_HEAD, true);
		ituWidgetSetVisible(VIDEO_BTN_BACK, true);
		ituWidgetSetVisible(VIDEO_BTN_PREV, true);
		ituWidgetSetVisible(VIDEO_BTN_NEXT, true);
		ituWidgetSetVisible(VIDEO_BG_BAR, true);
		ituWidgetSetVisible(VIDEO_BG_FULL_BAR, false);
		ituWidgetSetVisible(VIDEO_BG_NO_FULL_, true);
		ituWidgetSetVisible(VIDEO_BG_NO_FULL, false);
		clear_avi_playing();
#endif
		break;
	case PLAYER_EVENT_OPEN_FILE_FAIL:
		printf("Open file fail\n");
		videoPlayerIsFileEOF = true;
		break;
	case PLAYER_EVENT_GET_FIRST_FRAME:
		printf("First_frame........\n");
		if(videoPlayerIsFirst)
		{
			videoPlayerIsFirst = false;
			break;
		}
		stop_video(NULL,NULL);
		ituWidgetEnable(VIDEO_BTN_BACK);
		ituWidgetSetVisible(VIDEO_BTN_PREV, true);
		ituWidgetSetVisible(VIDEO_BTN_NEXT, true);
		ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
		ituWidgetSetVisible(MEDIA_BG_VIDEO_BLACK, false);
		break;
	default:
		break;
	}
}

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	//printf("VideoPlayerViewBackgroundDraw......................\n");
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;

	destx = rect->x + x;
	desty = rect->y + y;

	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

bool media_init(ITUWidget* widget, char* param)
{
	cur_page  = page_date;
	if (!MEDIA_ICON_PHOTO_WIN)
	{
		PHOTO_BG = ituSceneFindWidget(&theScene, "PHOTO_BG");
		assert(PHOTO_BG);
		PHOTO_L_BG = ituSceneFindWidget(&theScene, "PHOTO_L_BG");
		assert(PHOTO_L_BG);

		MEDIA_ICON_PHOTO_WIN = ituSceneFindWidget(&theScene, "MEDIA_ICON_PHOTO_WIN");
		assert(MEDIA_ICON_PHOTO_WIN);
		MEDIA_ICON_VIDEO_WIN = ituSceneFindWidget(&theScene, "MEDIA_ICON_VIDEO_WIN");
		assert(MEDIA_ICON_VIDEO_WIN);
		ituWidgetSetDraw(MEDIA_ICON_VIDEO_WIN, VideoPlayerViewBackgroundDraw);
		MEDIA_BG_VIDEO_BLACK = ituSceneFindWidget(&theScene, "MEDIA_BG_VIDEO_BLACK");
		assert(MEDIA_BG_VIDEO_BLACK);
		PHOTO_L_SLISTBOX_0 = ituSceneFindWidget(&theScene, "PHOTO_L_SLISTBOX_0");
		assert(PHOTO_L_SLISTBOX_0);
		PHOTO_L_SLISTBOX_1 = ituSceneFindWidget(&theScene, "PHOTO_L_SLISTBOX_1");
		assert(PHOTO_L_SLISTBOX_1);
		PHOTO_L_SLISTBOX_2 = ituSceneFindWidget(&theScene, "PHOTO_L_SLISTBOX_2");
		assert(PHOTO_L_SLISTBOX_2);
		PHOTO_L_SLISTBOX_3 = ituSceneFindWidget(&theScene, "PHOTO_L_SLISTBOX_3");
		assert(PHOTO_L_SLISTBOX_3);
		PHOTO_TEXT_FILE_NO = ituSceneFindWidget(&theScene, "PHOTO_TEXT_FILE_NO");
		assert(PHOTO_TEXT_FILE_NO);
		PHOTO_TEXT_FILE_NAME = ituSceneFindWidget(&theScene, "PHOTO_TEXT_FILE_NAME");
		assert(PHOTO_TEXT_FILE_NAME);

		VIDEO_BG = ituSceneFindWidget(&theScene, "VIDEO_BG");
		assert(VIDEO_BG);
		VIDEO_L_BG = ituSceneFindWidget(&theScene, "VIDEO_L_BG");
		assert(VIDEO_L_BG);

		VIDEO_L_SLISTBOX_0 = ituSceneFindWidget(&theScene, "VIDEO_L_SLISTBOX_0");
		assert(VIDEO_L_SLISTBOX_0);
		VIDEO_L_SLISTBOX_1 = ituSceneFindWidget(&theScene, "VIDEO_L_SLISTBOX_1");
		assert(VIDEO_L_SLISTBOX_1);
		VIDEO_L_SLISTBOX_2 = ituSceneFindWidget(&theScene, "VIDEO_L_SLISTBOX_2");
		assert(VIDEO_L_SLISTBOX_2);
		VIDEO_L_SLISTBOX_3 = ituSceneFindWidget(&theScene, "VIDEO_L_SLISTBOX_3");
		assert(VIDEO_L_SLISTBOX_3);
		VIDEO_TEXT_FILE_NO = ituSceneFindWidget(&theScene, "VIDEO_TEXT_FILE_NO");
		assert(VIDEO_TEXT_FILE_NO);
		VIDEO_TEXT_FILE_NAME = ituSceneFindWidget(&theScene, "VIDEO_TEXT_FILE_NAME");
		assert(VIDEO_TEXT_FILE_NAME);
		VIDEO_BTN_PLAY = ituSceneFindWidget(&theScene, "VIDEO_BTN_PLAY");
		assert(VIDEO_BTN_PLAY);
		VIDEO_BTN_STOP = ituSceneFindWidget(&theScene, "VIDEO_BTN_STOP");
		assert(VIDEO_BTN_STOP);

		VIDEO_TR_BAR = ituSceneFindWidget(&theScene, "VIDEO_TR_BAR");
		assert(VIDEO_TR_BAR);
		VIDEO_PR_BAR = ituSceneFindWidget(&theScene, "VIDEO_PR_BAR");
		assert(VIDEO_PR_BAR);
		VIDEO_TEXT_FILE_TIME = ituSceneFindWidget(&theScene, "VIDEO_TEXT_FILE_TIME");
		assert(VIDEO_TEXT_FILE_TIME);
		VIDEO_TR_FULL_BAR = ituSceneFindWidget(&theScene, "VIDEO_TR_FULL_BAR");
		assert(VIDEO_TR_FULL_BAR);
		VIDEO_PR_FULL_BAR = ituSceneFindWidget(&theScene, "VIDEO_PR_FULL_BAR");
		assert(VIDEO_PR_FULL_BAR);

		PHOTO_BG_DEL = ituSceneFindWidget(&theScene, "PHOTO_BG_DEL");
		assert(PHOTO_BG_DEL);
		PHOTO_HEAD = ituSceneFindWidget(&theScene, "PHOTO_HEAD");
		assert(PHOTO_HEAD);
		PHOTO_BTN_PREV = ituSceneFindWidget(&theScene, "PHOTO_BTN_PREV");
		assert(PHOTO_BTN_PREV);
		PHOTO_BTN_NEXT = ituSceneFindWidget(&theScene, "PHOTO_BTN_NEXT");
		assert(PHOTO_BTN_NEXT);
		PHOTO_BG_FULL_1 = ituSceneFindWidget(&theScene, "PHOTO_BG_FULL_1");
		assert(PHOTO_BG_FULL_1);

		VIDEO_BG_DEL = ituSceneFindWidget(&theScene, "VIDEO_BG_DEL");
		assert(VIDEO_BG_DEL);
		VIDEO_HEAD = ituSceneFindWidget(&theScene, "VIDEO_HEAD");
		assert(VIDEO_HEAD);
		VIDEO_BTN_BACK = ituSceneFindWidget(&theScene, "VIDEO_BTN_BACK");
		assert(VIDEO_BTN_BACK);
		VIDEO_BTN_PREV = ituSceneFindWidget(&theScene, "VIDEO_BTN_PREV");
		assert(VIDEO_BTN_PREV);
		VIDEO_BTN_NEXT = ituSceneFindWidget(&theScene, "VIDEO_BTN_NEXT");
		assert(VIDEO_BTN_NEXT);
		VIDEO_BG_BAR = ituSceneFindWidget(&theScene, "VIDEO_BG_BAR");
		assert(VIDEO_BG_BAR);
		VIDEO_BG_FULL_BAR = ituSceneFindWidget(&theScene, "VIDEO_BG_FULL_BAR");
		assert(VIDEO_BG_FULL_BAR);

		VIDEO_BG_NO_FULL = ituSceneFindWidget(&theScene, "VIDEO_BG_NO_FULL");
		assert(VIDEO_BG_NO_FULL);
		VIDEO_BG_NO_FULL_ = ituSceneFindWidget(&theScene, "VIDEO_BG_NO_FULL_");
		assert(VIDEO_BG_NO_FULL_);
		VIDEO_BTN_SLIDER = ituSceneFindWidget(&theScene, "VIDEO_BTN_SLIDER");
		assert(VIDEO_BTN_SLIDER);
		VIDEO_BTN_FULL_SLIDER = ituSceneFindWidget(&theScene, "VIDEO_BTN_FULL_SLIDER");
		assert(VIDEO_BTN_FULL_SLIDER);
	}
	
	ituWidgetSetVisible(MEDIA_ICON_PHOTO_WIN, false);
	imagePlayerLoading = imagePlayerLoaded = false;
	if (imagePlayerDataSize > 0)
	{
		free(imagePlayerData);
		imagePlayerData = NULL;
		imagePlayerDataSize = 0;
	}
	ituWidgetDisable(VIDEO_BTN_SLIDER);
	ituWidgetDisable(VIDEO_BTN_FULL_SLIDER);
	media_itu_init();
	//image_memo_init();
	//usleep(300*1000);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_3);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_3);

	ituWidgetGetGlobalPosition(MEDIA_ICON_VIDEO_WIN, &x, &y);
	width = ituWidgetGetWidth(MEDIA_ICON_VIDEO_WIN);
	height = ituWidgetGetHeight(MEDIA_ICON_VIDEO_WIN);
	itv_set_video_window(x, y, width, height);

	mtal_pb_init(EventHandler);
	return true;
}

void  video_play_total_time()
{
	mtal_pb_get_total_duration_ext(&video_total_time,mtal_spec.srcname);
	printf("total time --------->%d\n",video_total_time);
}

void video_play_update_time()
{
	int s = 0;
	char temp_str[20];
	long temp_sec_time = 0;
	struct timeval tv;
	struct tm *tm,temp_time;
	char time_buf[10] = "\0";

	if((mtal_pb_get_current_time(&s) == 0 )&& (s != video_prev_time))
	{
		video_prev_time = s;
		temp_sec_time = video_total_time - s;
		time_buf[0] = '0';
		time_buf[1] = temp_sec_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (temp_sec_time%60)/10+'0';
		time_buf[4] = (temp_sec_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		ituTrackBarSetValue(VIDEO_TR_BAR,s*952/video_total_time);
		ituProgressBarSetValue(VIDEO_PR_BAR,s*952/video_total_time);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,s*952/video_total_time);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,s*952/video_total_time);
	}
}

bool media_timer(ITUWidget* widget, char* param)
{
	if (imagePlayerLoading && !imagePlayerLoaded)
	{
		int ret = PhotoLoad(imagePlayerPath, imagePlayerLoadCallback);
		if (ret == 0)
			imagePlayerLoading = false;
	}
	else if (imagePlayerLoaded)
	{
		ituIconLoadJpegData(MEDIA_ICON_PHOTO_WIN, imagePlayerData, imagePlayerDataSize);
		ituWidgetSetVisible(MEDIA_ICON_PHOTO_WIN, true);
		free(imagePlayerData);
		imagePlayerData = NULL;
		imagePlayerDataSize = 0;
		imagePlayerLoaded = false;
	}

	if (videoPlayerIsPlaying)
	{
		video_play_update_time();
	}

	if (sd_state_change)
	{
		sd_state_change = false;
		ITULayer* PAGE_HOME = ituSceneFindWidget(&theScene, "PAGE_HOME");
		assert(PAGE_HOME);
		ituLayerGoto(PAGE_HOME);
	}

	return true;
}

bool media_leave(ITUWidget* widget, char* param)
{
	if(!videoPlayerIsFileEOF || ituWidgetIsVisible(VIDEO_BG))
	{
		//AudioStop();
		mtal_pb_stop();
		videoPlayerIsPlaying = false;
		//mtal_pb_exit();
		SceneLeaveVideoState();
		AudioResumeKeySound();
		videoPlayerIsFileEOF = true;
	}
	clear_avi_playing();
	mtal_pb_exit();
	return true;
}

bool view_photo(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollText* item;
	char* file_name = NULL;
	char* file_title = NULL;
	char* file_time = NULL;
	            
	image_cur_page = PHOTO_L_SLISTBOX_0->listbox.pageIndex;
	image_cur_index = PHOTO_L_SLISTBOX_0->listbox.focusIndex;
	//printf("------------>page %d     index %d\n",image_cur_page,image_cur_index);
	item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
	if(item)
	{
		imagePlayerNumber= (int)ituWidgetGetCustomData(item);
		file_name = ImageMemoStartPlay(imagePlayerNumber);
		printf("------>%s<-------\n",file_name);
		if(file_name)
		{
			strcpy(imagePlayerPath,file_name);
			imagePlayerLoading = true;
			ituWidgetSetVisible(MEDIA_ICON_PHOTO_WIN, false);
			ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
			ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
		}
	}
	return true;
}

bool prev_photo(ITUWidget* widget, char* param)
{
	char* file_name = NULL;
	imagePlayerNumber--;
	if(imagePlayerNumber < 0)
		imagePlayerNumber = ImageMemoGetCount() - 1;
	ituListBoxPrevImpl(PHOTO_L_SLISTBOX_0);
	ituListBoxPrevImpl(PHOTO_L_SLISTBOX_1);
	ituListBoxPrevImpl(PHOTO_L_SLISTBOX_2);
	ituListBoxPrevImpl(PHOTO_L_SLISTBOX_3);
	image_cur_page = PHOTO_L_SLISTBOX_0->listbox.pageIndex;
	image_cur_index = PHOTO_L_SLISTBOX_0->listbox.focusIndex;
//	printf("------------>page %d     index %d\n",image_cur_page,image_cur_index);
	file_name = ImageMemoStartPlay(imagePlayerNumber);
	if(file_name)
	{
		//printf("---------->%s\n",file_name);
		strcpy(imagePlayerPath,file_name);
		imagePlayerLoading = true;
		ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
		ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
	}
	return true;
}

bool next_photo(ITUWidget* widget, char* param)
{
	char* file_name = NULL;

	imagePlayerNumber++;
	if(imagePlayerNumber > (ImageMemoGetCount() - 1))
		imagePlayerNumber = 0;
	ituListBoxNextImpl(PHOTO_L_SLISTBOX_0);
	ituListBoxNextImpl(PHOTO_L_SLISTBOX_1);
	ituListBoxNextImpl(PHOTO_L_SLISTBOX_2);
	ituListBoxNextImpl(PHOTO_L_SLISTBOX_3);
	image_cur_page = PHOTO_L_SLISTBOX_0->listbox.pageIndex;
	image_cur_index = PHOTO_L_SLISTBOX_0->listbox.focusIndex;
//	printf("------------>page %d     index %d\n",image_cur_page,image_cur_index);
	file_name = ImageMemoStartPlay(imagePlayerNumber);
	if(file_name)
	{
		//	printf("---------->%s\n",file_name);
		strcpy(imagePlayerPath,file_name);
		imagePlayerLoading = true;
		ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
		ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
	}
	return true;
}


bool delete_photo(ITUWidget* widget, char* param)
{
	ImageMemoDeleteEntry(imagePlayerNumber);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_3);
	//set_Image_new_file();
	char* file_name = NULL;
	file_name = ImageMemoStartPlay(imagePlayerNumber);
	if(file_name)
	{
		int i;
		for(i = 0;i < image_cur_page;i++)
		{
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_0->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_1->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_2->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_3->listbox));
			usleep(1000);
		}
		ituListBoxCheck(PHOTO_L_SLISTBOX_0,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_1,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_2,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_3,image_cur_index);
		strcpy(imagePlayerPath,file_name);
		imagePlayerLoading = true;
		ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
		ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
	}
	else
	{
		if(ImageMemoGetCount())
		{
		#if 0
			image_cur_page = 1;
			image_cur_index = 0;
			imagePlayerNumber = 0;
			file_name = ImageMemoStartPlay(imagePlayerNumber);
			if(file_name)
			{
				strcpy(imagePlayerPath,file_name);
				imagePlayerLoading = true;
				ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
				ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
			}
		#else
			ituWidgetSetVisible(PHOTO_L_BG,true);
			ituWidgetSetVisible(PHOTO_BG,false);
		#endif
		}
		else
		{
			ituWidgetSetVisible(PHOTO_L_BG,true);
			ituWidgetSetVisible(PHOTO_BG,false);
			ITULayer* PAGE_HOME = ituSceneFindWidget(&theScene, "PAGE_HOME");
			assert(PAGE_HOME);
			ituLayerGoto(PAGE_HOME);
		}
	}
	return true;
}

bool photo_back_list(ITUWidget* widget, char* param)
{
	//if(get_Image_new_file())
	{
		int i;
		clear_Image_new_file();
		ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_0);
		ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_1);
		ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_2);
		ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_3);
		usleep(10*1000);
		for(i = 0;i < image_cur_page;i++)
		{
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_0->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_1->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_2->listbox));
			ListBoxNextPage(&(PHOTO_L_SLISTBOX_3->listbox));
			usleep(1000);
		}
		ituListBoxCheck(PHOTO_L_SLISTBOX_0,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_1,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_2,image_cur_index);
		ituListBoxCheck(PHOTO_L_SLISTBOX_3,image_cur_index);
	}
	return true;
}
void delete_1_sec_video(int *totaltime, char *filepath)
{
	mtal_pb_get_total_duration_ext(totaltime,filepath);
	if((*totaltime) == 0)
	{
		VideoMemoDeleteEntry(VideoPlayerNumber);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_0);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_1);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_2);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_3);
		if(VideoPlayerNumber == VideoMemoGetCount())
			VideoPlayerNumber = 0;
		delete_1_sec_video(totaltime,VideoMemoStartPlay(VideoPlayerNumber));
	}
}

void _video_play(char* file_name)
{
	strcpy(mtal_spec.srcname, file_name);
	mtal_spec.vol_level = 99;
	//AudioStop();
	//mtal_pb_stop();
	mtal_pb_select_file(&mtal_spec);
	user_amp_off();
	mtal_pb_play();
}

bool view_video(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollText* item;
	char* file_name = NULL;
	char* file_title = NULL;
	char* file_time = NULL;
	char file_buf[50] = "\0";
	char time_buf[10] = "\0";

	//mtal_pb_init(EventHandler);
	SceneEnterVideoState();
	AudioPauseKeySound();
	
	video_cur_page = VIDEO_L_SLISTBOX_0->listbox.pageIndex;
	video_cur_index = VIDEO_L_SLISTBOX_0->listbox.focusIndex;
	
	ituWidgetDisable(VIDEO_BTN_BACK);
	//ituWidgetDisable(VIDEO_BTN_PREV);
	//ituWidgetDisable(VIDEO_BTN_NEXT);
	ituWidgetSetVisible(VIDEO_BTN_PREV, false);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
	ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
	ituWidgetSetVisible(MEDIA_BG_VIDEO_BLACK, true);
	
	item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
	if(item)
	{
		VideoPlayerNumber= (int)ituWidgetGetCustomData(item);
		file_name = VideoMemoStartPlay(VideoPlayerNumber);
		printf("%d----file_name--->%s\n",VideoPlayerNumber,file_name);
		if(file_name)
		{
			strcpy(file_buf,file_name);
			delete_1_sec_video(&video_total_time,file_buf);
			videoPlayerIsFileEOF = false;
			_video_play(file_name);
		//	video_first_start();
			file_name = VideoMemoStartPlay(VideoPlayerNumber);
			time_buf[0] = '0';
			time_buf[1] = video_total_time/60+'0';
			time_buf[2] = ':';
			time_buf[3] = (video_total_time%60)/10+'0';
			time_buf[4] = (video_total_time%60)%10+'0';
			ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
			ituTextSetString(VIDEO_TEXT_FILE_NO, VideoMemoTitel(VideoPlayerNumber));
			ituTextSetString(VIDEO_TEXT_FILE_NAME, VideoMemoTime(VideoPlayerNumber));
			//ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
			ituWidgetSetVisible(VIDEO_BTN_STOP, false);
			ituTrackBarSetValue(VIDEO_TR_BAR,0);
			ituProgressBarSetValue(VIDEO_PR_BAR,0);
			ituTrackBarSetValue(VIDEO_TR_FULL_BAR,0);
			ituProgressBarSetValue(VIDEO_PR_FULL_BAR,0);
		}
	}
	return true;
}

bool play_video(ITUWidget* widget, char* param)
{
	if(videoPlayerIsFileEOF)
	{
		char* file_name = NULL;
		file_name = VideoMemoStartPlay(VideoPlayerNumber);
		printf("%d------>%s<-------\n",VideoPlayerNumber,file_name);
		if(file_name)
		{
			//AudioStop();
			mtal_pb_stop();
			mtal_pb_exit();
			mtal_pb_init(EventHandler);
			strcpy(mtal_spec.srcname, file_name);
			mtal_spec.vol_level = 99;
			mtal_pb_select_file(&mtal_spec);
			mtal_pb_play();
			videoPlayerIsFirst = true;
		}
	}
	else if(videoPlayerIsPause)
	{
		mtal_pb_play();
	}
	user_amp_on();
	set_avi_playing();
	videoPlayerIsPlaying = true;
	videoPlayerIsFileEOF = false;
	videoPlayerIsPause = false;
	return true;
}

bool stop_video(ITUWidget* widget, char* param)
{
	clear_avi_playing();
	mtal_pb_pause();
	videoPlayerIsPause = true;
	videoPlayerIsPlaying = false;
	return true;
}

bool prev_video(ITUWidget* widget, char* param)
{
	char* file_name = NULL;
	char time_buf[10] = "\0";
	char file_buf[50] = "\0";
	VideoPlayerNumber--;
	if(VideoPlayerNumber < 0)
		VideoPlayerNumber = VideoMemoGetCount() - 1;
	ituListBoxPrevImpl(VIDEO_L_SLISTBOX_0);
	ituListBoxPrevImpl(VIDEO_L_SLISTBOX_1);
	ituListBoxPrevImpl(VIDEO_L_SLISTBOX_2);
	ituListBoxPrevImpl(VIDEO_L_SLISTBOX_3);
	video_cur_page = VIDEO_L_SLISTBOX_0->listbox.pageIndex;
	video_cur_index = VIDEO_L_SLISTBOX_0->listbox.focusIndex;
	ituWidgetDisable(VIDEO_BTN_BACK);
	//ituWidgetDisable(VIDEO_BTN_PREV);
	//ituWidgetDisable(VIDEO_BTN_NEXT);
	ituWidgetSetVisible(VIDEO_BTN_PREV, false);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
	ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
	ituWidgetSetVisible(MEDIA_BG_VIDEO_BLACK, true);
	//AudioStop();
	mtal_pb_stop();
	mtal_pb_exit();
	mtal_pb_init(EventHandler);
	videoPlayerIsPlaying = false;
	videoPlayerIsFileEOF = true;
	videoPlayerIsPause = true;
	file_name = VideoMemoStartPlay(VideoPlayerNumber);
	if(file_name)
	{
		strcpy(file_buf,file_name);
		//mtal_pb_get_total_duration_ext(&video_total_time,file_buf);
		delete_1_sec_video(&video_total_time,file_buf);
		videoPlayerIsFileEOF = false;
		_video_play(file_name);
		//video_first_start();
		file_name = VideoMemoStartPlay(VideoPlayerNumber);
		time_buf[0] = '0';
		time_buf[1] = video_total_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (video_total_time%60)/10+'0';
		time_buf[4] = (video_total_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		ituTextSetString(VIDEO_TEXT_FILE_NO, VideoMemoTitel(VideoPlayerNumber));
		ituTextSetString(VIDEO_TEXT_FILE_NAME, VideoMemoTime(VideoPlayerNumber));
		ituTrackBarSetValue(VIDEO_TR_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_BAR,0);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,0);
		//ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
		ituWidgetSetVisible(VIDEO_BTN_STOP, false);
	}
	return true;
}

bool next_video(ITUWidget* widget, char* param)
{
	char* file_name = NULL;
	char time_buf[10] = "\0";
	char file_buf[50] = "\0";
	VideoPlayerNumber++;
	if(VideoPlayerNumber > (VideoMemoGetCount() - 1))
		VideoPlayerNumber = 0;
	ituListBoxNextImpl(VIDEO_L_SLISTBOX_0);
	ituListBoxNextImpl(VIDEO_L_SLISTBOX_1);
	ituListBoxNextImpl(VIDEO_L_SLISTBOX_2);
	ituListBoxNextImpl(VIDEO_L_SLISTBOX_3);
	video_cur_page = VIDEO_L_SLISTBOX_0->listbox.pageIndex;
	video_cur_index = VIDEO_L_SLISTBOX_0->listbox.focusIndex;
	ituWidgetDisable(VIDEO_BTN_BACK);
	//ituWidgetDisable(VIDEO_BTN_PREV);
	//ituWidgetDisable(VIDEO_BTN_NEXT);
	ituWidgetSetVisible(VIDEO_BTN_PREV, false);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
	ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
	ituWidgetSetVisible(MEDIA_BG_VIDEO_BLACK, true);
	//AudioStop();
	mtal_pb_stop();
	mtal_pb_exit();
	mtal_pb_init(EventHandler);
	videoPlayerIsPlaying = false;
	videoPlayerIsFileEOF = true;
	videoPlayerIsPause = true;
	file_name = VideoMemoStartPlay(VideoPlayerNumber);
	if(file_name)
	{
		strcpy(file_buf,file_name);
		//mtal_pb_get_total_duration_ext(&video_total_time,file_buf);
		delete_1_sec_video(&video_total_time,file_buf);
			videoPlayerIsFileEOF = false;
			_video_play(file_name);
		//video_first_start();
		file_name = VideoMemoStartPlay(VideoPlayerNumber);
		time_buf[0] = '0';
		time_buf[1] = video_total_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (video_total_time%60)/10+'0';
		time_buf[4] = (video_total_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		usleep(10);
		ituTextSetString(VIDEO_TEXT_FILE_NO, VideoMemoTitel(VideoPlayerNumber));
		ituTextSetString(VIDEO_TEXT_FILE_NAME, VideoMemoTime(VideoPlayerNumber));
		ituTrackBarSetValue(VIDEO_TR_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_BAR,0);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,0);
		//ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
		ituWidgetSetVisible(VIDEO_BTN_STOP, false);
	}
	return true;
}

bool video_delete_pop(ITUWidget* widget, char* param)
{
	if(videoPlayerIsPlaying)
	{
		clear_avi_playing();
		mtal_pb_pause();
		videoPlayerIsPause = true;
		videoPlayerIsPlaying = false;
	}
	return true;
}

bool video_back_list(ITUWidget* widget, char* param);

bool delete_video(ITUWidget* widget, char* param)
{
	char* file_name = NULL;
	char time_buf[10] = "\0";
	char file_buf[50] = "\0";
	
	ituWidgetSetVisible(MEDIA_BG_VIDEO_BLACK, true);
	ituWidgetSetVisible(VIDEO_BTN_PREV, false);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
	ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
	//if(videoPlayerIsPause)
	{
		//AudioStop();
		mtal_pb_stop();
		mtal_pb_exit();
		mtal_pb_init(EventHandler);
		videoPlayerIsPlaying = false;
		videoPlayerIsFileEOF = true;
		videoPlayerIsPause = false;
	}
	VideoMemoDeleteEntry(VideoPlayerNumber);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_3);
	//set_Video_new_file();
	file_name = VideoMemoStartPlay(VideoPlayerNumber);
	if(file_name)
	{
		int i;
		for(i = 0;i < video_cur_page;i++)
		{
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_0->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_1->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_2->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_3->listbox));
			usleep(1000);
		}
		ituListBoxCheck(VIDEO_L_SLISTBOX_0,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_1,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_2,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_3,video_cur_index);
		strcpy(file_buf,file_name);
		//printf("file name-1--->%s\n",file_buf);
		videoPlayerIsFileEOF = false;
		_video_play(file_name);
		video_first_start();
		mtal_pb_get_total_duration_ext(&video_total_time,file_buf);
		time_buf[0] = '0';
		time_buf[1] = video_total_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (video_total_time%60)/10+'0';
		time_buf[4] = (video_total_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		ituTextSetString(VIDEO_TEXT_FILE_NO, VideoMemoTitel(VideoPlayerNumber));
		ituTextSetString(VIDEO_TEXT_FILE_NAME, VideoMemoTime(VideoPlayerNumber));
		ituTrackBarSetValue(VIDEO_TR_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_BAR,0);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,0);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,0);
	}
	else
	{
		if(VideoMemoGetCount())
		{
		#if 0
			VideoPlayerNumber = 0;
			file_name = VideoMemoStartPlay(VideoPlayerNumber);
			if(file_name)
			{
				strcpy(file_buf,file_name);
				printf("file name-2--->%s\n",file_buf);
				mtal_pb_get_total_duration_ext(&video_total_time,file_buf);
				time_buf[0] = '0';
				time_buf[1] = video_total_time/60+'0';
				time_buf[2] = ':';
				time_buf[3] = (video_total_time%60)/10+'0';
				time_buf[4] = (video_total_time%60)%10+'0';
				ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
				ituTextSetString(VIDEO_TEXT_FILE_NO, VideoMemoTitel(imagePlayerNumber));
				ituTextSetString(VIDEO_TEXT_FILE_NAME, VideoMemoTime(imagePlayerNumber));
				ituTrackBarSetValue(VIDEO_TR_BAR,0);
				ituProgressBarSetValue(VIDEO_PR_BAR,0);
			}
		#else
			ituWidgetSetVisible(VIDEO_L_BG,true);
			ituWidgetSetVisible(VIDEO_BG,false);
			video_back_list(widget,param);
		#endif
		}
		else
		{
			ituWidgetSetVisible(VIDEO_L_BG,true);
			ituWidgetSetVisible(VIDEO_BG,false);
			video_back_list(widget,param);
			ITULayer* PAGE_HOME = ituSceneFindWidget(&theScene, "PAGE_HOME");
			assert(PAGE_HOME);
			ituLayerGoto(PAGE_HOME);
		}
	}
	return true;
}

bool video_jump(ITUWidget* widget, char* param)
{
	return true;
}

bool video_bar_jump(ITUWidget* widget, char* param)
{
	//printf("video_bar_jump.......................111111\n");
	return true;
}

bool video_btn_jump(ITUWidget* widget, char* param)
{
	printf("video_btn_jump.......................222222\n");
	int s = 0;
	long temp_sec_time = 0;
	char time_buf[10] = "\0";
	
	if(videoPlayerIsPlaying)
	{
		s = VIDEO_TR_BAR->value * video_total_time /952;
		mtal_pb_seekto(s);

		temp_sec_time = video_total_time - s;
		time_buf[0] = '0';
		time_buf[1] = temp_sec_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (temp_sec_time%60)/10+'0';
		time_buf[4] = (temp_sec_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		ituTrackBarSetValue(VIDEO_TR_FULL_BAR,VIDEO_TR_BAR->value);
		ituProgressBarSetValue(VIDEO_PR_FULL_BAR,VIDEO_TR_BAR->value);
	}
	return true;
}

bool video_btn_jump_full(ITUWidget* widget, char* param)
{
	printf("video_btn_jump.......................222222\n");
	int s = 0;
	long temp_sec_time = 0;
	char time_buf[10] = "\0";
	
	if(videoPlayerIsPlaying)
	{
		s = VIDEO_TR_FULL_BAR->value * video_total_time /952;
		mtal_pb_seekto(s);

		temp_sec_time = video_total_time - s;
		time_buf[0] = '0';
		time_buf[1] = temp_sec_time/60+'0';
		time_buf[2] = ':';
		time_buf[3] = (temp_sec_time%60)/10+'0';
		time_buf[4] = (temp_sec_time%60)%10+'0';
		ituTextSetString(VIDEO_TEXT_FILE_TIME, time_buf);
		ituTrackBarSetValue(VIDEO_TR_BAR,VIDEO_TR_FULL_BAR->value);
		ituProgressBarSetValue(VIDEO_PR_BAR,VIDEO_TR_FULL_BAR->value);
	}
	return true;
}

bool media_change(ITUWidget* widget, char* param)
{
	printf("media_change.......................\n");
	if(videoPlayerIsPlaying)
	{
		mtal_pb_seekto(VIDEO_TR_BAR->value * video_total_time /952);
	}
	return true;
}

bool video_back_list(ITUWidget* widget, char* param)
{
	//AudioStop();
	mtal_pb_stop();
	videoPlayerIsPlaying = false;
	clear_avi_playing();
	mtal_pb_exit();
	mtal_pb_init(EventHandler);
	SceneLeaveVideoState();
	AudioResumeKeySound();
	videoPlayerIsFileEOF = true;
		printf("video_back_list.............111111111111111111\n");
	//if(get_Video_new_file())
	{
		int i;
		printf("video_back_list.............222222222222222222\n");
		clear_Video_new_file();
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_0);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_1);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_2);
		ituListBoxReload((ITUListBox*)VIDEO_L_SLISTBOX_3);
		usleep(10*1000);
		for(i = 0;i < video_cur_page;i++)
		{
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_0->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_1->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_2->listbox));
			ListBoxNextPage(&(VIDEO_L_SLISTBOX_3->listbox));
			usleep(1000);
		}
		ituListBoxCheck(VIDEO_L_SLISTBOX_0,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_1,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_2,video_cur_index);
		ituListBoxCheck(VIDEO_L_SLISTBOX_3,video_cur_index);
	}
	return true;
}

bool media_close_full(ITUWidget* widget, char* param)
{
	ituWidgetSetVisible(VIDEO_BG_NO_FULL_, true);
	ituWidgetSetVisible(VIDEO_BG_NO_FULL_, true);
	if(videoPlayerIsPlaying)
	{
		ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
		ituWidgetSetVisible(VIDEO_BTN_STOP, true);
	}
	else
	{
		ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
		ituWidgetSetVisible(VIDEO_BTN_STOP, false);
	}
	ituWidgetSetVisible(VIDEO_BG_BAR, true);
	ituWidgetSetVisible(VIDEO_BG_FULL_BAR, false);

	if(video_first)
	{
		ituWidgetSetVisible(VIDEO_BTN_PREV, false);
		ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
		ituWidgetSetVisible(VIDEO_BTN_PLAY, false);
	}
	
	return true;
}
void media_full()
{
	if(videoPlayerIsPlaying)
		ituWidgetSetVisible(VIDEO_BTN_STOP, false);
			
	ituWidgetSetVisible(VIDEO_HEAD, false);
	ituWidgetSetVisible(VIDEO_BTN_PREV, false);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, false);
	ituWidgetSetVisible(VIDEO_BG_BAR, false);
	ituWidgetSetVisible(VIDEO_BG_FULL_BAR, true);
	ituWidgetSetVisible(VIDEO_BG_NO_FULL_, false);
	ituWidgetSetVisible(VIDEO_BG_NO_FULL, true);
	return true;
}

//2018.3.28 my.wei add for UI command queue
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
void media_event_handler_ui(void)
{
	printf("media_event_handler_ui\r\n");
	ituWidgetSetVisible(VIDEO_BTN_PLAY, true);
	ituWidgetSetVisible(VIDEO_BTN_STOP, false);
	ituTrackBarSetValue(VIDEO_TR_BAR,952);
	ituProgressBarSetValue(VIDEO_PR_BAR,952);
	ituTrackBarSetValue(VIDEO_TR_FULL_BAR,952);
	ituProgressBarSetValue(VIDEO_PR_FULL_BAR,952);
	ituTextSetString(VIDEO_TEXT_FILE_TIME, "00:00");
	ituWidgetSetVisible(VIDEO_HEAD, true);
	ituWidgetSetVisible(VIDEO_BTN_BACK, true);
	ituWidgetSetVisible(VIDEO_BTN_PREV, true);
	ituWidgetSetVisible(VIDEO_BTN_NEXT, true);
	ituWidgetSetVisible(VIDEO_BG_BAR, true);
	ituWidgetSetVisible(VIDEO_BG_FULL_BAR, false);
	ituWidgetSetVisible(VIDEO_BG_NO_FULL_, true);
	ituWidgetSetVisible(VIDEO_BG_NO_FULL, false);

	clear_avi_playing();
	mtal_pb_pause();
	//videoPlayerIsPause = true;    
}
#endif
