#include <sys/ioctl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "lwip/ip.h"
#include "lwip/dns.h"
#include "ite/itp.h"
#include "iniparser/iniparser.h"
#include "network.h"

#define DHCP_TIMEOUT_MSEC       (25 * 1000)
#define HEARTBEAT_TIMEOUT_SEC   (30 * 60)

static bool networkIsReady;
static bool networkServerIsReady;
static bool networkHeartbeatIsReady;
static time_t networkLastRecvTime;
static int networkSocket;
static bool networkToReset;

#ifdef CFG_NET_WIFI
static WIFI_MGR_SETTING gWifiSetting;
#endif
static  struct timeval tvStart = {0, 0}, tvEnd = {0, 0};
// wifi init
static int gInit =0;

bool NetworkIsReady(void)
{
    return networkIsReady;
}

bool NetworkServerIsReady(void)
{
    return networkIsReady && networkServerIsReady;
}

static void ResetEthernet(void)
{
	ITPEthernetSetting setting[CFG_NET_ETHERNET_COUNT];
    ITPEthernetInfo info;
    unsigned long mscnt = 0;
    char buf[16];
	int i;


	for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++){
		memset(&setting[i], 0, sizeof (ITPEthernetSetting));

		setting[i].index = i;
	}
#if (CFG_NET_ETHERNET_COUNT == 3)
/*---------------------eth_0 setting---------------------*/
    // dhcp
	setting[0].dhcp = theConfig.dhcp;

    // autoip
	setting[0].autoip = 0;

    // ipaddr
	strcpy(buf, "192.168.191.1");
	setting[0].ipaddr[0] = atoi(strtok(buf, "."));
	setting[0].ipaddr[1] = atoi(strtok(NULL, "."));
	setting[0].ipaddr[2] = atoi(strtok(NULL, "."));
	setting[0].ipaddr[3] = atoi(strtok(NULL, " "));

    // netmask
    strcpy(buf, theConfig.netmask);
	setting[0].netmask[0] = atoi(strtok(buf, "."));
	setting[0].netmask[1] = atoi(strtok(NULL, "."));
	setting[0].netmask[2] = atoi(strtok(NULL, "."));
	setting[0].netmask[3] = atoi(strtok(NULL, " "));

    // gateway
#ifdef CFG_NET_ETHERNET_MULTI_INTERFACE
	strcpy(buf, CFG_NET_ETHERNET_THIRD_GW);
	setting[0].gw[0] = atoi(strtok(buf, "."));
	setting[0].gw[1] = atoi(strtok(NULL, "."));
	setting[0].gw[2] = atoi(strtok(NULL, "."));
	setting[0].gw[3] = atoi(strtok(NULL, " "));
#endif

/*----------------------------------------------------*/



/*---------------------eth_1 setting---------------------*/

	setting[1].dhcp = theConfig.dhcp;

	setting[1].autoip = 0;

	strcpy(buf, "192.168.1.1");
	setting[1].ipaddr[0] = atoi(strtok(buf, "."));
	setting[1].ipaddr[1] = atoi(strtok(NULL, "."));
	setting[1].ipaddr[2] = atoi(strtok(NULL, "."));
	setting[1].ipaddr[3] = atoi(strtok(NULL, " "));

	strcpy(buf, theConfig.netmask);
	setting[1].netmask[0] = atoi(strtok(buf, "."));
	setting[1].netmask[1] = atoi(strtok(NULL, "."));
	setting[1].netmask[2] = atoi(strtok(NULL, "."));
	setting[1].netmask[3] = atoi(strtok(NULL, " "));

#ifdef CFG_NET_ETHERNET_MULTI_INTERFACE
	strcpy(buf, CFG_NET_ETHERNET_SECOND_GW);
	setting[1].gw[0] = atoi(strtok(buf, "."));
	setting[1].gw[1] = atoi(strtok(NULL, "."));
	setting[1].gw[2] = atoi(strtok(NULL, "."));
	setting[1].gw[3] = atoi(strtok(NULL, " "));
#endif

/*----------------------------------------------------*/


/*---------------------eth_2 setting---------------------*/

	setting[2].dhcp = theConfig.dhcp;

	setting[2].autoip = 0;

	strcpy(buf, theConfig.ipaddr);
	setting[2].ipaddr[0] = atoi(strtok(buf, "."));
	setting[2].ipaddr[1] = atoi(strtok(NULL, "."));
	setting[2].ipaddr[2] = atoi(strtok(NULL, "."));
	setting[2].ipaddr[3] = atoi(strtok(NULL, " "));

	strcpy(buf, theConfig.netmask);
	setting[2].netmask[0] = atoi(strtok(buf, "."));
	setting[2].netmask[1] = atoi(strtok(NULL, "."));
	setting[2].netmask[2] = atoi(strtok(NULL, "."));
	setting[2].netmask[3] = atoi(strtok(NULL, " "));

	strcpy(buf, theConfig.gw);
	setting[2].gw[0] = atoi(strtok(buf, "."));
	setting[2].gw[1] = atoi(strtok(NULL, "."));
	setting[2].gw[2] = atoi(strtok(NULL, "."));
	setting[2].gw[3] = atoi(strtok(NULL, " "));

