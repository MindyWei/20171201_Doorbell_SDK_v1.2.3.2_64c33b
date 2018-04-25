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

static char channel_str[4][10]={"Door1","Door2","CCTV1","CCTV2"};

#define MAX_VIDEOMEMO_COUNT    999

#define VIDEOMEMO_PATH  "E:/video"

#define VIDEOMEMO_EXT  ".avi"

static VideoMemoEntry VideoMemoArray[MAX_VIDEOMEMO_COUNT];

static bool VideoMemoInited, VideoMemoQuit, VideoMemoRecording;
static int VideoMemoIndex, VideoMemoCount, VideoMemoUnplayCount;

char Video_Snap_Path[37];
char Video_Play_Path[37];
char Video_Snap_Path[37];
char Video_Play_Title[16];
char Video_Play_Time[20];

bool Video_new_file = false;

bool get_Video_new_file()
{
	return Video_new_file;
}

void clear_Video_new_file()
{
	Video_new_file = false;
}

void set_Video_new_file()
{
	Video_new_file = true;
}

static int VideoMemoCompare(const void *pa, const void *pb, const void *config)
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

void VideoMemoCheck();

static void* _video_memo_init(void* arg)
{
	DIR *pdir = NULL;
	struct dirent* val;
	void* rbTree;
	test_tick = SDL_GetTicks();
	mkdir(VIDEOMEMO_PATH, S_IRWXU);
	char filename_s[50] = "\0";
	int video_time = 0;
	bool check_last_video_time = true;

	VideoMemoCount = VideoMemoUnplayCount = 0;
	rbTree = rbinit(VideoMemoCompare, NULL);
	if (rbTree == NULL)
	{
		puts("insufficient memory");
		return;
	}

	pdir = opendir(VIDEOMEMO_PATH);
	if (pdir)
	{
		struct dirent *pent;

		// Make this our current directory
		chdir(VIDEOMEMO_PATH);

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

			if (strcmp(pch, VIDEOMEMO_EXT) != 0)
				continue;
			/*
			if(check_last_video_time)
			{
				check_last_video_time = false;
				strcpy(filename_s, VIDEOMEMO_PATH);
				strcat(filename_s, "/");
				strcat(filename_s, pent->d_name);
				mtal_pb_get_total_duration_ext(&video_time,filename_s);
				printf("video  ->%s    ->%d\n",filename_s,video_time);
				if(video_time == 0)
				{
					remove(filename_s);
					continue;
				}
			}
			*/
		//	printf("video---->%s\n",pent->d_name);
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
		VideoMemoEntry* entry = &VideoMemoArray[VideoMemoCount];
			strncpy(entry->name, val->d_name,19);
		if(val->d_name[20] == 'D')
			entry->cam= val->d_name[21] - '0';
		else if(val->d_name[20] == 'C')
			entry->cam= val->d_name[21] - '0'+2;
		entry->type= val->d_name[22] - '0';
		entry->played= val->d_name[23] - '0';
			//printf("------------>played  = %d\n",entry->played);
			if (!entry->played)
				VideoMemoUnplayCount++;
			VideoMemoCount++;
			printf("Video---->%s\n",val->d_name);
	}
	if(VideoMemoCount)
		VideoMemoCheck();
	printf("total video file ---->%d\n",VideoMemoCount);
	//printf("_video_memo_init----------11111--------------->%d\n",SDL_GetTicks()-test_tick);
}

void video_delete_all(void)
{
	DIR *pdir = NULL;
	struct dirent* val;
	void* rbTree;
	char filename[50] = "\0";
	char newname[50] = "\0";
	
	if(opendir(VIDEOMEMO_PATH) == NULL)
		return;
	
	pdir = opendir(VIDEOMEMO_PATH);
	if (pdir)
	{
		struct dirent *pent;

		// Make this our current directory
			chdir(VIDEOMEMO_PATH);

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

			if (strcmp(pch, VIDEOMEMO_EXT) != 0)
				continue;
			//printf("#######--------->%s\n",pent->d_name);
			strcpy(filename, VIDEOMEMO_PATH);
			strcat(filename, "/");
			strcat(filename, pent->d_name);
			//printf("#######old-->%s\n#######new-->%s\n",filename,newname);
			remove(filename);
		}
	}
	
}

