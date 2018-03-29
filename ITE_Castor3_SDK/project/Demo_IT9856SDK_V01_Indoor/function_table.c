#include "ite/itu.h"

//Logo
extern bool LogoOnEnter(ITUWidget* widget, char* param);

//TouchCalibration
extern bool TouchCalibrationCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool TouchCalibrationOnEnter(ITUWidget* widget, char* param);
extern bool TouchCalibrationOnLeave(ITUWidget* widget, char* param);

//Screansaver
extern bool ScreensaverOnEnter(ITUWidget* widget, char* param);

//Upgrade
extern bool UpgradeOnTimer(ITUWidget* widget, char* param);
extern bool UpgradeOnEnter(ITUWidget* widget, char* param);

//Main
extern bool MainSetDoNotDisturbModeOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyMissedCallsOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyMessagesOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyGuardOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyCallIncomingOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyVoiceMemoIncomingOnCustom(ITUWidget* widget, char* param);

//MainMenu
extern bool mainMenuCoverFlowOnChanged(ITUWidget* widget, char* param);
extern bool MainMenuOnEnter(ITUWidget* widget, char* param);
extern bool MainMenuOnLeave(ITUWidget* widget, char* param);
extern bool MainMenuColorPickerOnSelection(ITUWidget* widget, char* param);
extern bool MainMenuWallpaperButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuResetButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuSaveButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuOnTimer(ITUWidget* widget, char* param);
extern bool MainMenuElevatorDownButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuElevatorUpButtonOnPress(ITUWidget* widget, char* param);

//Calendar
extern bool CalendarOnEnter(ITUWidget* widget, char* param);
extern bool CalendarConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool CalendarWheelOnChanged(ITUWidget* widget, char* param);
extern bool CalendarCoverFlowOnChanged(ITUWidget* widget, char* param);
extern bool CalendarBackButtonOnPress(ITUWidget* widget, char* param);

//Clock
extern bool ClockOnEnter(ITUWidget* widget, char* param);
extern bool ClockOnTimer(ITUWidget* widget, char* param);
extern bool ClockConfirmButtonOnPress(ITUWidget* widget, char* param);

//Weather
extern bool WeatherCurrTemperatureButtonOnMouseLongPress(ITUWidget* widget, char* param);
extern bool WeatherOnTimer(ITUWidget* widget, char* param);
extern bool WeatherOnEnter(ITUWidget* widget, char* param);
extern bool WeatherOnLeave(ITUWidget* widget, char* param);

//Camera
extern bool CameraScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool CameraScrollListOnSelection(ITUWidget* widget, char* param);
extern bool CameraScreenButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOnEnter(ITUWidget* widget, char* param);
extern bool CameraOnLeave(ITUWidget* widget, char* param);
extern bool CameraOnTimer(ITUWidget* widget, char* param);
extern bool CameraRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraDisconnectButtonOnPress(ITUWidget* widget, char* param);

//CameraOutdoor
extern bool CameraOutdoorOnEnter(ITUWidget* widget, char* param);
extern bool CameraOutdoorOnLeave(ITUWidget* widget, char* param);
extern bool CameraOutdoorOnTimer(ITUWidget* widget, char* param);
extern bool CameraOutdoorButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOutdoorRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOutdoorStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOutdoorUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOutdoorSnapshotButtonOnPress(ITUWidget* widget, char* param);

//CameraView
extern bool CameraViewOnEnter(ITUWidget* widget, char* param);
extern bool CameraViewOnLeave(ITUWidget* widget, char* param);
extern bool CameraViewScreenButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewDisconnectButtonOnPress(ITUWidget* widget, char* param);

//House
extern bool HouseModeIndoorRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool HouseModeOutdoorRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool HouseModeSleepRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool HouseLightScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseLightScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseLightScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseAirConditionerScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseAirConditionerScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseAirConditionerScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseCurtainScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseCurtainCloseScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseCurtainCloseScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseCurtainPauseScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseCurtainPauseScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseCurtainOpenScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseCurtainOpenScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HousePlugScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HousePlugScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HousePlugScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseTVScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseTVScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseTVScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseModeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseModeScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool HouseModeScrollIconListBoxOnSelection(ITUWidget* widget, char* param);
extern bool HouseOnEnter(ITUWidget* widget, char* param);
extern bool HouseOnLeave(ITUWidget* widget, char* param);

//AirConditioner
extern bool AirConditionerOnEnter(ITUWidget* widget, char* param);
extern bool AirConditionerOnLeave(ITUWidget* widget, char* param);
extern bool AirConditionerOnTimer(ITUWidget* widget, char* param);
extern bool AirConditionerPowerButtonOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerAutoRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerCoolRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerDryRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerFanRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerHeatRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerWindSlowRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerWindNormalRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerWindFastRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerWindAutoRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AirConditionerCurrTempBackgroundButtonOnMouseLongPress(ITUWidget* widget, char* param);
extern bool AirConditionerTempCTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool AirConditionerTempFTrackBarOnChanged(ITUWidget* widget, char* param);

//VoiceMemo
extern bool VoiceMemoOnEnter(ITUWidget* widget, char* param);
extern bool VoiceMemoOnLeave(ITUWidget* widget, char* param);
extern bool VoiceMemoOnTimer(ITUWidget* widget, char* param);
extern bool VoiceMemoScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool VoiceMemoNameScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool VoiceMemoTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool VoiceMemoScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool VoiceMemoScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param);
extern bool VoiceMemoRecCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VoiceMemoDeleteButtonOnPress(ITUWidget* widget, char* param);

//Phone
extern bool PhoneNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneCallButtonOnPress(ITUWidget* widget, char* param);

//PhoneResident
extern bool PhoneResidentOnEnter(ITUWidget* widget, char* param);
extern bool PhoneResidentOnLeave(ITUWidget* widget, char* param);
extern bool PhoneBackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneLogScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneLogScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool PhoneLogAddButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogBlackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogWhiteButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogClearButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneResidentRoomBackgroundButtonOnMouseLongPress(ITUWidget* widget, char* param);
extern bool PhoneLogScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param);

//PhoneAdmin
extern bool PhoneAdminOnEnter(ITUWidget* widget, char* param);
extern bool PhoneAdminScrollListBoxOnLoad(ITUWidget* widget, char* param);

//PhoneMisc
extern bool PhoneMiscOnEnter(ITUWidget* widget, char* param);
extern bool PhoneMiscScrollListBoxOnLoad(ITUWidget* widget, char* param);

//PhoneContact
extern bool PhoneContactOnEnter(ITUWidget* widget, char* param);
extern bool PhoneContactNameScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneContactCommentScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneContactScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param);
extern bool PhoneContactAddButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactBlackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactWhiteButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactClearButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactAddBackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactAddConfirmButtonOnPress(ITUWidget* widget, char* param);

//PhoneContactKeyboard
extern bool PhoneContactKeyboardEnterButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardBackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardChsButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneContactKeyboardOnEnter(ITUWidget* widget, char* param);

//Calling
extern bool CallingVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool CallingRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingAnswerButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingHangUpButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingErrorConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingWaitingCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingWaitingConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool CallingConnectedOnCustom(ITUWidget* widget, char* param);
extern bool CallingErrorOnCustom(ITUWidget* widget, char* param);
extern bool CallingIncomingOnCustom(ITUWidget* widget, char* param);
extern bool CallingOnTimer(ITUWidget* widget, char* param);
extern bool CallingOnEnter(ITUWidget* widget, char* param);
extern bool CallingOnLeave(ITUWidget* widget, char* param);

//Called
extern bool CalledVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool CalledRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledAnswerButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledHangUpButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledWaitingCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledWaitingConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledIncomingOnCustom(ITUWidget* widget, char* param);
extern bool CalledConnectedOnCustom(ITUWidget* widget, char* param);
extern bool CalledOnTimer(ITUWidget* widget, char* param);
extern bool CalledOnEnter(ITUWidget* widget, char* param);
extern bool CalledOnLeave(ITUWidget* widget, char* param);
extern bool CalledSourceButtonOnPress(ITUWidget* widget, char* param);
extern bool CalledSourceScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool CalledSourceScrollListBoxOnSelection(ITUWidget* widget, char* param);

