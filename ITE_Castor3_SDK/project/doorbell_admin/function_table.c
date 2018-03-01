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
extern bool MainNotifyCallIncomingOnCustom(ITUWidget* widget, char* param);

//Login
extern bool LoginKeyboardEnterButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardBackButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardChsButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param);
extern bool LoginOnEnter(ITUWidget* widget, char* param);
extern bool LoginOnLeave(ITUWidget* widget, char* param);

//MainMenu
extern bool MainSetDoNotDisturbModeOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyMissedCallsOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyGuardMessagesOnCustom(ITUWidget* widget, char* param);
extern bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param);
extern bool MainMenuOnEnter(ITUWidget* widget, char* param);
extern bool MainMenuExitPopupButtonOnPress(ITUWidget* widget, char* param);
extern bool MainMenuDisturbPopupButtonOnPress(ITUWidget* widget, char* param);

//Phone
extern bool PhoneNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneCallButtonOnPress(ITUWidget* widget, char* param);

//PhoneResident
extern bool PhoneResidentOnEnter(ITUWidget* widget, char* param);
extern bool PhoneResidentOnLeave(ITUWidget* widget, char* param);
extern bool PhoneBackButtonOnPress(ITUWidget* widget, char* param);
extern bool PhoneLogIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneLogListBoxOnLoad(ITUWidget* widget, char* param);
extern bool PhoneLogListBoxOnSelection(ITUWidget* widget, char* param);

//PhoneAdmin
extern bool PhoneAdminOnEnter(ITUWidget* widget, char* param);
extern bool PhoneAdminListBoxOnLoad(ITUWidget* widget, char* param);

//PhoneMisc
extern bool PhoneMiscOnEnter(ITUWidget* widget, char* param);
extern bool PhoneMiscListBoxOnLoad(ITUWidget* widget, char* param);

//Calling
extern bool CallingVolCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CallingVolNumberCheckBoxOnPress(ITUWidget* widget, char* param);
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
extern bool CallingVideoMemoButtonOnPress(ITUWidget* widget, char* param);

//Called
extern bool CalledVolCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CalledVolNumberCheckBoxOnPress(ITUWidget* widget, char* param);
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

//Redirect
extern bool RedirectNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool RedirectCallButtonOnPress(ITUWidget* widget, char* param);

//RedirectResident
extern bool RedirectResidentOnEnter(ITUWidget* widget, char* param);
extern bool RedirectResidentOnLeave(ITUWidget* widget, char* param);
extern bool RedirectBackButtonOnPress(ITUWidget* widget, char* param);
extern bool RedirectLogIconListBoxOnLoad(ITUWidget* widget, char* param);
extern bool RedirectLogListBoxOnLoad(ITUWidget* widget, char* param);
extern bool RedirectLogListBoxOnSelection(ITUWidget* widget, char* param);

//RedirectAdmin
extern bool RedirectAdminOnEnter(ITUWidget* widget, char* param);
extern bool RedirectAdminOnLeave(ITUWidget* widget, char* param);
extern bool RedirectAdminListBoxOnLoad(ITUWidget* widget, char* param);

//RedirectMisc
extern bool RedirectMiscOnEnter(ITUWidget* widget, char* param);
extern bool RedirectMiscOnLeave(ITUWidget* widget, char* param);
extern bool RedirectMiscListBoxOnLoad(ITUWidget* widget, char* param);

//Camera
extern bool CameraListBoxOnLoad(ITUWidget* widget, char* param);
extern bool CameraListOnSelection(ITUWidget* widget, char* param);
extern bool CameraScreenButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraOnEnter(ITUWidget* widget, char* param);
extern bool CameraOnLeave(ITUWidget* widget, char* param);
extern bool CameraOnTimer(ITUWidget* widget, char* param);
extern bool CameraRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraBindingCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CameraConnectButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraVolCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CameraVolNumberCheckBoxOnPress(ITUWidget* widget, char* param);

//CameraView
extern bool CameraViewOnEnter(ITUWidget* widget, char* param);
extern bool CameraViewOnLeave(ITUWidget* widget, char* param);
extern bool CameraViewScreenButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewSnapshotButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewBindingCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CameraViewConnectButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraViewVolCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool CameraViewVolNumberCheckBoxOnPress(ITUWidget* widget, char* param);

