#include <assert.h>
#include "scene.h"

bool E1PlayAudio(ITUWidget* widget, char* param)
{
	AudioPlayMusic("A:/sounds/maintenance.mp3", NULL);
    return true;
}

