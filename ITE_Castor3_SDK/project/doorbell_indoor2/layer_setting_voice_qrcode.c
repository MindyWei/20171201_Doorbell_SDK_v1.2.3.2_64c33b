#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

#define MAX_FILE_SIZE  6000000

static ITUIcon* settingVoiceQRCodeIcon;
static ITUTextBox* settingVoiceKeyboardUserTextBox;

static uint8_t* qrcodeImageContent;
static int qrcodeImageContentSize;
static bool qrcodeImageLoaded;

static void QRCodeImageClear(void)
{
    if (qrcodeImageContentSize == 0)
        return;

    qrcodeImageContentSize = 0; // to avoid reentrant

    free(qrcodeImageContent);
    qrcodeImageContent = NULL;
    ituIconReleaseSurface(settingVoiceQRCodeIcon);
}

static void QRCodeGetImage(uint8_t* image, int imageSize)
{
    QRCodeImageClear();
    qrcodeImageContent = image;
    qrcodeImageContentSize = imageSize;
    
    if (imageSize && imageSize < MAX_FILE_SIZE)
    {
        qrcodeImageLoaded = true;
    }
    else
    {
        printf("JPG file can not bigger than 6M bytes!!\n");
        free(qrcodeImageContent);
        qrcodeImageContent = NULL;
        qrcodeImageContentSize = 0;
    }
}

bool SettingVoiceQRCodeOnTimer(ITUWidget* widget, char* param)
{
    if (qrcodeImageLoaded)
    {
        //show the retrieve Jpeg image file.
        ituIconLoadJpegData(settingVoiceQRCodeIcon, qrcodeImageContent, qrcodeImageContentSize);
        free(qrcodeImageContent);
        qrcodeImageContentSize = 0;
        qrcodeImageLoaded = false;
    }
    return true;
}

bool SettingVoiceQRCodeOnEnter(ITUWidget* widget, char* param)
{
    if (!settingVoiceQRCodeIcon)
    {
        settingVoiceQRCodeIcon = ituSceneFindWidget(&theScene, "settingVoiceQRCodeIcon");
        assert(settingVoiceQRCodeIcon);

        settingVoiceKeyboardUserTextBox = ituSceneFindWidget(&theScene, "settingVoiceKeyboardUserTextBox");
        assert(settingVoiceKeyboardUserTextBox);
    }

    SceneLeaveVideoState();
    QRCodeImageClear();

    qrcodeImageContent = NULL;
    qrcodeImageContentSize = 0;
    QRCodeRegister(ituTextBoxGetString(settingVoiceKeyboardUserTextBox), QRCodeGetImage);

    return true;
}

void SettingVoiceQRCodeReset(void)
{
    settingVoiceQRCodeIcon = NULL;
}
