#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "scene.h"
#include "leaf_mediastream.h"

//call
extern LeafCall *call;
static ITUIcon* messageServiceViewImageIcon;

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void messageServiceViewImageIconDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);    
}
#endif

void MessageServiceViewShow(char* image)
{
    if (!messageServiceViewImageIcon)
    {
        messageServiceViewImageIcon = ituSceneFindWidget(&theScene, "messageServiceViewImageIcon");
        assert(messageServiceViewImageIcon);
    }
}

bool MessageServiceViewOnEnter(ITUWidget* widget, char* param)
{   
	int x, y, width, height;

    if (!messageServiceViewImageIcon)
    {
        messageServiceViewImageIcon = ituSceneFindWidget(&theScene, "messageServiceViewImageIcon");
        assert(messageServiceViewImageIcon);
    }

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
	ituWidgetSetDraw(messageServiceViewImageIcon, messageServiceViewImageIconDraw);
	SceneEnterVideoState(33);
#endif  
	// Jpeg
	ituWidgetGetGlobalPosition(messageServiceViewImageIcon, &x, &y);
	width = ituWidgetGetWidth(messageServiceViewImageIcon);
	height = ituWidgetGetHeight(messageServiceViewImageIcon);

#ifndef _WIN32
	leaf_show_snapshot(call,width,height,"E:/snapshot.jpg");
#endif

	return true;
}

bool MessageServiceViewOnTimer(ITUWidget* widget, char* param)
{

}


bool MessageServiceViewOnLeave(ITUWidget* widget, char* param)
{
#ifndef _WIN32
	itv_stop_vidSurf_anchor();
#endif
#ifdef CFG_BUILD_ITV            
	itv_flush_dbuf();
#ifndef CFG_TEST_VIDEO
	itv_set_pb_mode(0);
#endif
#endif  
	SceneLeaveVideoState();

	return true;
}

void MessageServiceViewReset(void)
{
    messageServiceViewImageIcon = NULL;
}
