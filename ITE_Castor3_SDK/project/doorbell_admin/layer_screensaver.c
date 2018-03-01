#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include "scene.h"
#include "doorbell.h"

static ITUIcon* screensaverIcon;
static ITUAnalogClock* screensaverAnalogClock;

bool ScreensaverOnEnter(ITUWidget* widget, char* param)
{
    if (!screensaverAnalogClock)
    {
        screensaverIcon = ituSceneFindWidget(&theScene, "screensaverIcon");
        assert(screensaverIcon);

        screensaverAnalogClock = ituSceneFindWidget(&theScene, "screensaverAnalogClock");
        assert(screensaverAnalogClock);
    }
    ituWidgetSetVisible(screensaverAnalogClock, false);
    ituWidgetSetVisible(screensaverIcon, false);

    switch (theConfig.screensaver_type)
    {
    case SCREENSAVER_CLOCK:
        ituWidgetSetVisible(screensaverAnalogClock, true);
       break;

    case SCREENSAVER_PHOTO:
        {
            // try to load screensaver jpeg file if exists
            FILE* f = fopen(CFG_PUBLIC_DRIVE ":/screensaver.jpg", "rb");
            if (f)
            {
                uint8_t* data;
                int size;

                fseek(f, 0, SEEK_END);
                size = ftell(f);
                fseek(f, 0, SEEK_SET);

                data = malloc(size);
                if (data)
                {
                    size = fread(data, 1, size, f);
                    ituIconLoadJpegData(screensaverIcon, data, size);
                    free(data);
                }
                fclose(f);
            }
            ituWidgetSetVisible(screensaverIcon, true);
       }
       break;
    }
	return true;
}


void ScreensaverReset(void)
{
    screensaverAnalogClock = NULL;
}
