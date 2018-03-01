#include "ite/itu.h"

//Main
extern bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param);

//Calling
bool CallingConnectedOnCustom(ITUWidget* widget, char* param);
bool CallingEndOnCustom(ITUWidget* widget, char* param);
bool CallingOnEnter(ITUWidget* widget, char* param);
bool CallingOnLeave(ITUWidget* widget, char* param);


ITUActionFunction actionFunctions[] =
{
	//Main
	"MainNotifyNetworkOnCustom", MainNotifyNetworkOnCustom,
	
    //Calling
	"CallingConnectedOnCustom", CallingConnectedOnCustom,
	"CallingEndOnCustom", CallingEndOnCustom,
	"CallingOnEnter", CallingOnEnter,
    "CallingOnLeave", CallingOnLeave,
    
    NULL, NULL
};