//GuardLog
extern bool GuardLogTopicListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogTimeListBoxOnLoad(ITUWidget* widget, char* param);
extern bool GuardLogViewButtonOnPress(ITUWidget* widget, char* param);
extern bool GuardLogDeleteOnPress(ITUWidget* widget, char* param);
extern bool GuardLogOnEnter(ITUWidget* widget, char* param);
extern bool GuardLogOnLeave(ITUWidget* widget, char* param);

//GuardLogView
extern bool GuardLogViewOnEnter(ITUWidget* widget, char* param);
extern bool GuardLogViewOnLeave(ITUWidget* widget, char* param);
extern bool GuardLogViewHandleButtonOnPress(ITUWidget* widget, char* param);

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

//SettingTerminal
extern bool SettingTerminalBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingTerminalOnEnter(ITUWidget* widget, char* param);

//SettingDoorCamera
extern bool SettingDoorCameraRoomAddrBackButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraPrevButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraNextButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraCancelButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnTimer(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnEnter(ITUWidget* widget, char* param);
extern bool SettingDoorCameraOnLeave(ITUWidget* widget, char* param);

//SettingLang
extern bool SettingLangOnEnter(ITUWidget* widget, char* param);
extern bool SettingLangOnLeave(ITUWidget* widget, char* param);
extern bool SettingLangChtRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingLangChsRadioBoxOnPress(ITUWidget* widget, char* param);
extern bool SettingLangEngRadioBoxOnPress(ITUWidget* widget, char* param);

//SettingMisc
extern bool SettingMiscOnEnter(ITUWidget* widget, char* param);
extern bool SettingMiscOnLeave(ITUWidget* widget, char* param);
extern bool SettingMiscConfirmButtonOnPress(ITUWidget* widget, char* param);

//SettingSysInfo
extern bool SettingSysInfoOnEnter(ITUWidget* widget, char* param);

//Time
extern bool TimeConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool TimeOnEnter(ITUWidget* widget, char* param);

//Date
extern bool DateConfirmButtonOnPress(ITUWidget* widget, char* param);
extern bool DateOnEnter(ITUWidget* widget, char* param);

//Account
extern bool AccountListBoxOnLoad(ITUWidget* widget, char* param);
extern bool AccountListBoxOnSelection(ITUWidget* widget, char* param);
extern bool AccountDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountOnEnter(ITUWidget* widget, char* param);

//AccountKeyboard
extern bool AccountKeyboardEnterButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardBackButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardChsButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param);
extern bool AccountKeyboardOnEnter(ITUWidget* widget, char* param);
extern bool AccountKeyboardOnLeave(ITUWidget* widget, char* param);

//Record
extern bool RecordTopicListBoxOnLoad(ITUWidget* widget, char* param);
extern bool RecordTimeListBoxOnLoad(ITUWidget* widget, char* param);
extern bool RecordViewButtonOnPress(ITUWidget* widget, char* param);
extern bool RecordDeleteButtonOnPress(ITUWidget* widget, char* param);
extern bool RecordOnEnter(ITUWidget* widget, char* param);

//RecordView
extern bool RecordViewVolCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool RecordViewVolNumberCheckBoxOnPress(ITUWidget* widget, char* param);
extern bool RecordViewOnTimer(ITUWidget* widget, char* param);
extern bool RecordViewOnEnter(ITUWidget* widget, char* param);
extern bool RecordViewOnLeave(ITUWidget* widget, char* param);

//VideoMemo
extern bool VideoMemoConnectedOnCustom(ITUWidget* widget, char* param);
extern bool VideoMemoErrorOnCustom(ITUWidget* widget, char* param);
extern bool VideoMemoHangUpButtonOnPress(ITUWidget* widget, char* param);
extern bool VideoMemoOnTimer(ITUWidget* widget, char* param);
extern bool VideoMemoOnEnter(ITUWidget* widget, char* param);

//CameraLobby
extern bool CameraLobbyOnEnter(ITUWidget* widget, char* param);
extern bool CameraLobbyOnLeave(ITUWidget* widget, char* param);
extern bool CameraLobbyOnTimer(ITUWidget* widget, char* param);
extern bool CameraLobbyButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraLobbyRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraLobbyStopRecordButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraLobbyUnlockButtonOnPress(ITUWidget* widget, char* param);
extern bool CameraLobbySnapshotButtonOnPress(ITUWidget* widget, char* param);

