#include "ite/itu.h"

extern bool MainMenuOnEnter(ITUWidget* widget, char* param);
extern bool MainMenuOnTimer(ITUWidget* widget, char* param);
extern bool MainMenuOnLeave(ITUWidget* widget, char* param);
extern bool monitor_init(ITUWidget* widget, char* param);
extern bool monitor_timer(ITUWidget* widget, char* param);
extern bool monitor_leave(ITUWidget* widget, char* param);
extern bool monitor_gain_adjust(ITUWidget* widget, char* param);
extern bool monitor_vol_adjust(ITUWidget* widget, char* param);
extern bool monitor_dis_reset(ITUWidget* widget, char* param);
extern bool monitor_dis_ch_hue(ITUWidget* widget, char* param);
extern bool monitor_dis_dajust(ITUWidget* widget, char* param);
extern bool monitor_dis_ch_con(ITUWidget* widget, char* param);
extern bool monitor_dis_ch_bri(ITUWidget* widget, char* param);
extern bool monitor_dis_full(ITUWidget* widget, char* param);
extern bool monitor_rec_end(ITUWidget* widget, char* param);
extern bool monitor_dis_set(ITUWidget* widget, char* param);
extern bool monitor_snap(ITUWidget* widget, char* param);
extern bool monitor_rec_start(ITUWidget* widget, char* param);
extern bool monitor_open(ITUWidget* widget, char* param);
extern bool monitor_talk(ITUWidget* widget, char* param);
extern bool monitor_sw_cam(ITUWidget* widget, char* param);
extern bool monitor_vol_set(ITUWidget* widget, char* param);
extern bool MonitorMDButtonOnPress(ITUWidget* widget, char* param);

