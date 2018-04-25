#include <sys/stat.h>
#include <sys/statvfs.h>
#include <assert.h>
#include <dirent.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "ite/itp.h"
#include "redblack/redblack.h"
#include "user_function.h"
#include <fat/Api_f.h>

static char channel_str[4][10]={"Door1","Door2","CCTV1","CCTV2"};

#define MAX_IMAGEMEMO_COUNT    128

#define IMAGEMEMO_PATH  "E:/photo"

#define IMAGEMEMO_PATH_F  "C:/photo"

#define BACKUP_PATH  "E:/backup"

#define IMAGEMEMO_EXT  ".jpg"

static ImageMemoEntry ImageMemoArray[MAX_IMAGEMEMO_COUNT];

static bool ImageMemoInited, ImageMemoQuit, ImageMemoRecording;
static int ImageMemoIndex, ImageMemoCount, ImageMemoUnplayCount;

char Image_Snap_Path[40];
char Image_Play_Path[40];
char Image_Snap_Path[40];
char Image_Play_Title[16];
char Image_Play_Time[20];

bool Image_new_file = false;

bool get_Image_new_file()
{
	return Image_new_file;
}

void clear_Image_new_file()
{
	Image_new_file = false;
}

void set_Image_new_file()
{
	Image_new_file = true;
}

static int ImageMemoCompare(const void *pa, const void *pb, const void *config)
{
	struct dirent* dira = (struct dirent*) pa;
	struct dirent* dirb = (struct dirent*) pb;
	assert(pa);
	assert(pb);

	if ((dira->d_type == DT_DIR) && (dirb->d_type != DT_DIR))
		return -1;

	if ((dira->d_type != DT_DIR) && (dirb->d_type == DT_DIR))
		return 1;

	return strcmp(dira->d_name, dirb->d_name);
}

void image_copy_sd(void)
{
	DIR *pdir = NULL;
	struct dirent* val;
	void* rbTree;
	char filename[50] = "\0";
	char newname[50] = "\0";

	
	if(opendir(BACKUP_PATH) == NULL)
		mkdir(BACKUP_PATH, S_IRWXU);
	if(opendir(BACKUP_PATH) == NULL)
		return;
	mkdir(IMAGEMEMO_PATH_F, S_IRWXU);
	
	pdir = opendir(IMAGEMEMO_PATH_F);
	if (pdir)
	{
		struct dirent *pent;

		// Make this our current directory
			chdir(IMAGEMEMO_PATH_F);

		// List the contents of the directory
		while ((pent = readdir(pdir)) != NULL)
		{
			struct dirent *dir, *ret;
			char* pch;

			if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
				continue;

			pch = strrchr(pent->d_name, '.');
			if (!pch)
				continue;

			if (strcmp(pch, IMAGEMEMO_EXT) != 0)
				continue;
			//printf("#######--------->%s\n",pent->d_name);
			strcpy(filename, IMAGEMEMO_PATH_F);
			strcat(filename, "/");
			strcat(filename, pent->d_name);
			strcpy(newname, BACKUP_PATH);
			strcat(newname, "/");
			strcat(newname, pent->d_name);
			//printf("#######old-->%s\n#######new-->%s\n",filename,newname);
			ugCopyFile(newname, filename);
		}
	}
	
}

static void* _image_memo_init(void* arg)
{
	DIR *pdir = NULL;
	struct dirent* val;
	void* rbTree;
	test_tick = SDL_GetTicks();
	if(StorageGetCurrType() == STORAGE_SD)
		mkdir(IMAGEMEMO_PATH, S_IRWXU);
	else
		mkdir(IMAGEMEMO_PATH_F, S_IRWXU);

	ImageMemoCount = ImageMemoUnplayCount = 0;
	rbTree = rbinit(ImageMemoCompare, NULL);
	if (rbTree == NULL)
	{
		puts("insufficient memory");
		return;
	}

	if(StorageGetCurrType() == STORAGE_SD)
		pdir = opendir(IMAGEMEMO_PATH);
	else
		pdir = opendir(IMAGEMEMO_PATH_F);
	if (pdir)
	{
		struct dirent *pent;

		// Make this our current directory
		if(StorageGetCurrType() == STORAGE_SD)
			chdir(IMAGEMEMO_PATH);
		else
			chdir(IMAGEMEMO_PATH_F);

		// List the contents of the directory
		while ((pent = readdir(pdir)) != NULL)
		{
			struct dirent *dir, *ret;
			char* pch;

			if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
				continue;

			pch = strrchr(pent->d_name, '.');
			if (!pch)
				continue;

			if (strcmp(pch, IMAGEMEMO_EXT) != 0)
				continue;

			dir = malloc(sizeof (struct dirent));
			if (dir == NULL)
			{
				printf("out of memory: %d\n", sizeof(struct dirent));
				return;
			}

			memcpy(dir, pent, sizeof (struct dirent));

			ret = (struct dirent*) rbsearch((void *)dir, rbTree);
			if(ret == NULL)
			{
				printf("out of memory: %d\n", sizeof(struct dirent));
				free(dir);
				return;
			}
			
		}
	}
	
	for (val = (struct dirent*) rblookup(RB_LULAST, NULL, rbTree);
	        val != NULL;
	        val = (struct dirent*)rblookup(RB_LUPREV, val, rbTree))
	{
		char buf[128],*pch;
		struct stat st;
		ImageMemoEntry* entry = &ImageMemoArray[ImageMemoCount];
		strncpy(entry->name, val->d_name,19);
		if(val->d_name[20] == 'D')
			entry->cam= val->d_name[21] - '0';
		else if(val->d_name[20] == 'C')
			entry->cam= val->d_name[21] - '0'+2;
		entry->type= val->d_name[22] - '0';
		entry->played= val->d_name[23] - '0';
		//printf("------------>played  = %d\n",entry->played);
		if (!entry->played)
			ImageMemoUnplayCount++;
		ImageMemoCount++;
		printf("Image---->%s\n",val->d_name);
	}
	printf("total image file ---->%d\n",ImageMemoCount);
	//printf("_image_memo_init----------11111--------------->%d\n",SDL_GetTicks()-test_tick);
}


