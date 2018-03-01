#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "scene.h"
#include "doorbell.h"

#ifdef CFG_BROADCAST_SERVER_ENABLE
#define broadcast_cmd_port   33333              //broadcast cmd port
#define broadcast_data_port  6666               //broadcast data port
#define broadcast_addr       "255.255.255.255" //broadcast addr

static bool broadcast_ongoing = false;
#endif

extern ITUBackgroundButton* phoneResidentRoomBackgroundButton;
extern ITUTextBox* phoneAreaTextBox;
extern ITUTextBox* phoneBuildingTextBox;
extern ITUTextBox* phoneUnitTextBox;
extern ITUTextBox* phoneFloorTextBox;
extern ITUTextBox* phoneRoomTextBox;

extern ITUBackground* phoneResidentIPBackground;
extern ITUTextBox* phoneIPTextBox;

static ITULayer* phoneResidentLayer;
static ITULayer* phoneAdminLayer;
static ITULayer* phoneContactLayer;
static ITULayer* phoneMiscLayer;
static ITUScrollListBox* phoneAdminScrollListBox;
static ITUScrollListBox* phoneMiscScrollListBox;

static ITUBackground* phoneContactAddBackground;
static ITUTextBox* phoneContactAddAreaTextBox;
static ITUTextBox* phoneContactAddBuildingTextBox;
static ITUTextBox* phoneContactAddUnitTextBox;
static ITUTextBox* phoneContactAddFloorTextBox;
static ITUTextBox* phoneContactAddRoomTextBox;

// status
extern ITUTextBox* activePhoneDialTextBox;
extern ITUTextBox* activePhoneContactAddTextBox;

#ifdef CFG_BROADCAST_SERVER_ENABLE
static bool Broadcast_voice(bool enable)
{
    int sockfd;                        
    struct sockaddr_in myaddr, broadcastAddr, remaddr; 
    int broadcastPermission = 1;         
    int slen=sizeof(remaddr);  
    int recvlen;		                                            /* # bytes in acknowledgement message */
    char *broadcast_buf = (enable == true) ? "Broadcast=1" : "Broadcast=0";	/* broadcast message buffer */
    char rtn_buf[32];                                               /* return message buffer*/    
    struct timeval timeout = {0};
    fd_set readfds;
    bool result = false;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("socket error\n");
    }

    /* bind it to all local addresses and pick any port number */	
    memset((char *)&myaddr, 0, sizeof(myaddr));	
    myaddr.sin_family = AF_INET;	
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);	
    myaddr.sin_port = htons(8888);	
    if (bind(sockfd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
    {		
        printf("bind failed\n");		
    }
        
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission)) < 0)
    {
        printf("setsockopt error\n");
    }
    
    /* Construct broadcast address structure */
    memset((char *)&broadcastAddr, 0, sizeof(broadcastAddr));   
    broadcastAddr.sin_family = AF_INET;                 
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcast_addr);
    broadcastAddr.sin_port = htons(broadcast_cmd_port);       

    //for(;;)
    {
        /* Broadcast sendString in datagram to clients */
        if (sendto(sockfd, broadcast_buf, strlen(broadcast_buf), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != strlen(broadcast_buf))
        {
            printf("sendto error\n");
        }
        /*set receive form return side with timeout*/
        FD_ZERO(&readfds);
        FD_SET(sockfd,&readfds);
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        select(sockfd+1,&readfds,NULL,NULL,&timeout);
        if(FD_ISSET(sockfd,&readfds))
        {
            /* receive an acknowledgement from the server, not get return addr*/
            //recvlen = recvfrom(sockfd, rtn_buf, 16, 0);
            /* now receive an acknowledgement from the server, get return addr */
            recvlen = recvfrom(sockfd, rtn_buf, 16, 0, (struct sockaddr *)&remaddr, &slen);                
            if (recvlen >= 0) 
            {     
                printf("in time ,left time %d s ,%d usec\n",timeout.tv_sec,timeout.tv_usec);
                rtn_buf[recvlen] = 0;	/* expect a printable string - terminate it */                        
                printf("received message: \"%s\"from %s\n", rtn_buf, inet_ntoa(remaddr.sin_addr));
                result = true;
            }
        }
        else
            printf("timeout ,left time %d s ,%d usec\n",timeout.tv_sec,timeout.tv_usec);
        //usleep(1000);
    } 
    close(sockfd);
    return result;
}
#endif //CFG_BROADCAST_SERVER_ENABLE