VideoMemoEntry* VideoMemoGetEntry(int index)
{
	printf("VideoMemoGetEntry.....index %d  count  %d \n",index,VideoMemoCount);
	if(index >= 0 && index < VideoMemoCount)
		return &VideoMemoArray[index];
	else
		return NULL;
}

int VideoMemoGetCount(void)
{
	return VideoMemoCount;
}

int VideoMemoGetUnPlay(void)
{
	return VideoMemoUnplayCount;
}

char* VideoMemoTitel(int index)
{
	char buf[PATH_MAX];
	VideoMemoEntry* entry;
	char temp_str[30] = "\0";
	
	printf("index----------->%d\n",index);
	entry = VideoMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	strcpy(temp_str,channel_str[entry->cam-1]);
	strcat(temp_str," [");
	temp_str[7] = (index+1)/100+'0';
	temp_str[8] = ((index+1)%100)/10+'0';
	temp_str[9] = (index+1)%10+'0';
	strcat(temp_str,"/");
	temp_str[11] = VideoMemoCount/100+'0';
	temp_str[12] = (VideoMemoCount%100)/10+'0';
	temp_str[13] = VideoMemoCount%10+'0';
	strcat(temp_str,"]");
	strcpy(Video_Play_Title,temp_str);
	printf("-----titel---->%s\n",Video_Play_Title);
	return &Video_Play_Title;
}

char* VideoMemoTime(int index)
{
	int i;
	char buf[PATH_MAX];
	VideoMemoEntry* entry;
	char temp_str[30] = "\0";
#if 1
	entry = VideoMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	//memset(Video_Play_Time,'\0',sizeof(Video_Play_Time));
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
	strcpy(Video_Play_Time,temp_str);
	printf("222-----time---->%s\n",Video_Play_Time);
#endif	
	return &Video_Play_Time;
}

long VideoMemoSec(int index)
{
	int i;
	char buf[PATH_MAX];
	VideoMemoEntry* entry;
	char temp_str[30] = "\0";
        long file_sec;
	struct tm file_time;
	
	entry = VideoMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	
        file_time.tm_year = (entry->name[4]-'0')*1000+(entry->name[5]-'0')*100+(entry->name[6]-'0')*10 + (entry->name[7]-'0');
        file_time.tm_mon = (entry->name[8]-'0')*10+(entry->name[9]-'0');
        file_time.tm_mday = (entry->name[10]-'0')*10+(entry->name[11]-'0');
        file_time.tm_hour = (entry->name[13]-'0')*10+(entry->name[14]-'0');
        file_time.tm_min = (entry->name[15]-'0')*10+(entry->name[16]-'0');
        file_time.tm_sec = (entry->name[17]-'0')*10+(entry->name[18]-'0');
	file_time.tm_year = file_time.tm_year - 1970;
	printf("VideoMemoSec----------->%d-%d-%d %d:%d:%d\n",file_time.tm_year,file_time.tm_mon,
															file_time.tm_mday,file_time.tm_hour,
															file_time.tm_min,file_time.tm_sec);
        file_sec = mktime(&file_time);
		
	return file_sec;
}
static void VideoMemoDeleteFile(int index)
{
	char buf[128] = "\0";
	char temp_str[2];
	VideoMemoEntry* entry = &VideoMemoArray[index];
	//printf("delete file--111->%d   %d\n",entry->played,VideoMemoArray[index].played);

	strcpy(buf, VIDEOMEMO_PATH);
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
	strcat(buf, VIDEOMEMO_EXT);
	printf("delete file--->%s   %d\n",buf,entry->played);
	remove(buf);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
	ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
}

void VideoMemoDeleteEntry(int index)
{
	if (VideoMemoCount == 0 || index >= VideoMemoCount)
		return;
	VideoMemoDeleteFile(index);
	if(index != MAX_VIDEOMEMO_COUNT -1)
		memmove(&VideoMemoArray[index], &VideoMemoArray[index+1], sizeof(VideoMemoEntry) * (MAX_VIDEOMEMO_COUNT - 1 - index));
	VideoMemoCount--;
}

