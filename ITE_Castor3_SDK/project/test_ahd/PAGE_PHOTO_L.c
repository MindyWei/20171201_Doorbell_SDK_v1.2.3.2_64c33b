#include <assert.h>
#include "user_function.h"

static char channel_str[4][10] 	= {"Door1","Door2","CCTV1","CCTV2"};
static char type_str[2][5] 	= {"Auto","Hand"};


static ITUIcon* PHOTO_ICON_WIN;
static ITUScrollListBox* PHOTO_L_SLISTBOX_0;
static ITUScrollListBox* PHOTO_L_SLISTBOX_1;
static ITUScrollListBox* PHOTO_L_SLISTBOX_2;
static ITUScrollIconListBox* PHOTO_L_SLISTBOX_3;
static ITUText* PHOTO_TEXT_FILE_NO;
static ITUText* PHOTO_TEXT_FILE_NAME;

char imagePlayerPath[128];
bool imagePlayerLoading, imagePlayerLoaded;
static uint8_t* imagePlayerData;
static int imagePlayerDataSize;
static int imagePlayerNumber;

bool photo_l_slistbox_0_load(ITUWidget* widget, char* param)
{
	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;
	ImageMemoEntry* entry;

	entryCount = ImageMemoGetCount();
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

		entry = ImageMemoGetEntry(j);
		ituTextSetString(scrolltext, entry->name);
		ituWidgetSetCustomData(scrolltext, j);

		ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
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
		{
			listbox->itemCount = i;
		}
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

bool photo_l_slistbox_1_load(ITUWidget* widget, char* param)
{
	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;
	ImageMemoEntry* entry;

	entryCount = ImageMemoGetCount();
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

		entry = ImageMemoGetEntry(j);
		ituTextSetString(scrolltext, channel_str[(entry->cam)]);
		ituWidgetSetCustomData(scrolltext, j);

		ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
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
		{
			listbox->itemCount = i;
		}
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
bool photo_l_slistbox_2_load(ITUWidget* widget, char* param)
{
	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;
	ImageMemoEntry* entry;

	entryCount = ImageMemoGetCount();
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

		entry = ImageMemoGetEntry(j);
		ituTextSetString(scrolltext, type_str[(entry->type)]);
		ituWidgetSetCustomData(scrolltext, j);

		ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
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
		{
			listbox->itemCount = i;
		}
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
bool photo_l_slistbox_3_load(ITUWidget* widget, char* param)
{

	ITCTree* node;
	ITUListBox* listbox = (ITUListBox*)widget;
	ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
	int i, j, count, entryCount;
	ImageMemoEntry* entry;

	entryCount = ImageMemoGetCount();
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

		if (entry->played)
			ituTextSetString(scrolltext, "1");
		else
			ituTextSetString(scrolltext, "0");

		ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
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
		{
			listbox->itemCount = i;
		}
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

bool photo_l_init(ITUWidget* widget, char* param)
{
	if (!PHOTO_ICON_WIN)
	{
		PHOTO_ICON_WIN = ituSceneFindWidget(&theScene, "PHOTO_ICON_WIN");
		assert(PHOTO_ICON_WIN);
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
	}
	ituWidgetSetVisible(PHOTO_ICON_WIN, false);
        imagePlayerLoading = imagePlayerLoaded = false;
        if (imagePlayerDataSize > 0)
        {
            free(imagePlayerData);
            imagePlayerData = NULL;
            imagePlayerDataSize = 0;
        }
		
	image_memo_init();
	
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_3);
	return true;
}

bool photo_l_timer(ITUWidget* widget, char* param)
{
	if (imagePlayerLoading && !imagePlayerLoaded)
	{
		int ret = PhotoLoad(imagePlayerPath, imagePlayerLoadCallback);
		if (ret == 0)
			imagePlayerLoading = false;
	}
	else if (imagePlayerLoaded)
	{
		ituIconLoadJpegData(PHOTO_ICON_WIN, imagePlayerData, imagePlayerDataSize);
		ituWidgetSetVisible(PHOTO_ICON_WIN, true);
		free(imagePlayerData);
		imagePlayerData = NULL;
		imagePlayerDataSize = 0;
		imagePlayerLoaded = false;
	}
	return false;
}

bool photo_l_leave(ITUWidget* widget, char* param)
{
	return true;
}

bool delete_photo(ITUWidget* widget, char* param)
{
	ImageMemoDeleteEntry(imagePlayerNumber);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_0);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_1);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_2);
	ituListBoxReload((ITUListBox*)PHOTO_L_SLISTBOX_3);
	char* file_name = NULL;
	file_name = ImageMemoStartPlay(imagePlayerNumber+1);
	if(file_name)
	{
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
	file_name = ImageMemoStartPlay(imagePlayerNumber+1);
	if(file_name)
	{
		imagePlayerNumber++;
		//	printf("---------->%s\n",file_name);
		strcpy(imagePlayerPath,file_name);
          	imagePlayerLoading = true;
		ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
		ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
	}
	return true;
}

bool prev_photo(ITUWidget* widget, char* param)
{
	char* file_name = NULL;
	file_name = ImageMemoStartPlay(imagePlayerNumber-1);
	if(file_name)
	{
		imagePlayerNumber--;
			//printf("---------->%s\n",file_name);
		strcpy(imagePlayerPath,file_name);
          	imagePlayerLoading = true;
		ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
		ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
	}
	return true;
}

bool view_photo(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)widget;
   	ITUScrollText* item;
	char* file_name = NULL;
	char* file_title = NULL;
	char* file_time = NULL;
	item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
	if(item)
	{
		imagePlayerNumber= (int)ituWidgetGetCustomData(item);
		file_name = ImageMemoStartPlay(imagePlayerNumber);
		//printf("------>%s<-------\n",file_name);
		if(file_name)
		{
			strcpy(imagePlayerPath,file_name);
	          	imagePlayerLoading = true;
				
			ituTextSetString(PHOTO_TEXT_FILE_NO, ImageMemoTitel(imagePlayerNumber));
			ituTextSetString(PHOTO_TEXT_FILE_NAME, ImageMemoTime(imagePlayerNumber));
		}
	}
	return true;
}