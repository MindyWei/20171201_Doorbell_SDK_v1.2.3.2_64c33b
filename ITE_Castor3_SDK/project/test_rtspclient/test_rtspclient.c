#include <pthread.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "ite/itp.h"
#include "castor3player.h"
#include "ite/itv.h"
#include "ite/ith_video.h"
#include "network.h"

/*
 * Global variables
 */

/*
 * Functions
 */

static void event_loop()
{
    int current_time = 0;
    int i;

    printf("*********************************\n");
    printf("*  iTE RTSPClient demonstrate  *\n");
    printf("*********************************\n\n");

#ifdef CFG_RTSP_CLIENT_ENABLE
    startRTSPClient("admin:admin@192.168.190.31", 554, NULL, NULL);
#endif

    while(1) usleep(500000);
}

static void
VideoInit(
    void)
{
    ithVideoInit(NULL);
    itv_init();
}

static void
VideoExit(
    void)
{
    /* release dbuf & itv */
    itv_flush_dbuf();
    itv_deinit();

    /* release decoder stuff */
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    ithVideoExit();
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}

void *TestFunc(void *arg)
{
    // target board drivers init
    itpInit();
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);
    
    NetworkInit();

    sleep(1);

    VideoInit();
    event_loop();
    VideoExit();

    itpExit();
    malloc_stats();
    return 0;
}