//Message
extern bool MessageNotifyVideoMessageOnCustom(ITUWidget* widget, char* param);
extern bool MessageNotifyTextMessageOnCustom(ITUWidget* widget, char* param);

//MessageVideo
extern bool MessageVideoTopicScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageVideoTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageVideoScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool MessageVideoDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool MessageVideoOnEnter(ITUWidget* widget, char* param);
extern bool MessageVideoOnLeave(ITUWidget* widget, char* param);

//MessageVideoView
extern bool MessageVideoViewOnEnter(ITUWidget* widget, char* param);
extern bool MessageVideoViewOnLeave(ITUWidget* widget, char* param);
extern bool MessageVideoViewOnTimer(ITUWidget* widget, char* param);
extern bool MessageVideoViewVolTrackBarOnChanged(ITUWidget* widget, char* param);

//MessageRecord
extern bool MessageRecordScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageRecordTopicScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageRecordTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageRecordScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool MessageRecordDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool MessageRecordOnEnter(ITUWidget* widget, char* param);

//MessageRecordView
extern bool MessageRecordViewOnEnter(ITUWidget* widget, char* param);
extern bool MessageRecordViewOnLeave(ITUWidget* widget, char* param);
extern bool MessageRecordViewOnTimer(ITUWidget* widget, char* param);

//MessageService
extern bool MessageServiceTopicScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageServiceTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool MessageServiceScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool MessageServiceDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool MessageServiceOnEnter(ITUWidget* widget, char* param);
extern bool MessageServiceOnLeave(ITUWidget* widget, char* param);

//MessageserviceView
extern bool MessageServiceViewOnEnter(ITUWidget* widget, char* param);
extern bool MessageServiceViewOnLeave(ITUWidget* widget, char* param);
extern bool MessageServiceViewOnTimer(ITUWidget* widget, char* param);

//Guard
extern bool GuardIndoorRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool GuardOutdoorRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool GuardSleepRadioBoxOnPress(ITUWidget* widget, char* param);

//GuardMain
extern bool GuardMainOnEnter(ITUWidget* widget, char* param);
extern bool GuardMainOnLeave(ITUWidget* widget, char* param);

//GuardCheck
extern bool GuardCheckScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardCheckSensorScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardCheckStatusScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardCheckOnEnter(ITUWidget* widget, char* param);

//GuardUnguard
extern bool GuardUnguardConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardUnguardOnEnter(ITUWidget* widget, char* param);

//GuardLog
extern bool GuardLogScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogSensorScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogTypeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogClearButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardLogOnEnter(ITUWidget* widget, char* param);

//GuardStatus
extern bool GuardStatusScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardStatusSensorScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardStatusTypeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardStatusTimeScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardStatusOnEnter(ITUWidget* widget, char* param);

//GuardPassword
extern bool GuardPasswordOldDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordNewDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordConfirmDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordClearButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordSaveButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardPasswordOnEnter(ITUWidget* widget, char* param);

//GuardUser
extern bool GuardUserConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardUserOnEnter(ITUWidget* widget, char* param);

//GuardUserSettingDetail
extern bool GuardUserSettingDetailScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardUserSettingDetailSensorScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardUserSettingDetailStatusScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardUserSettingDetailStatusScrollListBoxOnSelect(ITUWidget* widget, char* param);
extern bool GuardUserSettingDetailOnEnter(ITUWidget* widget, char* param);
extern bool GuardUserSettingDetailOnLeave(ITUWidget* widget, char* param);

//Setting
extern bool SettingIconScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingScrollListBoxOnSelection(ITUWidget* widget, char* param);
extern bool SettingPasswordConfirmButtonOnPress(ITUWidget* widget, char* param);

//SettingDisplay
extern bool SettingDisplayBrightnessTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingDisplayScreensaverTimeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisplayScreensaverTypeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisplayOnEnter(ITUWidget* widget, char* param);
extern bool SettingDisplayOnLeave(ITUWidget* widget, char* param);

//SettingSound
extern bool SettingSoundKeyVolumeMeterOnChanged(ITUWidget* widget, char* param);
extern bool SettingSoundRingVolumeMeterOnChanged(ITUWidget* widget, char* param);
extern bool SettingSoundKeyRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSoundRingRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSoundOnEnter(ITUWidget* widget, char* param);
extern bool SettingSoundOnLeave(ITUWidget* widget, char* param);

//SettingVoice
extern bool SettingVoiceRecVolumeMeterOnChanged(ITUWidget* widget, char* param);
extern bool SettingVoicePlayVolumeMeterOnChanged(ITUWidget* widget, char* param);
extern bool SettingVoiceOnEnter(ITUWidget* widget, char* param);
extern bool SettingVoiceOnLeave(ITUWidget* widget, char* param);
extern bool SettingVoiceNewMobileButtonOnPress(ITUWidget* widget, char* param);

//SettingVoiceKeyboard
extern bool SettingVoiceKeyboardEnterButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardChsButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingVoiceKeyboardOnEnter(ITUWidget* widget, char* param);

//SettingVoiceQRCode
extern bool SettingVoiceQRCodeOnEnter(ITUWidget* widget, char* param);
extern bool SettingVoiceQRCodeOnTimer(ITUWidget* widget, char* param);