bool PhoneNumberButtonOnPress(ITUWidget* widget, char* param)
{
    if (!phoneResidentLayer)
    {
        phoneResidentLayer = ituSceneFindWidget(&theScene, "phoneResidentLayer");
        assert(phoneResidentLayer);

        phoneContactLayer = ituSceneFindWidget(&theScene, "phoneContactLayer");
        assert(phoneContactLayer);

        phoneMiscLayer = ituSceneFindWidget(&theScene, "phoneMiscLayer");
        assert(phoneMiscLayer);

        phoneContactAddBackground = ituSceneFindWidget(&theScene, "phoneContactAddBackground");
        assert(phoneContactAddBackground);

        phoneContactAddAreaTextBox = ituSceneFindWidget(&theScene, "phoneContactAddAreaTextBox");
        assert(phoneContactAddAreaTextBox);

        phoneContactAddBuildingTextBox = ituSceneFindWidget(&theScene, "phoneContactAddBuildingTextBox");
        assert(phoneContactAddBuildingTextBox);

        phoneContactAddUnitTextBox = ituSceneFindWidget(&theScene, "phoneContactAddUnitTextBox");
        assert(phoneContactAddUnitTextBox);

        phoneContactAddFloorTextBox = ituSceneFindWidget(&theScene, "phoneContactAddFloorTextBox");
        assert(phoneContactAddFloorTextBox);

        phoneContactAddRoomTextBox = ituSceneFindWidget(&theScene, "phoneContactAddRoomTextBox");
        assert(phoneContactAddRoomTextBox);

        activePhoneContactAddTextBox = phoneContactAddAreaTextBox;
    }

    if (ituWidgetIsVisible(phoneResidentLayer))
    {
        ITUButton* btn = (ITUButton*) widget;
        char* input = ituTextGetString(&btn->text);

        if (activePhoneDialTextBox == phoneIPTextBox)
        {
            if (*input == '*')
                *input = '.';
        }
        else
        {
            char* str = ituTextGetString(activePhoneDialTextBox);
            int count = str ? strlen(str) : 0;

            if (activePhoneDialTextBox == phoneAreaTextBox)
            {
                if (count == 0 && input[0] == '#')
                {
                    ituWidgetSetVisible(phoneResidentRoomBackgroundButton, false);
                    ituWidgetSetVisible(phoneResidentIPBackground, true);
                    activePhoneDialTextBox = phoneIPTextBox;
                    ituTextBoxInput(activePhoneDialTextBox, input);
                    return true;
                }
            }

            if (count >= activePhoneDialTextBox->maxLen)
            {
                if (activePhoneDialTextBox == phoneAreaTextBox)
                    activePhoneDialTextBox = phoneBuildingTextBox;
                else if (activePhoneDialTextBox == phoneBuildingTextBox)
                    activePhoneDialTextBox = phoneUnitTextBox;
                else if (activePhoneDialTextBox == phoneUnitTextBox)
                    activePhoneDialTextBox = phoneFloorTextBox;
                else if (activePhoneDialTextBox == phoneFloorTextBox)
                    activePhoneDialTextBox = phoneRoomTextBox;
            }
        }
        ituTextBoxInput(activePhoneDialTextBox, input);
    }
    else if (ituWidgetIsVisible(phoneContactLayer) && ituWidgetIsVisible(phoneContactAddBackground))
    {
        ITUButton* btn = (ITUButton*) widget;
        char* input = ituTextGetString(&btn->text);
        char* str = ituTextGetString(activePhoneContactAddTextBox);
        int count = str ? strlen(str) : 0;

        if (count >= activePhoneContactAddTextBox->maxLen)
        {
            if (activePhoneContactAddTextBox == phoneContactAddAreaTextBox)
                activePhoneContactAddTextBox = phoneContactAddBuildingTextBox;
            else if (activePhoneContactAddTextBox == phoneContactAddBuildingTextBox)
                activePhoneContactAddTextBox = phoneContactAddUnitTextBox;
            else if (activePhoneContactAddTextBox == phoneContactAddUnitTextBox)
                activePhoneContactAddTextBox = phoneContactAddFloorTextBox;
            else if (activePhoneContactAddTextBox == phoneContactAddFloorTextBox)
                activePhoneContactAddTextBox = phoneContactAddRoomTextBox;
        }
        ituTextBoxInput(activePhoneContactAddTextBox, input);
    }
    else
    {
        ituLayerGoto(phoneResidentLayer);
    }
	return true;
}