/*----------------------------------------------------*/


	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);
	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[1]);
	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[2]);
#elif (CFG_NET_ETHERNET_COUNT == 1)
    // dhcp
	setting[0].dhcp = 0;

    // autoip
	setting[0].autoip = 0;

    // ipaddr
	strcpy(buf, TEST_IPADDR);
	setting[0].ipaddr[0] = atoi(strtok(buf, "."));
	setting[0].ipaddr[1] = atoi(strtok(NULL, "."));
	setting[0].ipaddr[2] = atoi(strtok(NULL, "."));
	setting[0].ipaddr[3] = atoi(strtok(NULL, " "));

    // netmask
    strcpy(buf, TEST_NETMASK);
	setting[0].netmask[0] = atoi(strtok(buf, "."));
	setting[0].netmask[1] = atoi(strtok(NULL, "."));
	setting[0].netmask[2] = atoi(strtok(NULL, "."));
	setting[0].netmask[3] = atoi(strtok(NULL, " "));

    // gateway
	strcpy(buf, TEST_GATEWAY);
	setting[0].gw[0] = atoi(strtok(buf, "."));
	setting[0].gw[1] = atoi(strtok(NULL, "."));
	setting[0].gw[2] = atoi(strtok(NULL, "."));
	setting[0].gw[3] = atoi(strtok(NULL, " "));

	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);
#endif



	printf("Wait ethernet cable to plugin...\n");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL))
    {
        sleep(1);
        putchar('.');
        fflush(stdout);
    }

    printf("\nWait DHCP settings");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        usleep(100000);
        mscnt += 100;

        putchar('.');
        fflush(stdout);

        if (mscnt >= DHCP_TIMEOUT_MSEC)
        {
            printf("\nDHCP timeout, use default settings\n");

			#if (CFG_NET_ETHERNET_COUNT == 3)
				for (i=0; i < CFG_NET_ETHERNET_COUNT; i++)
				{
					setting[i].dhcp = setting[i].autoip = 0;
					ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[i]);
				}

			#elif (CFG_NET_ETHERNET_COUNT == 1)
			setting[0].dhcp = setting[0].autoip = 0;
			ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);
			#endif
            break;
        }
    }
    puts("");

    if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        char* ip;

        info.index = 0;
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &info);
        ip = ipaddr_ntoa((const ip_addr_t*)&info.address);

		printf("IP address[0]: %s\n", ip);

    #if CFG_SNTP_UPDATE_DELAY > 0
        {
            DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
            int i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

            for (i = 0; i < count; i++)
                sntp_add_server_address(managerInfoArray[i].ip);

            if (theCenterInfo.ip[0] != '\0')
                sntp_add_server_address(theCenterInfo.ip);

            sntp_set_update_delay(CFG_SNTP_UPDATE_DELAY);
            sntp_set_port(CFG_SNTP_PORT);
            sntp_init();
        }
    #endif // CFG_SNTP_UPDATE_DELAY > 0

        networkIsReady = true;

    }
}


