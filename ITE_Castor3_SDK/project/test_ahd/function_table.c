#include "ite/itu.h"

extern bool MainMenuOnEnter(ITUWidget* widget, char* param);
extern bool MainMenuOnTimer(ITUWidget* widget, char* param);
extern bool MainMenuOnLeave(ITUWidget* widget, char* param);
extern bool MonitorOnEnter(ITUWidget* widget, char* param);
extern bool MonitorOnTimer(ITUWidget* widget, char* param);
extern bool MonitorOnLeave(ITUWidget* widget, char* param);
extern bool MonitorFullBackgroundButtonOnPress(ITUWidget* widget, char* param);
extern bool MonitorRecordingButtonOnPress(ITUWidget* widget, char* param);
extern bool MonitorSnapButtonOnPress(ITUWidget* widget, char* param);
extern bool MonitorRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool MonitorCameraSwitchButtonOnPress(ITUWidget* widget, char* param);
extern bool MonitorMDButtonOnPress(ITUWidget* widget, char* param);

extern bool AudioPlayerOnEnter(ITUWidget* widget, char* param);
extern bool AudioPlayerOnLeave(ITUWidget* widget, char* param);
extern bool AudioPlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerOnTimer(ITUWidget* widget, char* param);
extern bool AudioPlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool AudioPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool VideoPlayerOnEnter(ITUWidget* widget, char* param);
extern bool VideoPlayerOnLeave(ITUWidget* widget, char* param);
extern bool VideoPlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerOnTimer(ITUWidget* widget, char* param);
extern bool VideoPlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool VideoPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool VideoPlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoViewOnTimer(ITUWidget* widget, char* param);
extern bool VideoViewOnEnter(ITUWidget* widget, char* param);
extern bool VideoViewOnLeave(ITUWidget* widget, char* param);
extern bool VideoViewSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoViewUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoViewPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoViewRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool VideoViewNextButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewLastButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoViewViewButtonOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerOnEnter(ITUWidget* widget, char* param);
extern bool ImagePlayerOnLeave(ITUWidget* widget, char* param);
extern bool ImagePlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerOnTimer(ITUWidget* widget, char* param);
extern bool ImagePlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool ImagePlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool ImageViewOnTimer(ITUWidget* widget, char* param);
extern bool ImageViewOnEnter(ITUWidget* widget, char* param);
extern bool ImageViewOnLeave(ITUWidget* widget, char* param);
extern bool ImageViewSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImageViewUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImageViewViewButtonOnPress(ITUWidget* widget, char* param);
extern bool ImageViewNextButtonOnPress(ITUWidget* widget, char* param);
extern bool ImageViewLastButtonOnPress(ITUWidget* widget, char* param);
extern bool CalendarOnEnter(ITUWidget* widget, char* param);
extern bool CalendarWheelOnChanged(ITUWidget* widget, char* param);
extern bool CalendarConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool CalendarCoverFlowOnChanged(ITUWidget* widget, char* param);
extern bool CalendarBackButtonOnPress(ITUWidget* widget, char* param);
extern bool ClockOnEnter(ITUWidget* widget, char* param);
extern bool ClockOnTimer(ITUWidget* widget, char* param);
extern bool ClockConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool StandbyOnEnter(ITUWidget* widget, char* param);
extern bool StandbyOnTimer(ITUWidget* widget, char* param);
extern bool StandbyOnLeave(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    "MainMenuOnEnter", MainMenuOnEnter,
    "MainMenuOnTimer", MainMenuOnTimer,
    "MainMenuOnLeave", MainMenuOnLeave,

    "MonitorOnEnter", MonitorOnEnter,
    "MonitorOnTimer", MonitorOnTimer,
    "MonitorOnLeave", MonitorOnLeave,
    "MonitorFullBackgroundButtonOnPress", MonitorFullBackgroundButtonOnPress,
    "MonitorRecordingButtonOnPress", MonitorRecordingButtonOnPress,
    "MonitorSnapButtonOnPress", MonitorSnapButtonOnPress,
    "MonitorRecordButtonOnPress", MonitorRecordButtonOnPress,
    "MonitorCameraSwitchButtonOnPress", MonitorCameraSwitchButtonOnPress,
    "MonitorMDButtonOnPress", MonitorMDButtonOnPress,

    "AudioPlayerOnEnter", AudioPlayerOnEnter,
    "AudioPlayerOnLeave", AudioPlayerOnLeave,
    "AudioPlayerSDInsertedOnCustom", AudioPlayerSDInsertedOnCustom,
    "AudioPlayerSDRemovedOnCustom", AudioPlayerSDRemovedOnCustom,
    "AudioPlayerUsbInsertedOnCustom", AudioPlayerUsbInsertedOnCustom,
    "AudioPlayerUsbRemovedOnCustom", AudioPlayerUsbRemovedOnCustom,
    "AudioPlayerOnTimer", AudioPlayerOnTimer,
    "AudioPlayerStorageTypeCheckBoxOnPress", AudioPlayerStorageTypeCheckBoxOnPress,
    "AudioPlayerStorageRadioBoxOnPress", AudioPlayerStorageRadioBoxOnPress,
    "AudioPlayerRepeatButtonOnPress", AudioPlayerRepeatButtonOnPress,
    "AudioPlayerVolTrackBarOnChanged", AudioPlayerVolTrackBarOnChanged,
    "AudioPlayerPlayCheckBoxOnPress", AudioPlayerPlayCheckBoxOnPress,
    "AudioPlayerNextButtonOnPress", AudioPlayerNextButtonOnPress,
    "AudioPlayerLastButtonOnPress", AudioPlayerLastButtonOnPress,
    "AudioPlayerRandomCheckBoxOnPress", AudioPlayerRandomCheckBoxOnPress,
    "AudioPlayerScrollMediaFileListBoxOnSelection", AudioPlayerScrollMediaFileListBoxOnSelection,

    "VideoPlayerOnEnter", VideoPlayerOnEnter,
    "VideoPlayerOnLeave", VideoPlayerOnLeave,
    "VideoPlayerSDInsertedOnCustom", VideoPlayerSDInsertedOnCustom,
    "VideoPlayerSDRemovedOnCustom", VideoPlayerSDRemovedOnCustom,
    "VideoPlayerUsbInsertedOnCustom", VideoPlayerUsbInsertedOnCustom,
    "VideoPlayerUsbRemovedOnCustom", VideoPlayerUsbRemovedOnCustom,
    "VideoPlayerOnTimer", VideoPlayerOnTimer,
    "VideoPlayerStorageRadioBoxOnPress", VideoPlayerStorageRadioBoxOnPress,
    "VideoPlayerVolTrackBarOnChanged", VideoPlayerVolTrackBarOnChanged,
    "VideoPlayerPlayCheckBoxOnPress", VideoPlayerPlayCheckBoxOnPress,
    "VideoPlayerNextButtonOnPress", VideoPlayerNextButtonOnPress,
    "VideoPlayerLastButtonOnPress", VideoPlayerLastButtonOnPress,
    "VideoPlayerRandomCheckBoxOnPress", VideoPlayerRandomCheckBoxOnPress,
    "VideoPlayerRepeatButtonOnPress", VideoPlayerRepeatButtonOnPress,
    "VideoPlayerScrollMediaFileListBoxOnSelection", VideoPlayerScrollMediaFileListBoxOnSelection,
    "VideoPlayerStorageTypeCheckBoxOnPress", VideoPlayerStorageTypeCheckBoxOnPress,

    "VideoViewOnTimer", VideoViewOnTimer,
    "VideoViewOnEnter", VideoViewOnEnter,
    "VideoViewOnLeave", VideoViewOnLeave,
    "VideoViewSDRemovedOnCustom", VideoViewSDRemovedOnCustom,
    "VideoViewUsbRemovedOnCustom", VideoViewUsbRemovedOnCustom,
    "VideoViewPlayCheckBoxOnPress", VideoViewPlayCheckBoxOnPress,
    "VideoViewRepeatButtonOnPress", VideoViewRepeatButtonOnPress,
    "VideoViewVolTrackBarOnChanged", VideoViewVolTrackBarOnChanged,
    "VideoViewNextButtonOnPress", VideoViewNextButtonOnPress,
    "VideoViewLastButtonOnPress", VideoViewLastButtonOnPress,
    "VideoViewRandomCheckBoxOnPress", VideoViewRandomCheckBoxOnPress,
    "VideoViewViewButtonOnPress", VideoViewViewButtonOnPress,

    "ImagePlayerOnEnter", ImagePlayerOnEnter,
    "ImagePlayerOnLeave", ImagePlayerOnLeave,
    "ImagePlayerSDInsertedOnCustom", ImagePlayerSDInsertedOnCustom,
    "ImagePlayerSDRemovedOnCustom", ImagePlayerSDRemovedOnCustom,
    "ImagePlayerUsbInsertedOnCustom", ImagePlayerUsbInsertedOnCustom,
    "ImagePlayerUsbRemovedOnCustom", ImagePlayerUsbRemovedOnCustom,
    "ImagePlayerOnTimer", ImagePlayerOnTimer,
    "ImagePlayerStorageRadioBoxOnPress", ImagePlayerStorageRadioBoxOnPress,
    "ImagePlayerNextButtonOnPress", ImagePlayerNextButtonOnPress,
    "ImagePlayerLastButtonOnPress", ImagePlayerLastButtonOnPress,
    "ImagePlayerScrollMediaFileListBoxOnSelection", ImagePlayerScrollMediaFileListBoxOnSelection,
    "ImagePlayerStorageTypeCheckBoxOnPress", ImagePlayerStorageTypeCheckBoxOnPress,

    "ImageViewOnTimer", ImageViewOnTimer,
    "ImageViewOnEnter", ImageViewOnEnter,
    "ImageViewOnLeave", ImageViewOnLeave,
    "ImageViewSDRemovedOnCustom", ImageViewSDRemovedOnCustom,
    "ImageViewUsbRemovedOnCustom", ImageViewUsbRemovedOnCustom,
    "ImageViewViewButtonOnPress", ImageViewViewButtonOnPress,
    "ImageViewNextButtonOnPress", ImageViewNextButtonOnPress,
    "ImageViewLastButtonOnPress", ImageViewLastButtonOnPress,

    "CalendarOnEnter", CalendarOnEnter,
    "CalendarWheelOnChanged", CalendarWheelOnChanged,
    "CalendarConfirmButtonOnPress", CalendarConfirmButtonOnPress,
    "CalendarCoverFlowOnChanged", CalendarCoverFlowOnChanged,
    "CalendarBackButtonOnPress", CalendarBackButtonOnPress,

    "ClockOnEnter", ClockOnEnter,
    "ClockOnTimer", ClockOnTimer,
    "ClockConfirmButtonOnPress", ClockConfirmButtonOnPress,

	"StandbyOnEnter", StandbyOnEnter,
	"StandbyOnTimer", StandbyOnTimer,
	"StandbyOnLeave", StandbyOnLeave,

    NULL, NULL
};