char* VideoMemoStartPlay(int index)
{
	char buf[50] = "\0";
	char buf_s[50] = "\0";
	VideoMemoEntry* entry;
	char temp_str[2];

	entry = VideoMemoGetEntry(index);
	if(entry == NULL)
		return NULL;
	memset(Video_Play_Path,0,sizeof(Video_Play_Path));
	strcpy(buf, VIDEOMEMO_PATH);
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
	printf("------------>played  = %d\n",entry->played);
	strcat(buf, VIDEOMEMO_EXT);
	if (!entry->played)
	{
		printf("@@@@@@@@@@@@@@@\n");
		Video_new_file = true;
		strcpy(buf_s, buf);
		buf_s[32] = '1';
		rename(buf,buf_s);
		strcpy(Video_Play_Path, buf_s);
		entry->played = 1;
		if (VideoMemoUnplayCount > 0)
			VideoMemoUnplayCount--;
	}
	else
		strcpy(Video_Play_Path, buf);
	//printf("VideoMemoStartPlay---------->%s\n",Video_Play_Path);
	return &Video_Play_Path;
}

void VideoMemoCheck()
{
	char buf[50] = "\0";
	char buf_s[50] = "\0";
	VideoMemoEntry* entry;
	char temp_str[2];
	int video_time = 0;

	entry = &VideoMemoArray[0];
	if(entry == NULL)
		return NULL;
	strcpy(buf, VIDEOMEMO_PATH);
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
	//printf("------------>played  = %d\n",entry->played);
	strcat(buf, VIDEOMEMO_EXT);
	mtal_pb_get_total_duration_ext(&video_time,buf);
	printf("video  ->%s    ->%d\n",buf,video_time);
	if(video_time == 0)
	{
		if(buf[32] == '0')
			VideoMemoUnplayCount--;
		VideoMemoDeleteEntry(0);
	}
	return;
}
char* VideoMemoRecord()
{
	VideoMemoEntry* entry;
	char* pch;
	
	struct timeval tv;
	struct tm* timeinfo;
	char filename[50] = "\0";
	char name_buf[30] = "\0";
	char name_buf_s[30] = "\0";


	if (VideoMemoRecording ||!cur_signal)
		return NULL;
	VideoMemoRecording = true;
	
	if(VideoMemoCount >= MAX_VIDEOMEMO_COUNT)
	{
		VideoMemoDeleteFile(MAX_VIDEOMEMO_COUNT-1);
		memmove(&VideoMemoArray[1], &VideoMemoArray[0], sizeof(VideoMemoEntry) * (MAX_VIDEOMEMO_COUNT-1));
	}
	else
		memmove(&VideoMemoArray[1], &VideoMemoArray[0], sizeof(VideoMemoEntry) * (VideoMemoCount));
	entry = &VideoMemoArray[0];
	
	gettimeofday(&tv, NULL);
	timeinfo = localtime((const time_t*)&tv.tv_sec);
	strftime(name_buf, 20, "%Y%m%d_%H%M%S", timeinfo);
	strcpy(name_buf_s, "VID_");
	strcat(name_buf_s,name_buf);
	strcpy(filename, VIDEOMEMO_PATH);
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
	if(auto_snap_filename||montion_enable)
		filename[31] = '1';
	else
		filename[31] = '0';
	filename[32] = '0';
	strcat(filename,VIDEOMEMO_EXT);
	strcpy(Video_Snap_Path,filename);
	printf("VideoMemoRecord[%d] filename --->%s\n",VideoMemoCount,Video_Snap_Path);
	strncpy(entry->name, name_buf_s,19);
	entry->cam= cur_signal;
	if(auto_snap_filename||montion_enable)
		entry->type= 1;
	else
		entry->type= 0;
	entry->played= 0;
	
	VideoMemoCount++;
	if(VideoMemoCount > MAX_VIDEOMEMO_COUNT)
		VideoMemoCount = MAX_VIDEOMEMO_COUNT;
	VideoMemoUnplayCount++;
	auto_snap_filename = false;
	//VideoMemoRecording = false;
	return Video_Snap_Path;
}

void VideoMemoRecEnd()
{
	VideoMemoRecording= false;
}

void video_memo_init()
{
	pthread_t      task;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, _video_memo_init, NULL);
}