bool PhoneCallButtonOnPress(ITUWidget* widget, char* param)
{
    if (!phoneAdminLayer)
    {
        if (!phoneResidentLayer)
        {
            phoneResidentLayer = ituSceneFindWidget(&theScene, "phoneResidentLayer");
            assert(phoneResidentLayer);

            phoneContactLayer = ituSceneFindWidget(&theScene, "phoneContactLayer");
            assert(phoneContactLayer);

            phoneMiscLayer = ituSceneFindWidget(&theScene, "phoneMiscLayer");
            assert(phoneMiscLayer);

            phoneContactAddBackground = ituSceneFindWidget(&theScene, "phoneContactAddBackground");
            assert(phoneContactAddBackground);

            phoneContactAddAreaTextBox = ituSceneFindWidget(&theScene, "phoneContactAddAreaTextBox");
            assert(phoneContactAddAreaTextBox);

            phoneContactAddBuildingTextBox = ituSceneFindWidget(&theScene, "phoneContactAddBuildingTextBox");
            assert(phoneContactAddBuildingTextBox);

            phoneContactAddUnitTextBox = ituSceneFindWidget(&theScene, "phoneContactAddUnitTextBox");
            assert(phoneContactAddUnitTextBox);

            phoneContactAddFloorTextBox = ituSceneFindWidget(&theScene, "phoneContactAddFloorTextBox");
            assert(phoneContactAddFloorTextBox);

            phoneContactAddRoomTextBox = ituSceneFindWidget(&theScene, "phoneContactAddRoomTextBox");
            assert(phoneContactAddRoomTextBox);

            activePhoneContactAddTextBox = phoneContactAddAreaTextBox;
        }
        phoneAdminLayer = ituSceneFindWidget(&theScene, "phoneAdminLayer");
        assert(phoneAdminLayer);

        phoneAdminScrollListBox = ituSceneFindWidget(&theScene, "phoneAdminScrollListBox");
        assert(phoneAdminScrollListBox);

        phoneMiscScrollListBox = ituSceneFindWidget(&theScene, "phoneMiscScrollListBox");
        assert(phoneMiscScrollListBox);
    }

    if (ituWidgetIsVisible(phoneResidentLayer))
    {
        if (ituTextGetString(activePhoneDialTextBox))
        {
            if (activePhoneDialTextBox == phoneIPTextBox)
            {
                char* ip = ituTextGetString(phoneIPTextBox);

                if (CallingCallByIP(&ip[1]) == 0)
				    return true; //goto callingLayer
            }
            else if (activePhoneDialTextBox == phoneRoomTextBox)
            {
                if (CallingCall(
                    ituTextGetString(phoneAreaTextBox),
                    ituTextGetString(phoneBuildingTextBox),
                    ituTextGetString(phoneUnitTextBox),
                    ituTextGetString(phoneFloorTextBox),
                    ituTextGetString(phoneRoomTextBox),
                    NULL) == 0)
				    return true; // goto callingLayer
            }
        }
    }
    else if (ituWidgetIsVisible(phoneAdminLayer))
    {
        ITUWidget* item = ituListBoxGetFocusItem(&phoneAdminScrollListBox->listbox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

		    if (CallingCallManager(info->ip) == 0)
            {
			    return true; // goto callingLayer
		    }
        }
    }
    else if (ituWidgetIsVisible(phoneMiscLayer))
    {
        ITUWidget* item = ituListBoxGetFocusItem(&phoneMiscScrollListBox->listbox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

		    if (CallingCallByIP(info->ip) == 0)
            {
			    return true; // goto callingLayer
		    }
        }
    }
#ifdef CFG_BROADCAST_SERVER_ENABLE
    if(broadcast_ongoing == false)
    {
        if(Broadcast_voice(true))
        {
            LeafStartConnect(broadcast_addr, broadcast_data_port);
            broadcast_ongoing = true;
            return false; // goto callingLayer
        }
    }
    else
    {
        //printf("STOP!!!\n");
        Broadcast_voice(false);
        LeafTerminate();
        broadcast_ongoing = false;
    }
#endif //CFG_BROADCAST_SERVER_ENABLE  
	return false;
}

void PhoneReset(void)
{
    phoneResidentLayer = NULL;
    phoneAdminLayer = NULL;
}

