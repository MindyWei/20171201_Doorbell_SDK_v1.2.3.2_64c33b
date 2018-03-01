#include "ite/itu.h"

//MainMenu
extern bool MainMenuOnEnter(ITUWidget* widget, char* param);

//Camera
extern bool CameraScrollListBoxOnLoad(ITUWidget* widget, char* param);
extern bool CameraScrollListOnSelection(ITUWidget* widget, char* param);
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

ITUActionFunction actionFunctions[] =
{
    //MainMenu
    "MainMenuOnEnter", MainMenuOnEnter,
    //Camera
    "CameraScrollListBoxOnLoad", CameraScrollListBoxOnLoad,
    "CameraScrollListOnSelection", CameraScrollListOnSelection,
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

    NULL, NULL
};