#ifdef CFG_NET_WIFI
static int wifiCallbackFucntion(int nState)
{
    switch (nState)
    {
        case WIFIMGR_STATE_CALLBACK_CONNECTION_FINISH:
            printf("[Indoor]WifiCallback connection finish \n");
            WebServerInit();
        break;
        
        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_DISCONNECT_30S:
            printf("[Indoor]WifiCallback connection disconnect 30s \n");
        break;
        
        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_RECONNECTION:
            printf("[Indoor]WifiCallback connection reconnection \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_TEMP_DISCONNECT:
            printf("[Indoor]WifiCallback connection temp disconnect \n");
        break;

        case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_FAIL:
            printf("[Indoor]WifiCallback connecting fail, please check ssid,password,secmode \n");
        break;

        default:
            printf("[Indoor]WifiCallback unknown %d state  \n",nState);
        break;

    }


}


static ResetWifi()
{
//    ITPEthernetSetting setting;
    char buf[16];

    memset(&gWifiSetting.setting, 0, sizeof (ITPEthernetSetting));

    gWifiSetting.setting.index = 0;

    // dhcp
    gWifiSetting.setting.dhcp = theConfig.dhcp;

    // autoip
    gWifiSetting.setting.autoip = 0;

    // ipaddr
    strcpy(buf, theConfig.ipaddr);
    gWifiSetting.setting.ipaddr[0] = atoi(strtok(buf, "."));
    gWifiSetting.setting.ipaddr[1] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.ipaddr[2] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.ipaddr[3] = atoi(strtok(NULL, " "));

    // netmask
    strcpy(buf, theConfig.netmask);
    gWifiSetting.setting.netmask[0] = atoi(strtok(buf, "."));
    gWifiSetting.setting.netmask[1] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.netmask[2] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.netmask[3] = atoi(strtok(NULL, " "));

    // gateway
    strcpy(buf, theConfig.gw);
    gWifiSetting.setting.gw[0] = atoi(strtok(buf, "."));
    gWifiSetting.setting.gw[1] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.gw[2] = atoi(strtok(NULL, "."));
    gWifiSetting.setting.gw[3] = atoi(strtok(NULL, " "));



}


#endif

#ifdef CFG_UDP_HEARTBEAT

static void NetworkInitHeartbeat(void)
{
    struct sockaddr_in sockAddr;
    int val = 1;

    networkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (networkSocket == -1)
        return;

	ioctlsocket(networkSocket, FIONBIO, &val);

    memset((char*)&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port = htons(CFG_UDP_HEARTBEAT_PORT);

    if (bind(networkSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
    {
        closesocket(networkSocket);
        networkSocket = -1;
        return;
    }
    networkHeartbeatIsReady = true;
}

static void NetworkResetHeartbeat(void)
{
    if (networkHeartbeatIsReady && networkSocket != -1)
    {
        closesocket(networkSocket);
        networkSocket = -1;
    }
    NetworkInitHeartbeat();
}
#endif // CFG_UDP_HEARTBEAT

static void* NetworkTask(void* arg)
{
#ifdef CFG_NET_WIFI

    int nTemp;    
#else
    ResetEthernet();
#endif

    for (;;)
    {


#ifdef CFG_NET_WIFI
        gettimeofday(&tvEnd, NULL);

        nTemp = itpTimevalDiff(&tvStart, &tvEnd);
        if (nTemp>5000 && gInit == 0){
            printf("init wifi \n");
            nTemp = wifiMgr_init(WIFIMGR_MODE_CLIENT, 0,gWifiSetting);
            gInit = 1;

        } else if (gInit == 1){
            networkIsReady = wifiMgr_is_wifi_available(&nTemp);
            networkIsReady = (bool)nTemp;
        }            
        
#else        
        networkIsReady = ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL);        
#endif

        // check center server connection
        if (networkIsReady)
        {        
            time_t now = time(NULL);
            struct sockaddr_in sockAddr;
            int slen = sizeof(sockAddr);

        #ifdef CFG_UDP_HEARTBEAT
            int count, diff;
            char buf[4];

            if (!networkHeartbeatIsReady)
                NetworkInitHeartbeat();

            // check center is alive or not
            diff = (int) difftime(now, networkLastRecvTime);
            if (diff >= HEARTBEAT_TIMEOUT_SEC)
            {
                networkServerIsReady = false;
            }

            count = recvfrom(networkSocket, buf, 1, 0, (struct sockaddr*)&sockAddr, &slen);
            if (count > 0)
            {
                networkServerIsReady = true;
                networkLastRecvTime = now;

                // send heartbeat packet back to center
                //sockAddr.sin_family = AF_INET;
                //sockAddr.sin_addr.s_addr = inet_addr(theCenterInfo.ip);

                count = sendto(networkSocket, "*", 1, 0, (const struct sockaddr*)&sockAddr, sizeof (sockAddr));
            }
        #endif // CFG_UDP_HEARTBEAT
        }

        if (networkToReset)
        {
        #ifdef CFG_NET_WIFI

        #else
            ResetEthernet();
        #endif
        
        #ifdef CFG_UDP_HEARTBEAT
            NetworkResetHeartbeat();
        #endif
            networkToReset = false;
        }

        sleep(1);
    }
    return NULL;
}

void NetworkSntpUpdate(void)
{
#if CFG_SNTP_UPDATE_DELAY > 0

    DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
    int i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

    sntp_clear_server_addresses();

    for (i = 0; i < count; i++)
        sntp_add_server_address(managerInfoArray[i].ip);

    if (theCenterInfo.ip[0] != '\0')
        sntp_add_server_address(theCenterInfo.ip);

    sntp_update();
#endif // CFG_SNTP_UPDATE_DELAY > 0
}

void NetworkInit(void)
{
    pthread_t task;
    int ret = 0;

    networkIsReady          = false;
    networkServerIsReady    = true;
#ifdef CFG_UDP_HEARTBEAT
    networkHeartbeatIsReady = false;
#endif
    networkToReset          = false;
    networkSocket           = -1;
    networkLastRecvTime     = time(NULL);

#ifdef CFG_NET_WIFI

    snprintf(gWifiSetting.ssid , WIFI_SSID_MAXLEN, theConfig.ssid);
    snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN, theConfig.password);
    snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN, theConfig.secumode);
    gWifiSetting.wifiCallback = wifiCallbackFucntion;
    ResetWifi();
    // init client mode
  //ret = wifiMgr_init(WIFIMGR_MODE_CLIENT, 0,gWifiSetting);
    
//    ret = wifiMgr_init(WIFIMGR_MODE_SOFTAP, 0,gWifiSetting);        
    gettimeofday(&tvStart, NULL);


#endif
    pthread_create(&task, NULL, NetworkTask, NULL);
}

void NetworkReset(void)
{
    networkToReset  = true;
}