#if 0 //my.wei mask for test ahd
extern bool media_init(ITUWidget* widget, char* param);
extern bool media_timer(ITUWidget* widget, char* param);
extern bool media_leave(ITUWidget* widget, char* param);
extern bool delete_video(ITUWidget* widget, char* param);
extern bool video_bar_jump(ITUWidget* widget, char* param);
extern bool video_btn_jump_full(ITUWidget* widget, char* param);
extern bool video_btn_jump(ITUWidget* widget, char* param);
extern bool stop_video(ITUWidget* widget, char* param);
extern bool play_video(ITUWidget* widget, char* param);
extern bool next_video(ITUWidget* widget, char* param);
extern bool prev_video(ITUWidget* widget, char* param);
extern bool video_delete_pop(ITUWidget* widget, char* param);
extern bool video_back_list(ITUWidget* widget, char* param);
extern bool media_close_full(ITUWidget* widget, char* param);
extern bool media_full(ITUWidget* widget, char* param);
extern bool video_l_slistbox_3_load(ITUWidget* widget, char* param);
extern bool view_video(ITUWidget* widget, char* param);
extern bool video_l_slistbox_2_load(ITUWidget* widget, char* param);
extern bool video_l_slistbox_1_load(ITUWidget* widget, char* param);
extern bool video_l_slistbox_0_load(ITUWidget* widget, char* param);
extern bool media_change(ITUWidget* widget, char* param);
extern bool delete_photo(ITUWidget* widget, char* param);
extern bool next_photo(ITUWidget* widget, char* param);
extern bool prev_photo(ITUWidget* widget, char* param);
extern bool photo_back_list(ITUWidget* widget, char* param);
extern bool photo_l_slistbox_3_load(ITUWidget* widget, char* param);
extern bool view_photo(ITUWidget* widget, char* param);
extern bool photo_l_slistbox_2_load(ITUWidget* widget, char* param);
extern bool photo_l_slistbox_1_load(ITUWidget* widget, char* param);
extern bool photo_l_slistbox_0_load(ITUWidget* widget, char* param);
extern bool intercom_init(ITUWidget* widget, char* param);
extern bool intercom_timer(ITUWidget* widget, char* param);
extern bool intercom_leave(ITUWidget* widget, char* param);
extern bool inter_call(ITUWidget* widget, char* param);
extern bool intercom_ing_init(ITUWidget* widget, char* param);
extern bool intercom_ing_timer(ITUWidget* widget, char* param);
extern bool intercom_ing_leave(ITUWidget* widget, char* param);
extern bool intercom_ing_adjust(ITUWidget* widget, char* param);
extern bool intercom_answer(ITUWidget* widget, char* param);
extern bool intercom_quit(ITUWidget* widget, char* param);
extern bool intercom_hang(ITUWidget* widget, char* param);
extern bool intercom_ing_vol_set(ITUWidget* widget, char* param);
extern bool set_main_bg_1_show(ITUWidget* widget, char* param);
extern bool set_init(ITUWidget* widget, char* param);
extern bool set_timer(ITUWidget* widget, char* param);
extern bool set_leave(ITUWidget* widget, char* param);
extern bool set_pop_up_confirm(ITUWidget* widget, char* param);
extern bool set_pop_up_cancel(ITUWidget* widget, char* param);
extern bool set_modify(ITUWidget* widget, char* param);
extern bool lang_slistbox_0_load(ITUWidget* widget, char* param);
extern bool set_change_lang(ITUWidget* widget, char* param);
extern bool lang_slistbox_1_load(ITUWidget* widget, char* param);
extern bool set_dis_reset(ITUWidget* widget, char* param);
extern bool set_dis_ch_hue(ITUWidget* widget, char* param);
extern bool set_dis_dajust(ITUWidget* widget, char* param);
extern bool set_dis_ch_con(ITUWidget* widget, char* param);
extern bool set_dis_ch_bri(ITUWidget* widget, char* param);
extern bool set_dis_back(ITUWidget* widget, char* param);
extern bool set_time_back(ITUWidget* widget, char* param);
extern bool volume_set(ITUWidget* widget, char* param);
extern bool set_vol_adjust(ITUWidget* widget, char* param);
extern bool set_vol_back(ITUWidget* widget, char* param);
extern bool set_md_time_change(ITUWidget* widget, char* param);
extern bool set_md_time_back(ITUWidget* widget, char* param);
extern bool set_md_video_check(ITUWidget* widget, char* param);
extern bool set_md_time(ITUWidget* widget, char* param);
extern bool set_auto_video_check(ITUWidget* widget, char* param);
extern bool set_enter_lang(ITUWidget* widget, char* param);
extern bool set_enter_clear_mode(ITUWidget* widget, char* param);
extern bool set_enter_display(ITUWidget* widget, char* param);
extern bool set_to_time_set(ITUWidget* widget, char* param);
extern bool set_format(ITUWidget* widget, char* param);
extern bool set_main_bg_1_hide(ITUWidget* widget, char* param);
#endif

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

    "monitor_init", monitor_init,
    "monitor_timer", monitor_timer,
    "monitor_leave", monitor_leave,
    "monitor_gain_adjust", monitor_gain_adjust,
    "monitor_vol_adjust", monitor_vol_adjust,
    "monitor_dis_reset", monitor_dis_reset,
    "monitor_dis_ch_hue", monitor_dis_ch_hue,
    "monitor_dis_dajust", monitor_dis_dajust,
    "monitor_dis_ch_con", monitor_dis_ch_con,
    "monitor_dis_ch_bri", monitor_dis_ch_bri,
    "monitor_dis_full", monitor_dis_full,
    "monitor_rec_end", monitor_rec_end,
    "monitor_dis_set", monitor_dis_set,
    "monitor_snap", monitor_snap,
    "monitor_rec_start", monitor_rec_start,
    "monitor_open", monitor_open,
    "monitor_talk", monitor_talk,
    "monitor_sw_cam", monitor_sw_cam,
    "monitor_vol_set", monitor_vol_set,
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
