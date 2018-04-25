#include "ite/itu.h"

extern bool home_init(ITUWidget* widget, char* param);
extern bool home_timer(ITUWidget* widget, char* param);
extern bool home_leave(ITUWidget* widget, char* param);
extern bool home_go(ITUWidget* widget, char* param);
extern bool home_change_mute(ITUWidget* widget, char* param);
#if 1 //my.wei mask for test ahd
extern bool standby_init(ITUWidget* widget, char* param);
extern bool standby_time(ITUWidget* widget, char* param);
extern bool standby_leave(ITUWidget* widget, char* param);
extern bool date_init(ITUWidget* widget, char* param);
extern bool date_timer(ITUWidget* widget, char* param);
extern bool date_leave(ITUWidget* widget, char* param);
extern bool date_set_change(ITUWidget* widget, char* param);
extern bool date_set_back(ITUWidget* widget, char* param);
extern bool date_to_time_set(ITUWidget* widget, char* param);
extern bool update_date_text(ITUWidget* widget, char* param);
#endif
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
#if 1 //my.wei mask for test ahd
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

ITUActionFunction actionFunctions[] =
{
    "home_init", home_init,
    "home_timer", home_timer,
    "home_leave", home_leave,
    "home_go", home_go,
    "home_change_mute", home_change_mute,
#if 1 //my.wei mask for test ahd
    "standby_init", standby_init,
    "standby_time", standby_time,
    "standby_leave", standby_leave,
    "date_init", date_init,
    "date_timer", date_timer,
    "date_leave", date_leave,
    "date_set_change", date_set_change,
    "date_set_back", date_set_back,
    "date_to_time_set", date_to_time_set,
    "update_date_text", update_date_text,
#endif    
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
#if 1 //my.wei mask for test ahd    
    "media_init", media_init,
    "media_timer", media_timer,
    "media_leave", media_leave,
    "delete_video", delete_video,
    "video_bar_jump", video_bar_jump,
    "video_btn_jump_full", video_btn_jump_full,
    "video_btn_jump", video_btn_jump,
    "stop_video", stop_video,
    "play_video", play_video,
    "next_video", next_video,
    "prev_video", prev_video,
    "video_delete_pop", video_delete_pop,
    "video_back_list", video_back_list,
    "media_close_full", media_close_full,
    "media_full", media_full,
    "video_l_slistbox_3_load", video_l_slistbox_3_load,
    "view_video", view_video,
    "video_l_slistbox_2_load", video_l_slistbox_2_load,
    "video_l_slistbox_1_load", video_l_slistbox_1_load,
    "video_l_slistbox_0_load", video_l_slistbox_0_load,
    "media_change", media_change,
    "delete_photo", delete_photo,
    "next_photo", next_photo,
    "prev_photo", prev_photo,
    "photo_back_list", photo_back_list,
    "photo_l_slistbox_3_load", photo_l_slistbox_3_load,
    "view_photo", view_photo,
    "photo_l_slistbox_2_load", photo_l_slistbox_2_load,
    "photo_l_slistbox_1_load", photo_l_slistbox_1_load,
    "photo_l_slistbox_0_load", photo_l_slistbox_0_load,
    "intercom_init", intercom_init,
    "intercom_timer", intercom_timer,
    "intercom_leave", intercom_leave,
    "inter_call", inter_call,
    "intercom_ing_init", intercom_ing_init,
    "intercom_ing_timer", intercom_ing_timer,
    "intercom_ing_leave", intercom_ing_leave,
    "intercom_ing_adjust", intercom_ing_adjust,
    "intercom_answer", intercom_answer,
    "intercom_quit", intercom_quit,
    "intercom_hang", intercom_hang,
    "intercom_ing_vol_set", intercom_ing_vol_set,
    "set_main_bg_1_show", set_main_bg_1_show,
    "set_init", set_init,
    "set_timer", set_timer,
    "set_leave", set_leave,
    "set_pop_up_confirm", set_pop_up_confirm,
    "set_pop_up_cancel", set_pop_up_cancel,
    "set_modify", set_modify,
    "lang_slistbox_0_load", lang_slistbox_0_load,
    "set_change_lang", set_change_lang,
    "lang_slistbox_1_load", lang_slistbox_1_load,
    "set_dis_reset", set_dis_reset,
    "set_dis_ch_hue", set_dis_ch_hue,
    "set_dis_dajust", set_dis_dajust,
    "set_dis_ch_con", set_dis_ch_con,
    "set_dis_ch_bri", set_dis_ch_bri,
    "set_dis_back", set_dis_back,
    "set_time_back", set_time_back,
    "volume_set", volume_set,
    "set_vol_adjust", set_vol_adjust,
    "set_vol_back", set_vol_back,
    "set_md_time_change", set_md_time_change,
    "set_md_time_back", set_md_time_back,
    "set_md_video_check", set_md_video_check,
    "set_md_time", set_md_time,
    "set_auto_video_check", set_auto_video_check,
    "set_enter_lang", set_enter_lang,
    "set_enter_clear_mode", set_enter_clear_mode,
    "set_enter_display", set_enter_display,
    "set_to_time_set", set_to_time_set,
    "set_format", set_format,
    "set_main_bg_1_hide", set_main_bg_1_hide,
#endif    
    NULL, NULL
};