ImageMemoEntry* ImageMemoGetEntry(int index)
{
	printf("ImageMemoGetEntry...............%d\n",index);
	if(index >= 0 && index < ImageMemoCount)
		return &ImageMemoArray[index];
	else
		return NULL;
}

int ImageMemoGetCount(void)
{
	return ImageMemoCount;
}

int ImageMemoGetUnPlay(void)
{
	return ImageMemoUnplayCount;
}

char* ImageMemoTitel(int index)
{
	char buf[PATH_MAX];
	ImageMemoEntry* entry;
	char temp_str[30] = "\0";

	entry = ImageMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	strcpy(temp_str,channel_str[entry->cam-1]);
	strcat(temp_str," [");
	temp_str[7] = (index+1)/100+'0';
	temp_str[8] = ((index+1)%100)/10+'0';
	temp_str[9] = (index+1)%10+'0';
	strcat(temp_str,"/");
	temp_str[11] = ImageMemoCount/100+'0';
	temp_str[12] = (ImageMemoCount%100)/10+'0';
	temp_str[13] = ImageMemoCount%10+'0';
	strcat(temp_str,"]");
	strcpy(Image_Play_Title,temp_str);
	printf("-----titel---->%s\n",Image_Play_Title);
	return &Image_Play_Title;
}

char* ImageMemoTime(int index)
{
	int i;
	char buf[PATH_MAX];
	ImageMemoEntry* entry;
	char temp_str[30] = "\0";
#if 1
	entry = ImageMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	//memset(Image_Play_Time,'\0',sizeof(Image_Play_Time));
	printf("111-----time---->%s\n",entry->name);
	temp_str[0] 	= entry->name[4];
	temp_str[1] 	= entry->name[5];
	temp_str[2] 	= entry->name[6];
	temp_str[3] 	= entry->name[7];
	temp_str[4] 	= '-';
	temp_str[5] 	= entry->name[8];
	temp_str[6] 	= entry->name[9];
	temp_str[7] 	= '-';
	temp_str[8] 	= entry->name[10];
	temp_str[9] 	= entry->name[11];
	temp_str[10]	= ' ';
	temp_str[11]	= entry->name[13];
	temp_str[12]	= entry->name[14];
	temp_str[13]	= ':';
	temp_str[14]	= entry->name[15];
	temp_str[15] 	= entry->name[16];
	temp_str[16]	= ':';
	temp_str[17]	= entry->name[17];
	temp_str[18] 	= entry->name[18];
	strcpy(Image_Play_Time,temp_str);
	printf("222-----time---->%s\n",Image_Play_Time);
#endif	
	return &Image_Play_Time;
}
static void ImageMemoDeleteFile(int index)
{
	char buf[128] = "\0";
	char temp_str[2];
	ImageMemoEntry* entry = &ImageMemoArray[index];
	//printf("delete file--111->%d   %d\n",entry->played,ImageMemoArray[index].played);

	if(StorageGetCurrType() == STORAGE_SD)
		strcpy(buf, IMAGEMEMO_PATH);
	else
		strcpy(buf, IMAGEMEMO_PATH_F);
		
	strcat(buf, "/");
	strcat(buf, entry->name);
	strcat(buf, "-");
	if(entry->cam == 1 || entry->cam == 2)
	{
		buf[29] = 'D';
		buf[30] = ('0'+entry->cam);
	}
	else if(entry->cam == 3 || entry->cam == 4)
	{
		buf[29] = 'C';
		buf[30] = ('0'+entry->cam-2);
	}
	buf[31] = ('0'+entry->type);
	buf[32] = ('0'+entry->played);
	strcat(buf, IMAGEMEMO_EXT);
	printf("delete file--->%s   %d\n",buf,entry->played);
	remove(buf);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
	ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
}