//Error
extern bool ErrorOnEnter(ITUWidget* widget, char* param);
extern bool ErrorOnLeave(ITUWidget* widget, char* param);

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
	"MainNotifyCallIncomingOnCustom", MainNotifyCallIncomingOnCustom,
    //Login
    "LoginKeyboardEnterButtonOnPress", LoginKeyboardEnterButtonOnPress,
    "LoginKeyboardBackButtonOnPress", LoginKeyboardBackButtonOnPress,
    "LoginKeyboardEnUpperButtonOnPress", LoginKeyboardEnUpperButtonOnPress,
    "LoginKeyboardChsButtonOnPress", LoginKeyboardChsButtonOnPress,
    "LoginKeyboardPageUpButtonOnPress", LoginKeyboardPageUpButtonOnPress,
    "LoginKeyboardPageDownButtonOnPress", LoginKeyboardPageDownButtonOnPress,
    "LoginKeyboardChsCharButtonOnPress", LoginKeyboardChsCharButtonOnPress,
    "LoginOnEnter", LoginOnEnter,
    "LoginOnLeave", LoginOnLeave,
    //MainMenu
	"MainSetDoNotDisturbModeOnCustom", MainSetDoNotDisturbModeOnCustom,
    "MainNotifyMissedCallsOnCustom", MainNotifyMissedCallsOnCustom,
    "MainNotifyGuardMessagesOnCustom", MainNotifyGuardMessagesOnCustom,
	"MainNotifyNetworkOnCustom", MainNotifyNetworkOnCustom,
    "MainMenuOnEnter", MainMenuOnEnter,
    "MainMenuExitPopupButtonOnPress", MainMenuExitPopupButtonOnPress,
	"MainMenuDisturbPopupButtonOnPress", MainMenuDisturbPopupButtonOnPress,
    //Phone
    "PhoneNumberButtonOnPress", PhoneNumberButtonOnPress,
    "PhoneCallButtonOnPress", PhoneCallButtonOnPress,
    //PhoneResident
    "PhoneResidentOnEnter", PhoneResidentOnEnter,
    "PhoneResidentOnLeave", PhoneResidentOnLeave,
    "PhoneBackButtonOnPress", PhoneBackButtonOnPress,
    "PhoneLogIconListBoxOnLoad", PhoneLogIconListBoxOnLoad,
    "PhoneLogListBoxOnLoad", PhoneLogListBoxOnLoad,
    "PhoneLogListBoxOnSelection", PhoneLogListBoxOnSelection,
    //PhoneAdmin
    "PhoneAdminOnEnter", PhoneAdminOnEnter,
    "PhoneAdminListBoxOnLoad", PhoneAdminListBoxOnLoad,
    //PhoneMisc
    "PhoneMiscOnEnter", PhoneMiscOnEnter,
    "PhoneMiscListBoxOnLoad", PhoneMiscListBoxOnLoad,
	//Calling
    "CallingVolCheckBoxOnPress", CallingVolCheckBoxOnPress,
    "CallingVolNumberCheckBoxOnPress", CallingVolNumberCheckBoxOnPress,
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
    "CallingVideoMemoButtonOnPress", CallingVideoMemoButtonOnPress,
	//Called
    "CalledVolCheckBoxOnPress", CalledVolCheckBoxOnPress,
    "CalledVolNumberCheckBoxOnPress", CalledVolNumberCheckBoxOnPress,
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
    //Redirect
    "RedirectNumberButtonOnPress", RedirectNumberButtonOnPress,
    "RedirectCallButtonOnPress", RedirectCallButtonOnPress,
    //RedirectResident
    "RedirectResidentOnEnter", RedirectResidentOnEnter,
    "RedirectResidentOnLeave", RedirectResidentOnLeave,
    "RedirectBackButtonOnPress", RedirectBackButtonOnPress,
    "RedirectLogIconListBoxOnLoad", RedirectLogIconListBoxOnLoad,
    "RedirectLogListBoxOnLoad", RedirectLogListBoxOnLoad,
    "RedirectLogListBoxOnSelection", RedirectLogListBoxOnSelection,
    //RedirectAdmin
    "RedirectAdminOnEnter", RedirectAdminOnEnter,
    "RedirectAdminOnLeave", RedirectAdminOnLeave,
    "RedirectAdminListBoxOnLoad", RedirectAdminListBoxOnLoad,
    //RedirectMisc
    "RedirectMiscOnEnter", RedirectMiscOnEnter,
    "RedirectMiscOnLeave", RedirectMiscOnLeave,
    "RedirectMiscListBoxOnLoad", RedirectMiscListBoxOnLoad,
    //Camera
	"CameraListBoxOnLoad", CameraListBoxOnLoad,
	"CameraListOnSelection", CameraListOnSelection,
	"CameraScreenButtonOnPress", CameraScreenButtonOnPress,
	"CameraOnEnter", CameraOnEnter,
	"CameraOnLeave", CameraOnLeave,
    "CameraOnTimer", CameraOnTimer,
    "CameraRecordButtonOnPress", CameraRecordButtonOnPress,
    "CameraStopRecordButtonOnPress", CameraStopRecordButtonOnPress,
    "CameraUnlockButtonOnPress", CameraUnlockButtonOnPress,
    "CameraSnapshotButtonOnPress", CameraSnapshotButtonOnPress,
    "CameraBindingCheckBoxOnPress", CameraBindingCheckBoxOnPress,
    "CameraConnectButtonOnPress", CameraConnectButtonOnPress,
    "CameraVolCheckBoxOnPress", CameraVolCheckBoxOnPress,
    "CameraVolNumberCheckBoxOnPress", CameraVolNumberCheckBoxOnPress,
    //CameraView
    "CameraViewOnEnter", CameraViewOnEnter,
    "CameraViewOnLeave", CameraViewOnLeave,
	"CameraViewScreenButtonOnPress", CameraViewScreenButtonOnPress,
    "CameraViewRecordButtonOnPress", CameraViewRecordButtonOnPress,
    "CameraViewStopRecordButtonOnPress", CameraViewStopRecordButtonOnPress,
    "CameraViewUnlockButtonOnPress", CameraViewUnlockButtonOnPress,
    "CameraViewSnapshotButtonOnPress", CameraViewSnapshotButtonOnPress,
    "CameraViewBindingCheckBoxOnPress", CameraViewBindingCheckBoxOnPress,
    "CameraViewConnectButtonOnPress", CameraViewConnectButtonOnPress,
    "CameraViewVolCheckBoxOnPress", CameraViewVolCheckBoxOnPress,
    "CameraViewVolNumberCheckBoxOnPress", CameraViewVolNumberCheckBoxOnPress,
    //GuardLog
	"GuardLogTopicListBoxOnLoad", GuardLogTopicListBoxOnLoad,
	"GuardLogTimeListBoxOnLoad", GuardLogTimeListBoxOnLoad,
	"GuardLogViewButtonOnPress", GuardLogViewButtonOnPress,
    "GuardLogDeleteOnPress", GuardLogDeleteOnPress,
	"GuardLogOnEnter", GuardLogOnEnter,
	"GuardLogOnLeave", GuardLogOnLeave,
    //GuardLogView
	"GuardLogViewOnEnter", GuardLogViewOnEnter, 
	"GuardLogViewOnLeave", GuardLogViewOnLeave, 
    "GuardLogViewHandleButtonOnPress", GuardLogViewHandleButtonOnPress,
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
    //SettingTerminal
    "SettingTerminalBackButtonOnPress", SettingTerminalBackButtonOnPress, 
    "SettingTerminalConfirmButtonOnPress", SettingTerminalConfirmButtonOnPress, 
    "SettingTerminalCancelButtonOnPress", SettingTerminalCancelButtonOnPress, 
    "SettingTerminalNumberButtonOnPress", SettingTerminalNumberButtonOnPress, 
    "SettingTerminalOnEnter", SettingTerminalOnEnter, 
    //SettingDoorCamera
	"SettingDoorCameraRoomAddrBackButtonOnPress", SettingDoorCameraRoomAddrBackButtonOnPress, 
    "SettingDoorCameraPrevButtonOnPress", SettingDoorCameraPrevButtonOnPress, 
	"SettingDoorCameraNextButtonOnPress", SettingDoorCameraNextButtonOnPress, 
	"SettingDoorCameraConfirmButtonOnPress", SettingDoorCameraConfirmButtonOnPress,         
    "SettingDoorCameraCancelButtonOnPress", SettingDoorCameraCancelButtonOnPress,         
    "SettingDoorCameraNumberButtonOnPress", SettingDoorCameraNumberButtonOnPress,
    "SettingDoorCameraOnTimer", SettingDoorCameraOnTimer,
    "SettingDoorCameraOnEnter", SettingDoorCameraOnEnter,         
    "SettingDoorCameraOnLeave", SettingDoorCameraOnLeave,         
    //SettingLang
    "SettingLangOnEnter", SettingLangOnEnter,         
    "SettingLangOnLeave", SettingLangOnLeave,         
    "SettingLangChtRadioBoxOnPress", SettingLangChtRadioBoxOnPress,         
    "SettingLangChsRadioBoxOnPress", SettingLangChsRadioBoxOnPress,         
    "SettingLangEngRadioBoxOnPress", SettingLangEngRadioBoxOnPress,         
    //SettingMisc
    "SettingMiscOnEnter", SettingMiscOnEnter,         
    "SettingMiscOnLeave", SettingMiscOnLeave,         
    "SettingMiscConfirmButtonOnPress", SettingMiscConfirmButtonOnPress,         
    //SettingSysInfo
    "SettingSysInfoOnEnter", SettingSysInfoOnEnter,         
    //Time
    "TimeConfirmButtonOnPress", TimeConfirmButtonOnPress,         
    "TimeOnEnter", TimeOnEnter,         
    //Date
    "DateConfirmButtonOnPress", DateConfirmButtonOnPress,         
    "DateOnEnter", DateOnEnter,         
    //Account
    "AccountListBoxOnLoad", AccountListBoxOnLoad,         
    "AccountListBoxOnSelection", AccountListBoxOnSelection,         
    "AccountDeleteButtonOnPress", AccountDeleteButtonOnPress,         
    "AccountOnEnter", AccountOnEnter,         
    //AccountKeyboard
    "AccountKeyboardEnterButtonOnPress", AccountKeyboardEnterButtonOnPress,
    "AccountKeyboardBackButtonOnPress", AccountKeyboardBackButtonOnPress,
    "AccountKeyboardEnUpperButtonOnPress", AccountKeyboardEnUpperButtonOnPress,
    "AccountKeyboardChsButtonOnPress", AccountKeyboardChsButtonOnPress,
    "AccountKeyboardPageUpButtonOnPress", AccountKeyboardPageUpButtonOnPress,
    "AccountKeyboardPageDownButtonOnPress", AccountKeyboardPageDownButtonOnPress,
    "AccountKeyboardChsCharButtonOnPress", AccountKeyboardChsCharButtonOnPress,
    "AccountKeyboardOnEnter", AccountKeyboardOnEnter,
    "AccountKeyboardOnLeave", AccountKeyboardOnLeave,
    //Record
    "RecordTopicListBoxOnLoad", RecordTopicListBoxOnLoad,
    "RecordTimeListBoxOnLoad", RecordTimeListBoxOnLoad,
    "RecordViewButtonOnPress", RecordViewButtonOnPress,
    "RecordDeleteButtonOnPress", RecordDeleteButtonOnPress,
    "RecordOnEnter", RecordOnEnter,
    //RecordView
    "RecordViewVolCheckBoxOnPress", RecordViewVolCheckBoxOnPress,
    "RecordViewVolNumberCheckBoxOnPress", RecordViewVolNumberCheckBoxOnPress,
    "RecordViewOnTimer", RecordViewOnTimer,
    "RecordViewOnEnter", RecordViewOnEnter,
    "RecordViewOnLeave", RecordViewOnLeave,
    //VideoMemo
    "VideoMemoConnectedOnCustom", VideoMemoConnectedOnCustom,
    "VideoMemoErrorOnCustom", VideoMemoErrorOnCustom,
    "VideoMemoHangUpButtonOnPress", VideoMemoHangUpButtonOnPress,
    "VideoMemoOnTimer", VideoMemoOnTimer,
    "VideoMemoOnEnter", VideoMemoOnEnter,
    //CameraLobby
	"CameraLobbyOnEnter", CameraLobbyOnEnter,
	"CameraLobbyOnLeave", CameraLobbyOnLeave,
    "CameraLobbyOnTimer", CameraLobbyOnTimer,
	"CameraLobbyButtonOnPress", CameraLobbyButtonOnPress,
    "CameraLobbyRecordButtonOnPress", CameraLobbyRecordButtonOnPress,
    "CameraLobbyStopRecordButtonOnPress", CameraLobbyStopRecordButtonOnPress,
    "CameraLobbyUnlockButtonOnPress", CameraLobbyUnlockButtonOnPress,
    "CameraLobbySnapshotButtonOnPress", CameraLobbySnapshotButtonOnPress,
    //Error
	"ErrorOnEnter", ErrorOnEnter,
    "ErrorOnLeave", ErrorOnLeave,

    NULL, NULL
};
