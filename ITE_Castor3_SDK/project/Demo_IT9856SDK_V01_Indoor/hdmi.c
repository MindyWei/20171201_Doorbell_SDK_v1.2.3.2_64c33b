#include <sys/ioctl.h>
#include "ite/itp.h"
#include "hdmitx/mmp_hdmitx.h"

void* Hdmitxloop(void* arg)
{
	for (;;)
	{
		mmpHDMITXDevLoopProc(false, 0, 0, MMP_HDHITX_IN_HDMIRX);
		usleep(100000);
	}	
}

void HdmiInit (void)
{
	pthread_t task;
    pthread_attr_t attr;

	
	mmpHDMITXIsChipEmpty();
	mmpHDMITXInitialize(MMP_HDHITX_IN_HDMIRX);		
	mmpHDMITXSetDisplayOption(HDMI_720p60, HDMITX_UNKNOWN_MODE, FALSE, FALSE);
		
	pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, Hdmitxloop, NULL);
}