void ImageMemoDeleteEntry(int index)
{
	if (ImageMemoCount == 0 || index >= ImageMemoCount)
		return;
	ImageMemoDeleteFile(index);
	int photo_max;
	if(StorageGetCurrType() == STORAGE_SD)
		photo_max = MAX_IMAGEMEMO_COUNT;
	else
		photo_max = 50;
	if(index != photo_max -1)
		memmove(&ImageMemoArray[index], &ImageMemoArray[index+1], sizeof(ImageMemoEntry) * (photo_max - 1 - index));
	ImageMemoCount--;
}

char* ImageMemoStartPlay(int index)
{
	char buf[50] = "\0";
	char buf_s[50] = "\0";
	ImageMemoEntry* entry;
	char temp_str[2];
	int i;

	entry = ImageMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	memset(Image_Play_Path,0,sizeof(Image_Play_Path));
	if(StorageGetCurrType() == STORAGE_SD)
		strcpy(buf, IMAGEMEMO_PATH);
	else
		strcpy(buf, IMAGEMEMO_PATH_F);
	strcat(buf, "/");
	strcat(buf, entry->name);
	strcat(buf, "-");
	if(entry->cam == 1 || entry->cam == 2)
	{
		buf[29] = 'D';
		buf[30] = ('0'+entry->cam);
	}
	else if(entry->cam == 3 || entry->cam == 4)
	{
		buf[29] = 'C';
		buf[30] = ('0'+entry->cam-2);
	}
	buf[31] = ('0'+entry->type);
	buf[32] = ('0'+entry->played);
	//printf("play_name------------>%s\n",entry->name);
	//printf("play_name------------>%s\n",buf);
	strcat(buf, IMAGEMEMO_EXT);
	if (!entry->played)
	{
		Image_new_file = true;
		strcpy(buf_s, buf);
		buf_s[32] = '1';
		rename(buf,buf_s);
		CapSave();
		strcpy(Image_Play_Path, buf_s);
		entry->played = 1;
		if (ImageMemoUnplayCount > 0)
			ImageMemoUnplayCount--;
	}
	else
		strcpy(Image_Play_Path, buf);
	//printf("ImageMemoStartPlay---------->%s\n",Image_Play_Path);
	return &Image_Play_Path;
}

char* ImageMemoRecord()
{
	ImageMemoEntry* entry;
	char* pch;
	
	struct timeval tv;
	struct tm* timeinfo;
	char filename[50] = "\0";
	char name_buf[30] = "\0";
	char name_buf_s[30] = "\0";
	int photo_max;


	if (ImageMemoRecording ||!cur_signal)
		return NULL;
	ImageMemoRecording = true;
	
	if(StorageGetCurrType() == STORAGE_SD)
		photo_max = MAX_IMAGEMEMO_COUNT;
	else
		photo_max = 50;
	if(ImageMemoCount >= photo_max)
	{
		ImageMemoDeleteFile(photo_max-1);
		memmove(&ImageMemoArray[1], &ImageMemoArray[0], sizeof(ImageMemoEntry) * (photo_max-1));
	}
	else
		memmove(&ImageMemoArray[1], &ImageMemoArray[0], sizeof(ImageMemoEntry) * (ImageMemoCount));
	entry = &ImageMemoArray[0];
	
	gettimeofday(&tv, NULL);
	timeinfo = localtime((const time_t*)&tv.tv_sec);
	strftime(name_buf, 20, "%Y%m%d_%H%M%S", timeinfo);
	strcpy(name_buf_s, "IMG_");
	strcat(name_buf_s,name_buf);
	if(StorageGetCurrType() == STORAGE_SD)
		strcpy(filename, IMAGEMEMO_PATH);
	else
		strcpy(filename, IMAGEMEMO_PATH_F);
	strcat(filename, "/");
	strcat(filename,name_buf_s);
	strcat(filename,"-");
	if(cur_signal == 1 || cur_signal == 2)
	{
		filename[29] = 'D';
		filename[30] = '0'+cur_signal;
	}
	else
	{
		filename[29] = 'C';
		filename[30] = '0'+cur_signal-2;
	}
	if(auto_snap_filename ||montion_enable)
		filename[31] = '1';
	else
		filename[31] = '0';
	filename[32] = '0';
	strcat(filename,IMAGEMEMO_EXT);
	strcpy(Image_Snap_Path,filename);
	printf("ImageMemoRecord filename --->%s\n",Image_Snap_Path);
	strncpy(entry->name, name_buf_s,19);
	entry->cam= cur_signal;
	if(auto_snap_filename||montion_enable)
		entry->type= 1;
	else
		entry->type= 0;
	entry->played= 0;
	
	ImageMemoCount++;
	if(ImageMemoCount > photo_max )
		ImageMemoCount = photo_max;
	ImageMemoUnplayCount++;
	auto_snap_filename = false;
	//ImageMemoRecording = false;
	return Image_Snap_Path;
}

bool ImageMemoRecGet()
{
	return ImageMemoRecording;
}

void ImageMemoRecEnd()
{
	ImageMemoRecording= false;
}

void image_memo_init()
{
	pthread_t      task;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, _image_memo_init, NULL);
}