//SettingVoiceDeleteUser
extern bool SettingVoiceDeleteUserOnEnter(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserOnLeave(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserScrollIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserScrollIconListBoxOnSelect(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserScrollListBoxOnSelect(ITUWidget* widget, char* param);
extern bool SettingVoiceDeleteUserButtonOnPress(ITUWidget* widget, char* param);

//SettingDisturb
extern bool SettingDisturbOnEnter(ITUWidget* widget, char* param);
extern bool SettingDisturbOnLeave(ITUWidget* widget, char* param);
extern bool SettingDisturbNormalRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbMuteRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbAllRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbTimeRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbHangUpRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbVoiceMemoRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbMessageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbTransferRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingDisturbTimeStartHourWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingDisturbTimeStartMinuteWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingDisturbTimeEndHourWheelOnChanged(ITUWidget* widget, char* param);
extern bool SettingDisturbTimeEndMinuteWheelOnChanged(ITUWidget* widget, char* param);

//SettingLang
extern bool SettingLangOnEnter(ITUWidget* widget, char* param);
extern bool SettingLangOnLeave(ITUWidget* widget, char* param);
extern bool SettingLangChtRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingLangChsRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingLangEngRadioBoxOnPress(ITUWidget* widget, char* param);

//SettingNetwork
extern bool SettingNetworkIPAssignRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkOnEnter(ITUWidget* widget, char* param);
extern bool SettingNetworkOnLeave(ITUWidget* widget, char* param);
extern bool SettingNetworkIPAddrButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkNetmaskButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkDNSButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkMacButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkGatewayButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkIPInputNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkIPInputBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkIPInputClearButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkIPInputConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingNetworkMacInputConfirmButtonOnPress(ITUWidget* widget, char* param);

//SettingWiFiSsid
extern bool SettingWiFiSsidOnEnter(ITUWidget* widget, char* param);
extern bool SettingWiFiSsidOnLeave(ITUWidget* widget, char* param);
extern bool SettingWiFiSsidNameScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingWiFiSsidStatusScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingWiFiSsidSignalScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingWiFiSsidScrollListBoxOnSelect(ITUWidget* widget, char* param);

//SettingWiFiPassword
extern bool SettingWiFiPasswordEnterButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordEnUpperButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordChsButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordPageUpButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordPageDownButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordChsCharButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiPasswordOnEnter(ITUWidget* widget, char* param);

//SettingWiFiNetwork
extern bool SettingWiFiNetworkIPAssignRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkIPAddrButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkNetmaskButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkDNSButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkIPInputBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkIPInputClearButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkIPInputConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkOnEnter(ITUWidget* widget, char* param);
extern bool SettingWiFiNetworkOnLeave(ITUWidget* widget, char* param);

//SettingSensor
extern bool SettingSensorOffRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSensorOnRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingSensorOnEnter(ITUWidget* widget, char* param);
extern bool SettingSensorOnLeave(ITUWidget* widget, char* param);

//SettingTerminal
extern bool SettingTerminalBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalOnEnter(ITUWidget* widget, char* param);

//SettingDoorCamera
extern bool SettingDoorCameraPrevButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraNextButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnTimer(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnEnter(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnLeave(ITUWidget* widget, char* param);

//SettingMisc
extern bool SettingMiscOnEnter(ITUWidget* widget, char* param);
extern bool SettingMiscOnLeave(ITUWidget* widget, char* param);
extern bool SettingMiscConfirmButtonOnPress(ITUWidget* widget, char* param);

//SettingMiscWarn
extern bool SettingMiscWarnTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool SettingMiscWarnOnEnter(ITUWidget* widget, char* param);
extern bool SettingMiscWarnOnLeave(ITUWidget* widget, char* param);

//SettingMiscDoorCamera
extern bool SettingMiscDoorCameraRoomAddrBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingMiscDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingMiscDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingMiscDoorCameraOnEnter(ITUWidget* widget, char* param);

//SettingSysInfo
extern bool SettingSysInfoOnEnter(ITUWidget* widget, char* param);
extern bool SettingSystemItemScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool SettingSystemMessageScrollListBoxOnLoad(ITUWidget* widget, char* param);

//AudioPlayer
extern bool AudioPlayerOnEnter(ITUWidget* widget, char* param);
extern bool AudioPlayerOnLeave(ITUWidget* widget, char* param);
extern bool AudioPlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool AudioPlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool AudioPlayerRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool AudioPlayerVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool AudioPlayerOnTimer(ITUWidget* widget, char* param);

//VideoPlayer
extern bool VideoPlayerOnEnter(ITUWidget* widget, char* param);
extern bool VideoPlayerOnLeave(ITUWidget* widget, char* param);
extern bool VideoPlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoPlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool VideoPlayerRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoPlayerVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool VideoPlayerOnTimer(ITUWidget* widget, char* param);
extern bool VideoPlayerPauseButtonOnPress(ITUWidget* widget, char* param);

//VideoView
extern bool VideoViewOnEnter(ITUWidget* widget, char* param);
extern bool VideoViewOnLeave(ITUWidget* widget, char* param);
extern bool VideoViewSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoViewUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool VideoViewRepeatButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewRandomCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoViewLastButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewPlayCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool VideoViewNextButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoViewVolTrackBarOnChanged(ITUWidget* widget, char* param);
extern bool VideoViewOnTimer(ITUWidget* widget, char* param);
extern bool VideoViewViewButtonOnPress(ITUWidget* widget, char* param);

//ImagePlayer
extern bool ImagePlayerOnEnter(ITUWidget* widget, char* param);
extern bool ImagePlayerOnLeave(ITUWidget* widget, char* param);
extern bool ImagePlayerSDInsertedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerUsbInsertedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImagePlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param);
extern bool ImagePlayerLastButtonOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerNextButtonOnPress(ITUWidget* widget, char* param);
extern bool ImagePlayerOnTimer(ITUWidget* widget, char* param);

//ImageView
extern bool ImageViewOnEnter(ITUWidget* widget, char* param);
extern bool ImageViewOnLeave(ITUWidget* widget, char* param);
extern bool ImageViewSDRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImageViewUsbRemovedOnCustom(ITUWidget* widget, char* param);
extern bool ImageViewLastButtonOnPress(ITUWidget* widget, char* param);
extern bool ImageViewNextButtonOnPress(ITUWidget* widget, char* param);
extern bool ImageViewOnTimer(ITUWidget* widget, char* param);
extern bool ImageViewViewButtonOnPress(ITUWidget* widget, char* param);

//VideoRecord
extern bool VideoRecordHangUpButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoRecordOnTimer(ITUWidget* widget, char* param);
extern bool VideoRecordOnEnter(ITUWidget* widget, char* param);
extern bool VideoRecordOnLeave(ITUWidget* widget, char* param);

ITUActionFunction actionFunctions[] =
{
    //Logo
    "LogoOnEnter", LogoOnEnter,
    //TouchCalibration
	"TouchCalibrationCheckBoxOnPress", TouchCalibrationCheckBoxOnPress,
	"TouchCalibrationOnEnter", TouchCalibrationOnEnter,
	"TouchCalibrationOnLeave", TouchCalibrationOnLeave,
	//Screansaver
	"ScreensaverOnEnter", ScreensaverOnEnter,
    //Upgrade
	"UpgradeOnTimer", UpgradeOnTimer,
	"UpgradeOnEnter", UpgradeOnEnter,
	//Main
	"MainSetDoNotDisturbModeOnCustom", MainSetDoNotDisturbModeOnCustom,
    "MainNotifyMissedCallsOnCustom", MainNotifyMissedCallsOnCustom,
	"MainNotifyMessagesOnCustom", MainNotifyMessagesOnCustom,
	"MainNotifyGuardOnCustom", MainNotifyGuardOnCustom,
	"MainNotifyNetworkOnCustom", MainNotifyNetworkOnCustom,
	"MainNotifyCallIncomingOnCustom", MainNotifyCallIncomingOnCustom,
    "MainNotifyVoiceMemoIncomingOnCustom", MainNotifyVoiceMemoIncomingOnCustom,
    //MainMenu
	"mainMenuCoverFlowOnChanged", mainMenuCoverFlowOnChanged,
    "MainMenuOnEnter", MainMenuOnEnter,
    "MainMenuOnLeave", MainMenuOnLeave,
    "MainMenuColorPickerOnSelection", MainMenuColorPickerOnSelection,
    "MainMenuWallpaperButtonOnPress", MainMenuWallpaperButtonOnPress,
    "MainMenuResetButtonOnPress", MainMenuResetButtonOnPress,
    "MainMenuSaveButtonOnPress", MainMenuSaveButtonOnPress,
    "MainMenuCancelButtonOnPress", MainMenuCancelButtonOnPress,
    "MainMenuOnTimer", MainMenuOnTimer,
    "MainMenuElevatorDownButtonOnPress", MainMenuElevatorDownButtonOnPress,
    "MainMenuElevatorUpButtonOnPress", MainMenuElevatorUpButtonOnPress,
    //Calendar
    "CalendarOnEnter", CalendarOnEnter,
    "CalendarConfirmButtonOnPress", CalendarConfirmButtonOnPress,
    "CalendarWheelOnChanged", CalendarWheelOnChanged,
    "CalendarCoverFlowOnChanged", CalendarCoverFlowOnChanged,
    "CalendarBackButtonOnPress", CalendarBackButtonOnPress,
    //Clock
    "ClockOnEnter", ClockOnEnter,
    "ClockOnTimer", ClockOnTimer,
    "ClockConfirmButtonOnPress", ClockConfirmButtonOnPress,
    //Weather
    "WeatherCurrTemperatureButtonOnMouseLongPress", WeatherCurrTemperatureButtonOnMouseLongPress,
    "WeatherOnTimer", WeatherOnTimer,
    "WeatherOnEnter", WeatherOnEnter,
    "WeatherOnLeave", WeatherOnLeave,
    //Camera
	"CameraScrollListBoxOnLoad", CameraScrollListBoxOnLoad,
	"CameraScrollListOnSelection", CameraScrollListOnSelection,
	"CameraScreenButtonOnPress", CameraScreenButtonOnPress,
	"CameraOnEnter", CameraOnEnter,
	"CameraOnLeave", CameraOnLeave,
    "CameraOnTimer", CameraOnTimer,
    "CameraRecordButtonOnPress", CameraRecordButtonOnPress,
    "CameraStopRecordButtonOnPress", CameraStopRecordButtonOnPress,
    "CameraUnlockButtonOnPress", CameraUnlockButtonOnPress,
    "CameraSnapshotButtonOnPress", CameraSnapshotButtonOnPress,
    "CameraDisconnectButtonOnPress", CameraDisconnectButtonOnPress,
    //CameraOutdoor
	"CameraOutdoorOnEnter", CameraOutdoorOnEnter,
	"CameraOutdoorOnLeave", CameraOutdoorOnLeave,
    "CameraOutdoorOnTimer", CameraOutdoorOnTimer,
	"CameraOutdoorButtonOnPress", CameraOutdoorButtonOnPress,
    "CameraOutdoorRecordButtonOnPress", CameraOutdoorRecordButtonOnPress,
    "CameraOutdoorStopRecordButtonOnPress", CameraOutdoorStopRecordButtonOnPress,
    "CameraOutdoorUnlockButtonOnPress", CameraOutdoorUnlockButtonOnPress,
    "CameraOutdoorSnapshotButtonOnPress", CameraOutdoorSnapshotButtonOnPress,
    //CameraView
    "CameraViewOnEnter", CameraViewOnEnter,
    "CameraViewOnLeave", CameraViewOnLeave,
	"CameraViewScreenButtonOnPress", CameraViewScreenButtonOnPress,
    "CameraViewRecordButtonOnPress", CameraViewRecordButtonOnPress,
    "CameraViewStopRecordButtonOnPress", CameraViewStopRecordButtonOnPress,
    "CameraViewUnlockButtonOnPress", CameraViewUnlockButtonOnPress,
    "CameraViewSnapshotButtonOnPress", CameraViewSnapshotButtonOnPress,
    "CameraViewDisconnectButtonOnPress", CameraViewDisconnectButtonOnPress,
    //House
    "HouseModeIndoorRadioBoxOnPress", HouseModeIndoorRadioBoxOnPress,
    "HouseModeOutdoorRadioBoxOnPress", HouseModeOutdoorRadioBoxOnPress,
    "HouseModeSleepRadioBoxOnPress", HouseModeSleepRadioBoxOnPress,
    "HouseLightScrollListBoxOnLoad", HouseLightScrollListBoxOnLoad,
    "HouseLightScrollIconListBoxOnLoad", HouseLightScrollIconListBoxOnLoad,
    "HouseLightScrollIconListBoxOnSelection", HouseLightScrollIconListBoxOnSelection,
    "HouseAirConditionerScrollListBoxOnLoad", HouseAirConditionerScrollListBoxOnLoad,
    "HouseAirConditionerScrollIconListBoxOnLoad", HouseAirConditionerScrollIconListBoxOnLoad,
    "HouseAirConditionerScrollIconListBoxOnSelection", HouseAirConditionerScrollIconListBoxOnSelection,
    "HouseCurtainScrollListBoxOnLoad", HouseCurtainScrollListBoxOnLoad,
    "HouseCurtainCloseScrollIconListBoxOnLoad", HouseCurtainCloseScrollIconListBoxOnLoad,
    "HouseCurtainCloseScrollIconListBoxOnSelection", HouseCurtainCloseScrollIconListBoxOnSelection,
    "HouseCurtainPauseScrollIconListBoxOnLoad", HouseCurtainPauseScrollIconListBoxOnLoad,
    "HouseCurtainPauseScrollIconListBoxOnSelection", HouseCurtainPauseScrollIconListBoxOnSelection,
    "HouseCurtainOpenScrollIconListBoxOnLoad", HouseCurtainOpenScrollIconListBoxOnLoad,
    "HouseCurtainOpenScrollIconListBoxOnSelection", HouseCurtainOpenScrollIconListBoxOnSelection,
    "HousePlugScrollListBoxOnLoad", HousePlugScrollListBoxOnLoad,
    "HousePlugScrollIconListBoxOnLoad", HousePlugScrollIconListBoxOnLoad,
    "HousePlugScrollIconListBoxOnSelection", HousePlugScrollIconListBoxOnSelection,
    "HouseTVScrollListBoxOnLoad", HouseTVScrollListBoxOnLoad,
    "HouseTVScrollIconListBoxOnLoad", HouseTVScrollIconListBoxOnLoad,
    "HouseTVScrollIconListBoxOnSelection", HouseTVScrollIconListBoxOnSelection,
    "HouseModeScrollListBoxOnLoad", HouseModeScrollListBoxOnLoad,
    "HouseModeScrollIconListBoxOnLoad", HouseModeScrollIconListBoxOnLoad,
    "HouseModeScrollIconListBoxOnSelection", HouseModeScrollIconListBoxOnSelection,
    "HouseOnEnter", HouseOnEnter,
    "HouseOnLeave", HouseOnLeave,
    //AirConditioner
    "AirConditionerOnEnter", AirConditionerOnEnter,
    "AirConditionerOnLeave", AirConditionerOnLeave,
    "AirConditionerOnTimer", AirConditionerOnTimer,
    "AirConditionerPowerButtonOnPress", AirConditionerPowerButtonOnPress,
    "AirConditionerAutoRadioBoxOnPress", AirConditionerAutoRadioBoxOnPress,
    "AirConditionerCoolRadioBoxOnPress", AirConditionerCoolRadioBoxOnPress,
    "AirConditionerDryRadioBoxOnPress", AirConditionerDryRadioBoxOnPress,
    "AirConditionerFanRadioBoxOnPress", AirConditionerFanRadioBoxOnPress,
    "AirConditionerHeatRadioBoxOnPress", AirConditionerHeatRadioBoxOnPress,
    "AirConditionerWindSlowRadioBoxOnPress",AirConditionerWindSlowRadioBoxOnPress,
    "AirConditionerWindNormalRadioBoxOnPress", AirConditionerWindNormalRadioBoxOnPress,
    "AirConditionerWindFastRadioBoxOnPress", AirConditionerWindFastRadioBoxOnPress,
    "AirConditionerWindAutoRadioBoxOnPress", AirConditionerWindAutoRadioBoxOnPress,
    "AirConditionerCurrTempBackgroundButtonOnMouseLongPress", AirConditionerCurrTempBackgroundButtonOnMouseLongPress,
    "AirConditionerTempCTrackBarOnChanged", AirConditionerTempCTrackBarOnChanged,
    "AirConditionerTempFTrackBarOnChanged", AirConditionerTempFTrackBarOnChanged,
    //VoiceMemo
    "VoiceMemoOnEnter", VoiceMemoOnEnter,
    "VoiceMemoOnLeave", VoiceMemoOnLeave,
    "VoiceMemoOnTimer", VoiceMemoOnTimer,
    "VoiceMemoScrollIconListBoxOnLoad", VoiceMemoScrollIconListBoxOnLoad,
    "VoiceMemoNameScrollListBoxOnLoad", VoiceMemoNameScrollListBoxOnLoad,
    "VoiceMemoTimeScrollListBoxOnLoad", VoiceMemoTimeScrollListBoxOnLoad,
    "VoiceMemoScrollListBoxOnSelection", VoiceMemoScrollListBoxOnSelection,
    "VoiceMemoScrollListBoxOnMouseLongPress", VoiceMemoScrollListBoxOnMouseLongPress,
    "VoiceMemoRecCheckBoxOnPress", VoiceMemoRecCheckBoxOnPress,
    "VoiceMemoDeleteButtonOnPress", VoiceMemoDeleteButtonOnPress,
    //Phone
    "PhoneNumberButtonOnPress", PhoneNumberButtonOnPress,
    "PhoneCallButtonOnPress", PhoneCallButtonOnPress,
    //PhoneResident
    "PhoneResidentOnEnter", PhoneResidentOnEnter,
    "PhoneResidentOnLeave", PhoneResidentOnLeave,
    "PhoneBackButtonOnPress", PhoneBackButtonOnPress,
    "PhoneLogScrollIconListBoxOnLoad", PhoneLogScrollIconListBoxOnLoad,
    "PhoneLogScrollListBoxOnLoad", PhoneLogScrollListBoxOnLoad,
    "PhoneLogScrollListBoxOnSelection", PhoneLogScrollListBoxOnSelection,
    "PhoneLogAddButtonOnPress", PhoneLogAddButtonOnPress,
    "PhoneLogBlackButtonOnPress", PhoneLogBlackButtonOnPress,
    "PhoneLogWhiteButtonOnPress", PhoneLogWhiteButtonOnPress,
    "PhoneLogDeleteButtonOnPress", PhoneLogDeleteButtonOnPress,
    "PhoneLogClearButtonOnPress", PhoneLogClearButtonOnPress,
    "PhoneResidentRoomBackgroundButtonOnMouseLongPress", PhoneResidentRoomBackgroundButtonOnMouseLongPress,
    "PhoneLogScrollListBoxOnMouseLongPress", PhoneLogScrollListBoxOnMouseLongPress,
    //PhoneAdmin
    "PhoneAdminOnEnter", PhoneAdminOnEnter,
    "PhoneAdminScrollListBoxOnLoad", PhoneAdminScrollListBoxOnLoad,
    //PhoneMisc
    "PhoneMiscOnEnter", PhoneMiscOnEnter,
    "PhoneMiscScrollListBoxOnLoad", PhoneMiscScrollListBoxOnLoad,
    //PhoneContact
    "PhoneContactOnEnter", PhoneContactOnEnter,
    "PhoneContactNameScrollListBoxOnLoad", PhoneContactNameScrollListBoxOnLoad,
    "PhoneContactCommentScrollListBoxOnLoad", PhoneContactCommentScrollListBoxOnLoad,
    "PhoneContactScrollListBoxOnMouseLongPress", PhoneContactScrollListBoxOnMouseLongPress,
    "PhoneContactAddButtonOnPress", PhoneContactAddButtonOnPress,
    "PhoneContactBlackButtonOnPress", PhoneContactBlackButtonOnPress,
    "PhoneContactWhiteButtonOnPress", PhoneContactWhiteButtonOnPress,
    "PhoneContactDeleteButtonOnPress", PhoneContactDeleteButtonOnPress,
    "PhoneContactClearButtonOnPress", PhoneContactClearButtonOnPress,
    "PhoneContactAddBackButtonOnPress", PhoneContactAddBackButtonOnPress,
    "PhoneContactAddConfirmButtonOnPress", PhoneContactAddConfirmButtonOnPress,
    //PhoneContactKeyboard
    "PhoneContactKeyboardEnterButtonOnPress", PhoneContactKeyboardEnterButtonOnPress,
    "PhoneContactKeyboardBackButtonOnPress", PhoneContactKeyboardBackButtonOnPress,
    "PhoneContactKeyboardEnUpperButtonOnPress", PhoneContactKeyboardEnUpperButtonOnPress,
    "PhoneContactKeyboardChsButtonOnPress", PhoneContactKeyboardChsButtonOnPress,
    "PhoneContactKeyboardPageUpButtonOnPress", PhoneContactKeyboardPageUpButtonOnPress,
    "PhoneContactKeyboardPageDownButtonOnPress", PhoneContactKeyboardPageDownButtonOnPress,
    "PhoneContactKeyboardChsCharButtonOnPress", PhoneContactKeyboardChsCharButtonOnPress,
    "PhoneContactKeyboardOnEnter", PhoneContactKeyboardOnEnter,
	//Calling
    "CallingVolTrackBarOnChanged", CallingVolTrackBarOnChanged,
    "CallingRecordButtonOnPress", CallingRecordButtonOnPress,
    "CallingStopRecordButtonOnPress", CallingStopRecordButtonOnPress,
    "CallingSnapshotButtonOnPress", CallingSnapshotButtonOnPress,
    "CallingAnswerButtonOnPress", CallingAnswerButtonOnPress,
    "CallingUnlockButtonOnPress", CallingUnlockButtonOnPress,
	"CallingHangUpButtonOnPress", CallingHangUpButtonOnPress,
    "CallingErrorConfirmButtonOnPress", CallingErrorConfirmButtonOnPress,
    "CallingWaitingCancelButtonOnPress", CallingWaitingCancelButtonOnPress,
    "CallingWaitingConfirmButtonOnPress", CallingWaitingConfirmButtonOnPress,
    "CallingConnectedOnCustom", CallingConnectedOnCustom,
	"CallingErrorOnCustom", CallingErrorOnCustom,
    "CallingIncomingOnCustom", CallingIncomingOnCustom,
    "CallingOnTimer", CallingOnTimer,
    "CallingOnEnter", CallingOnEnter,
    "CallingOnLeave", CallingOnLeave,
	//Called
    "CalledVolTrackBarOnChanged", CalledVolTrackBarOnChanged,
    "CalledRecordButtonOnPress", CalledRecordButtonOnPress,
    "CalledStopRecordButtonOnPress", CalledStopRecordButtonOnPress,
    "CalledSnapshotButtonOnPress", CalledSnapshotButtonOnPress,
	"CalledAnswerButtonOnPress", CalledAnswerButtonOnPress,
	"CalledUnlockButtonOnPress", CalledUnlockButtonOnPress,
    "CalledHangUpButtonOnPress", CalledHangUpButtonOnPress,
    "CalledWaitingCancelButtonOnPress", CalledWaitingCancelButtonOnPress,
    "CalledWaitingConfirmButtonOnPress", CalledWaitingConfirmButtonOnPress,
    "CalledIncomingOnCustom", CalledIncomingOnCustom,
    "CalledConnectedOnCustom", CalledConnectedOnCustom,
    "CalledOnTimer", CalledOnTimer,
    "CalledOnEnter", CalledOnEnter,
    "CalledOnLeave", CalledOnLeave,
    "CalledSourceButtonOnPress", CalledSourceButtonOnPress,
    "CalledSourceScrollListBoxOnLoad", CalledSourceScrollListBoxOnLoad,
    "CalledSourceScrollListBoxOnSelection", CalledSourceScrollListBoxOnSelection,
    //Message
	"MessageNotifyVideoMessageOnCustom", MessageNotifyVideoMessageOnCustom,
    "MessageNotifyTextMessageOnCustom", MessageNotifyTextMessageOnCustom,
    //MessageVideo
    "MessageVideoTopicScrollListBoxOnLoad", MessageVideoTopicScrollListBoxOnLoad,
    "MessageVideoTimeScrollListBoxOnLoad", MessageVideoTimeScrollListBoxOnLoad,
    "MessageVideoScrollListBoxOnSelection", MessageVideoScrollListBoxOnSelection,
	"MessageVideoDeleteButtonOnPress", MessageVideoDeleteButtonOnPress,
    "MessageVideoOnEnter", MessageVideoOnEnter,
    "MessageVideoOnLeave", MessageVideoOnLeave,
    //MessageVideoView
	"MessageVideoViewOnEnter", MessageVideoViewOnEnter,
	"MessageVideoViewOnLeave", MessageVideoViewOnLeave,
    "MessageVideoViewOnTimer", MessageVideoViewOnTimer,
    "MessageVideoViewVolTrackBarOnChanged", MessageVideoViewVolTrackBarOnChanged,
    //MessageRecord
	"MessageRecordScrollIconListBoxOnLoad", MessageRecordScrollIconListBoxOnLoad,
    "MessageRecordTopicScrollListBoxOnLoad", MessageRecordTopicScrollListBoxOnLoad,
	"MessageRecordTimeScrollListBoxOnLoad", MessageRecordTimeScrollListBoxOnLoad,
    "MessageRecordScrollListBoxOnSelection", MessageRecordScrollListBoxOnSelection,
	"MessageRecordDeleteButtonOnPress", MessageRecordDeleteButtonOnPress,
	"MessageRecordOnEnter", MessageRecordOnEnter,
    //MessageRecordView
    "MessageRecordViewOnEnter", MessageRecordViewOnEnter,
	"MessageRecordViewOnLeave", MessageRecordViewOnLeave,
    "MessageRecordViewOnTimer", MessageRecordViewOnTimer,
    //MessageService
	"MessageServiceTopicScrollListBoxOnLoad", MessageServiceTopicScrollListBoxOnLoad,
	"MessageServiceTimeScrollListBoxOnLoad", MessageServiceTimeScrollListBoxOnLoad,
    "MessageServiceScrollListBoxOnSelection", MessageServiceScrollListBoxOnSelection,
	"MessageServiceDeleteButtonOnPress", MessageServiceDeleteButtonOnPress,
	"MessageServiceOnEnter", MessageServiceOnEnter,
	"MessageServiceOnLeave", MessageServiceOnLeave,
    //MessageServiceView
	"MessageServiceViewOnEnter", MessageServiceViewOnEnter, 
	"MessageServiceViewOnLeave", MessageServiceViewOnLeave, 
    "MessageServiceViewOnTimer", MessageServiceViewOnTimer,
    //Guard
    "GuardIndoorRadioBoxOnPress", GuardIndoorRadioBoxOnPress,
    "GuardOutdoorRadioBoxOnPress", GuardOutdoorRadioBoxOnPress,
    "GuardSleepRadioBoxOnPress", GuardSleepRadioBoxOnPress,
    //GuardMain
    "GuardMainOnEnter", GuardMainOnEnter,
    "GuardMainOnLeave", GuardMainOnLeave,
    //GuardCheck
    "GuardCheckScrollIconListBoxOnLoad", GuardCheckScrollIconListBoxOnLoad,
    "GuardCheckSensorScrollListBoxOnLoad", GuardCheckSensorScrollListBoxOnLoad,
    "GuardCheckStatusScrollListBoxOnLoad", GuardCheckStatusScrollListBoxOnLoad,
    "GuardCheckOnEnter", GuardCheckOnEnter,
    //GuardUnguard
    "GuardUnguardConfirmButtonOnPress", GuardUnguardConfirmButtonOnPress,
    "GuardUnguardOnEnter", GuardUnguardOnEnter,
    //GuardLog
    "GuardLogScrollIconListBoxOnLoad", GuardLogScrollIconListBoxOnLoad,
    "GuardLogSensorScrollListBoxOnLoad", GuardLogSensorScrollListBoxOnLoad,
    "GuardLogTypeScrollListBoxOnLoad", GuardLogTypeScrollListBoxOnLoad,
    "GuardLogTimeScrollListBoxOnLoad", GuardLogTimeScrollListBoxOnLoad,
    "GuardLogClearButtonOnPress", GuardLogClearButtonOnPress,
    "GuardLogOnEnter", GuardLogOnEnter,
    //GuardStatus
    "GuardStatusScrollIconListBoxOnLoad", GuardStatusScrollIconListBoxOnLoad,
    "GuardStatusSensorScrollListBoxOnLoad", GuardStatusSensorScrollListBoxOnLoad,
    "GuardStatusTypeScrollListBoxOnLoad", GuardStatusTypeScrollListBoxOnLoad,
    "GuardStatusTimeScrollListBoxOnLoad", GuardStatusTimeScrollListBoxOnLoad,
    "GuardStatusOnEnter", GuardStatusOnEnter,
    //GuardPassword
    "GuardPasswordOldDeleteButtonOnPress", GuardPasswordOldDeleteButtonOnPress,
    "GuardPasswordNewDeleteButtonOnPress", GuardPasswordNewDeleteButtonOnPress,
    "GuardPasswordConfirmDeleteButtonOnPress", GuardPasswordConfirmDeleteButtonOnPress,
    "GuardPasswordNumberButtonOnPress", GuardPasswordNumberButtonOnPress,
    "GuardPasswordClearButtonOnPress", GuardPasswordClearButtonOnPress,
    "GuardPasswordSaveButtonOnPress", GuardPasswordSaveButtonOnPress,
    "GuardPasswordOnEnter", GuardPasswordOnEnter,
    //GuardUser
    "GuardUserConfirmButtonOnPress", GuardUserConfirmButtonOnPress,
    "GuardUserOnEnter", GuardUserOnEnter,
    //GuardUserSettingDetail
    "GuardUserSettingDetailScrollIconListBoxOnLoad", GuardUserSettingDetailScrollIconListBoxOnLoad,
    "GuardUserSettingDetailSensorScrollListBoxOnLoad", GuardUserSettingDetailSensorScrollListBoxOnLoad,
    "GuardUserSettingDetailStatusScrollListBoxOnLoad", GuardUserSettingDetailStatusScrollListBoxOnLoad,
    "GuardUserSettingDetailStatusScrollListBoxOnSelect", GuardUserSettingDetailStatusScrollListBoxOnSelect,
    "GuardUserSettingDetailOnEnter", GuardUserSettingDetailOnEnter,
    "GuardUserSettingDetailOnLeave", GuardUserSettingDetailOnLeave,
    //Setting
    "SettingIconScrollListBoxOnLoad", SettingIconScrollListBoxOnLoad,
    "SettingScrollListBoxOnLoad", SettingScrollListBoxOnLoad,
    "SettingScrollListBoxOnSelection", SettingScrollListBoxOnSelection,
    "SettingPasswordConfirmButtonOnPress", SettingPasswordConfirmButtonOnPress,
    //SettingDisplay
    "SettingDisplayBrightnessTrackBarOnChanged", SettingDisplayBrightnessTrackBarOnChanged, 
    "SettingDisplayScreensaverTimeRadioBoxOnPress", SettingDisplayScreensaverTimeRadioBoxOnPress, 
    "SettingDisplayScreensaverTypeRadioBoxOnPress", SettingDisplayScreensaverTypeRadioBoxOnPress, 
	"SettingDisplayOnEnter", SettingDisplayOnEnter, 
	"SettingDisplayOnLeave", SettingDisplayOnLeave,     
    //SettingSound
    "SettingSoundKeyVolumeMeterOnChanged", SettingSoundKeyVolumeMeterOnChanged, 
    "SettingSoundRingVolumeMeterOnChanged", SettingSoundRingVolumeMeterOnChanged, 
    "SettingSoundKeyRadioBoxOnPress", SettingSoundKeyRadioBoxOnPress, 
    "SettingSoundRingRadioBoxOnPress", SettingSoundRingRadioBoxOnPress, 
	"SettingSoundOnEnter", SettingSoundOnEnter, 
	"SettingSoundOnLeave", SettingSoundOnLeave,     
    //SettingVoice
    "SettingVoiceRecVolumeMeterOnChanged", SettingVoiceRecVolumeMeterOnChanged, 
    "SettingVoicePlayVolumeMeterOnChanged", SettingVoicePlayVolumeMeterOnChanged, 
	"SettingVoiceOnEnter", SettingVoiceOnEnter, 
	"SettingVoiceOnLeave", SettingVoiceOnLeave,         
    "SettingVoiceNewMobileButtonOnPress", SettingVoiceNewMobileButtonOnPress,
    //SettingVoiceKeyboard
    "SettingVoiceKeyboardEnterButtonOnPress", SettingVoiceKeyboardEnterButtonOnPress,
    "SettingVoiceKeyboardBackButtonOnPress", SettingVoiceKeyboardBackButtonOnPress,
    "SettingVoiceKeyboardEnUpperButtonOnPress", SettingVoiceKeyboardEnUpperButtonOnPress,
    "SettingVoiceKeyboardChsButtonOnPress", SettingVoiceKeyboardChsButtonOnPress,
    "SettingVoiceKeyboardPageUpButtonOnPress", SettingVoiceKeyboardPageUpButtonOnPress,
    "SettingVoiceKeyboardPageDownButtonOnPress", SettingVoiceKeyboardPageDownButtonOnPress,
    "SettingVoiceKeyboardChsCharButtonOnPress", SettingVoiceKeyboardChsCharButtonOnPress,
    "SettingVoiceKeyboardOnEnter", SettingVoiceKeyboardOnEnter,
    //SettingVoiceQRCode
    "SettingVoiceQRCodeOnEnter", SettingVoiceQRCodeOnEnter,
    "SettingVoiceQRCodeOnTimer", SettingVoiceQRCodeOnTimer,
    //SettingVoiceDeleteUser
    "SettingVoiceDeleteUserOnEnter", SettingVoiceDeleteUserOnEnter,
    "SettingVoiceDeleteUserOnLeave", SettingVoiceDeleteUserOnLeave,
    "SettingVoiceDeleteUserScrollIconListBoxOnLoad", SettingVoiceDeleteUserScrollIconListBoxOnLoad,
    "SettingVoiceDeleteUserScrollListBoxOnLoad", SettingVoiceDeleteUserScrollListBoxOnLoad,
    "SettingVoiceDeleteUserScrollIconListBoxOnSelect", SettingVoiceDeleteUserScrollIconListBoxOnSelect,
    "SettingVoiceDeleteUserScrollListBoxOnSelect", SettingVoiceDeleteUserScrollListBoxOnSelect,
    "SettingVoiceDeleteUserButtonOnPress", SettingVoiceDeleteUserButtonOnPress,
    //SettingDisturb
    "SettingDisturbOnEnter", SettingDisturbOnEnter,
    "SettingDisturbOnLeave", SettingDisturbOnLeave,
    "SettingDisturbNormalRadioBoxOnPress", SettingDisturbNormalRadioBoxOnPress,
    "SettingDisturbMuteRadioBoxOnPress", SettingDisturbMuteRadioBoxOnPress,
    "SettingDisturbAllRadioBoxOnPress", SettingDisturbAllRadioBoxOnPress,
    "SettingDisturbTimeRadioBoxOnPress", SettingDisturbTimeRadioBoxOnPress,
    "SettingDisturbHangUpRadioBoxOnPress", SettingDisturbHangUpRadioBoxOnPress,
    "SettingDisturbVoiceMemoRadioBoxOnPress", SettingDisturbVoiceMemoRadioBoxOnPress,
    "SettingDisturbMessageRadioBoxOnPress", SettingDisturbMessageRadioBoxOnPress,
    "SettingDisturbTransferRadioBoxOnPress", SettingDisturbTransferRadioBoxOnPress,
    "SettingDisturbTimeStartHourWheelOnChanged", SettingDisturbTimeStartHourWheelOnChanged,
    "SettingDisturbTimeStartMinuteWheelOnChanged", SettingDisturbTimeStartMinuteWheelOnChanged,
    "SettingDisturbTimeEndHourWheelOnChanged", SettingDisturbTimeEndHourWheelOnChanged,
    "SettingDisturbTimeEndMinuteWheelOnChanged", SettingDisturbTimeEndMinuteWheelOnChanged,
    //SettingLang
    "SettingLangOnEnter", SettingLangOnEnter,         
    "SettingLangOnLeave", SettingLangOnLeave,         
    "SettingLangChtRadioBoxOnPress", SettingLangChtRadioBoxOnPress,         
    "SettingLangChsRadioBoxOnPress", SettingLangChsRadioBoxOnPress,         
    "SettingLangEngRadioBoxOnPress", SettingLangEngRadioBoxOnPress,         
    //SettingNetwork
    "SettingNetworkIPAssignRadioBoxOnPress", SettingNetworkIPAssignRadioBoxOnPress, 
	"SettingNetworkOnEnter", SettingNetworkOnEnter, 
	"SettingNetworkOnLeave", SettingNetworkOnLeave,         
    "SettingNetworkIPAddrButtonOnPress", SettingNetworkIPAddrButtonOnPress,         
    "SettingNetworkNetmaskButtonOnPress", SettingNetworkNetmaskButtonOnPress,         
    "SettingNetworkDNSButtonOnPress", SettingNetworkDNSButtonOnPress,         
    "SettingNetworkMacButtonOnPress", SettingNetworkMacButtonOnPress,         
    "SettingNetworkGatewayButtonOnPress", SettingNetworkGatewayButtonOnPress,         
    "SettingNetworkIPInputNumberButtonOnPress", SettingNetworkIPInputNumberButtonOnPress,         
    "SettingNetworkIPInputBackButtonOnPress", SettingNetworkIPInputBackButtonOnPress,         
    "SettingNetworkIPInputClearButtonOnPress", SettingNetworkIPInputClearButtonOnPress,         
    "SettingNetworkIPInputConfirmButtonOnPress", SettingNetworkIPInputConfirmButtonOnPress,         
    "SettingNetworkMacInputConfirmButtonOnPress", SettingNetworkMacInputConfirmButtonOnPress,         
    //SettingWiFiSsid
    "SettingWiFiSsidOnEnter", SettingWiFiSsidOnEnter,
    "SettingWiFiSsidOnLeave", SettingWiFiSsidOnLeave,
    "SettingWiFiSsidNameScrollListBoxOnLoad", SettingWiFiSsidNameScrollListBoxOnLoad,
    "SettingWiFiSsidStatusScrollListBoxOnLoad", SettingWiFiSsidStatusScrollListBoxOnLoad,
    "SettingWiFiSsidSignalScrollListBoxOnLoad", SettingWiFiSsidSignalScrollListBoxOnLoad,
    "SettingWiFiSsidScrollListBoxOnSelect", SettingWiFiSsidScrollListBoxOnSelect,
    //SettingWiFiPassword
    "SettingWiFiPasswordEnterButtonOnPress", SettingWiFiPasswordEnterButtonOnPress,
    "SettingWiFiPasswordBackButtonOnPress", SettingWiFiPasswordBackButtonOnPress,
    "SettingWiFiPasswordEnUpperButtonOnPress", SettingWiFiPasswordEnUpperButtonOnPress,
    "SettingWiFiPasswordChsButtonOnPress", SettingWiFiPasswordChsButtonOnPress,
    "SettingWiFiPasswordPageUpButtonOnPress", SettingWiFiPasswordPageUpButtonOnPress,
    "SettingWiFiPasswordPageDownButtonOnPress", SettingWiFiPasswordPageDownButtonOnPress,
    "SettingWiFiPasswordChsCharButtonOnPress", SettingWiFiPasswordChsCharButtonOnPress,
    "SettingWiFiPasswordOnEnter", SettingWiFiPasswordOnEnter,
    //SettingWiFiNetwork
    "SettingWiFiNetworkIPAssignRadioBoxOnPress", SettingWiFiNetworkIPAssignRadioBoxOnPress,
    "SettingWiFiNetworkIPAddrButtonOnPress", SettingWiFiNetworkIPAddrButtonOnPress,
    "SettingWiFiNetworkNetmaskButtonOnPress", SettingWiFiNetworkNetmaskButtonOnPress,
    "SettingWiFiNetworkDNSButtonOnPress", SettingWiFiNetworkDNSButtonOnPress,
    "SettingWiFiNetworkIPInputBackButtonOnPress", SettingWiFiNetworkIPInputBackButtonOnPress,
    "SettingWiFiNetworkIPInputClearButtonOnPress", SettingWiFiNetworkIPInputClearButtonOnPress,
    "SettingWiFiNetworkIPInputConfirmButtonOnPress", SettingWiFiNetworkIPInputConfirmButtonOnPress,
    "SettingWiFiNetworkOnEnter", SettingWiFiNetworkOnEnter,
    "SettingWiFiNetworkOnLeave", SettingWiFiNetworkOnLeave,
    //SettingSensor
    "SettingSensorOffRadioBoxOnPress", SettingSensorOffRadioBoxOnPress, 
	"SettingSensorOnRadioBoxOnPress", SettingSensorOnRadioBoxOnPress, 
	"SettingSensorOnEnter", SettingSensorOnEnter,         
    "SettingSensorOnLeave", SettingSensorOnLeave,         
    //SettingTerminal
    "SettingTerminalBackButtonOnPress", SettingTerminalBackButtonOnPress, 
    "SettingTerminalConfirmButtonOnPress", SettingTerminalConfirmButtonOnPress, 
    "SettingTerminalCancelButtonOnPress", SettingTerminalCancelButtonOnPress, 
    "SettingTerminalNumberButtonOnPress", SettingTerminalNumberButtonOnPress, 
    "SettingTerminalOnEnter", SettingTerminalOnEnter, 
    //SettingDoorCamera
    "SettingDoorCameraPrevButtonOnPress", SettingDoorCameraPrevButtonOnPress, 
	"SettingDoorCameraNextButtonOnPress", SettingDoorCameraNextButtonOnPress, 
	"SettingDoorCameraConfirmButtonOnPress", SettingDoorCameraConfirmButtonOnPress,         
    "SettingDoorCameraCancelButtonOnPress", SettingDoorCameraCancelButtonOnPress,         
    "SettingDoorCameraNumberButtonOnPress", SettingDoorCameraNumberButtonOnPress,
    "SettingDoorCameraOnTimer", SettingDoorCameraOnTimer,
    "SettingDoorCameraOnEnter", SettingDoorCameraOnEnter,         
    "SettingDoorCameraOnLeave", SettingDoorCameraOnLeave,         
    //SettingMisc
    "SettingMiscOnEnter", SettingMiscOnEnter,         
    "SettingMiscOnLeave", SettingMiscOnLeave,         
    "SettingMiscConfirmButtonOnPress", SettingMiscConfirmButtonOnPress,         
    //SettingMiscWarn
    "SettingMiscWarnTrackBarOnChanged", SettingMiscWarnTrackBarOnChanged,         
    "SettingMiscWarnOnEnter", SettingMiscWarnOnEnter,  
    "SettingMiscWarnOnLeave", SettingMiscWarnOnLeave,
    //SettingMiscDoorCamera
    "SettingMiscDoorCameraRoomAddrBackButtonOnPress", SettingMiscDoorCameraRoomAddrBackButtonOnPress,         
    "SettingMiscDoorCameraConfirmButtonOnPress", SettingMiscDoorCameraConfirmButtonOnPress,         
    "SettingMiscDoorCameraNumberButtonOnPress", SettingMiscDoorCameraNumberButtonOnPress,         
    "SettingMiscDoorCameraOnEnter", SettingMiscDoorCameraOnEnter,         
    //SettingSysInfo
    "SettingSysInfoOnEnter", SettingSysInfoOnEnter,         
    "SettingSystemItemScrollListBoxOnLoad", SettingSystemItemScrollListBoxOnLoad,
    "SettingSystemMessageScrollListBoxOnLoad", SettingSystemMessageScrollListBoxOnLoad,
    //AudioPlayer
    "AudioPlayerOnEnter", AudioPlayerOnEnter,
    "AudioPlayerOnLeave", AudioPlayerOnLeave,
    "AudioPlayerSDInsertedOnCustom", AudioPlayerSDInsertedOnCustom,
    "AudioPlayerSDRemovedOnCustom", AudioPlayerSDRemovedOnCustom,
    "AudioPlayerUsbInsertedOnCustom", AudioPlayerUsbInsertedOnCustom,
    "AudioPlayerUsbRemovedOnCustom", AudioPlayerUsbRemovedOnCustom,
    "AudioPlayerStorageRadioBoxOnPress", AudioPlayerStorageRadioBoxOnPress,
    "AudioPlayerUsbRemovedOnCustom", AudioPlayerUsbRemovedOnCustom,
    "AudioPlayerStorageRadioBoxOnPress", AudioPlayerStorageRadioBoxOnPress,
    "AudioPlayerStorageTypeCheckBoxOnPress", AudioPlayerStorageTypeCheckBoxOnPress,
    "AudioPlayerScrollMediaFileListBoxOnSelection", AudioPlayerScrollMediaFileListBoxOnSelection,
    "AudioPlayerRepeatButtonOnPress", AudioPlayerRepeatButtonOnPress,
    "AudioPlayerRandomCheckBoxOnPress", AudioPlayerRandomCheckBoxOnPress,
    "AudioPlayerLastButtonOnPress", AudioPlayerLastButtonOnPress,
    "AudioPlayerPlayCheckBoxOnPress", AudioPlayerPlayCheckBoxOnPress,
    "AudioPlayerNextButtonOnPress", AudioPlayerNextButtonOnPress,
    "AudioPlayerVolTrackBarOnChanged", AudioPlayerVolTrackBarOnChanged,
    "AudioPlayerOnTimer", AudioPlayerOnTimer,
    //VideoPlayer
    "VideoPlayerOnEnter", VideoPlayerOnEnter,
    "VideoPlayerOnLeave", VideoPlayerOnLeave,
    "VideoPlayerSDInsertedOnCustom", VideoPlayerSDInsertedOnCustom,
    "VideoPlayerSDRemovedOnCustom", VideoPlayerSDRemovedOnCustom,
    "VideoPlayerUsbInsertedOnCustom", VideoPlayerUsbInsertedOnCustom,
    "VideoPlayerUsbRemovedOnCustom", VideoPlayerUsbRemovedOnCustom,
    "VideoPlayerStorageRadioBoxOnPress", VideoPlayerStorageRadioBoxOnPress,
    "VideoPlayerUsbRemovedOnCustom", VideoPlayerUsbRemovedOnCustom,
    "VideoPlayerStorageRadioBoxOnPress", VideoPlayerStorageRadioBoxOnPress,
    "VideoPlayerStorageTypeCheckBoxOnPress", VideoPlayerStorageTypeCheckBoxOnPress,
    "VideoPlayerScrollMediaFileListBoxOnSelection", VideoPlayerScrollMediaFileListBoxOnSelection,
    "VideoPlayerRepeatButtonOnPress", VideoPlayerRepeatButtonOnPress,
    "VideoPlayerRandomCheckBoxOnPress", VideoPlayerRandomCheckBoxOnPress,
    "VideoPlayerLastButtonOnPress", VideoPlayerLastButtonOnPress,
    "VideoPlayerPlayCheckBoxOnPress", VideoPlayerPlayCheckBoxOnPress,
    "VideoPlayerNextButtonOnPress", VideoPlayerNextButtonOnPress,
    "VideoPlayerVolTrackBarOnChanged", VideoPlayerVolTrackBarOnChanged,
    "VideoPlayerOnTimer", VideoPlayerOnTimer,
	"VideoPlayerPauseButtonOnPress", VideoPlayerPauseButtonOnPress,
    //VideoView
    "VideoViewOnEnter", VideoViewOnEnter,
    "VideoViewOnLeave", VideoViewOnLeave,
    "VideoViewSDRemovedOnCustom", VideoViewSDRemovedOnCustom,
    "VideoViewUsbRemovedOnCustom", VideoViewUsbRemovedOnCustom,
    "VideoViewRepeatButtonOnPress", VideoViewRepeatButtonOnPress,
    "VideoViewRandomCheckBoxOnPress", VideoViewRandomCheckBoxOnPress,
    "VideoViewLastButtonOnPress", VideoViewLastButtonOnPress,
    "VideoViewPlayCheckBoxOnPress", VideoViewPlayCheckBoxOnPress,
    "VideoViewNextButtonOnPress", VideoViewNextButtonOnPress,
    "VideoViewVolTrackBarOnChanged", VideoViewVolTrackBarOnChanged,
    "VideoViewOnTimer", VideoViewOnTimer,
    "VideoViewViewButtonOnPress", VideoViewViewButtonOnPress,
    //ImagePlayer
    "ImagePlayerOnEnter", ImagePlayerOnEnter,
    "ImagePlayerOnLeave", ImagePlayerOnLeave,
    "ImagePlayerSDInsertedOnCustom", ImagePlayerSDInsertedOnCustom,
    "ImagePlayerSDRemovedOnCustom", ImagePlayerSDRemovedOnCustom,
    "ImagePlayerUsbInsertedOnCustom", ImagePlayerUsbInsertedOnCustom,
    "ImagePlayerUsbRemovedOnCustom", ImagePlayerUsbRemovedOnCustom,
    "ImagePlayerStorageRadioBoxOnPress", ImagePlayerStorageRadioBoxOnPress,
    "ImagePlayerUsbRemovedOnCustom", ImagePlayerUsbRemovedOnCustom,
    "ImagePlayerStorageRadioBoxOnPress", ImagePlayerStorageRadioBoxOnPress,
    "ImagePlayerStorageTypeCheckBoxOnPress", ImagePlayerStorageTypeCheckBoxOnPress,
    "ImagePlayerScrollMediaFileListBoxOnSelection", ImagePlayerScrollMediaFileListBoxOnSelection,
    "ImagePlayerLastButtonOnPress", ImagePlayerLastButtonOnPress,
    "ImagePlayerNextButtonOnPress", ImagePlayerNextButtonOnPress,
    "ImagePlayerOnTimer", ImagePlayerOnTimer,
    //ImageView
    "ImageViewOnEnter", ImageViewOnEnter,
    "ImageViewOnLeave", ImageViewOnLeave,
    "ImageViewSDRemovedOnCustom", ImageViewSDRemovedOnCustom,
    "ImageViewUsbRemovedOnCustom", ImageViewUsbRemovedOnCustom,
    "ImageViewLastButtonOnPress", ImageViewLastButtonOnPress,
    "ImageViewNextButtonOnPress", ImageViewNextButtonOnPress,
    "ImageViewOnTimer", ImageViewOnTimer,
    "ImageViewViewButtonOnPress", ImageViewViewButtonOnPress,
    //VideoRecord
    "VideoRecordHangUpButtonOnPress", VideoRecordHangUpButtonOnPress,
    "VideoRecordOnTimer", VideoRecordOnTimer,
    "VideoRecordOnEnter", VideoRecordOnEnter,
    "VideoRecordOnLeave", VideoRecordOnLeave,

    NULL, NULL
};
